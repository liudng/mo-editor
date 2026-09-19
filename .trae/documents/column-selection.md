# 实现列选择（矩形选区）功能

## Context（背景）

mo-editor 目前只支持 QPlainTextEdit 的常规流式选择。用户希望像 Notepad++ 那样，用 **Alt+Shift+方向键**（或编辑菜单项）开始**列选择**（矩形/垂直选区），用于同时对多行进行编辑（输入、删除、复制等）。

冲突检查：Qt/QPlainTextEdit 默认不绑定 Alt+Shift+方向键；本项目现有快捷键（Ctrl+Z/X/C/V、F3、Ctrl+F 等）均不占用；主流 Linux 桌面（GNOME/KDE 默认配置）也不占用该组合（KDE 用 Ctrl+Alt+方向键切桌面、Meta+方向键平铺窗口）。可用。

## 行为设计（Notepad++ / VS Code 风格）

- **Alt+Shift+↑/↓/←/→**：未激活时，以当前光标为锚点开始列选择；已激活时，移动"活动角"（四个方向可自由移动，矩形可翻转）。列号为虚拟列（可超过短行行尾，渲染/编辑时按行 clamp）。
- **编辑菜单 → "开始列选择"**：锚点为当前光标、活动角移到下一行（有可见反馈），后续仍用 Alt+Shift+方向键扩展。
- **取消**：Esc（消费按键）、鼠标点击、Enter、以及任何未处理的按键（先取消再执行原行为）。
- 激活时真实光标始终位于活动角（复用 Qt 的滚动/闪烁光标）。

### 激活时的编辑操作（每行生效，单次 Ctrl+Z 可整体撤销）

| 操作 | 行为 |
|---|---|
| 输入字符 / Tab（插入 tabWidth 空格） | 宽度>0：先删除每行 `[左列,右列)` 再在左列插入；宽度==0：在每行 clamp 后的列处插入。编辑后列选区保持在插入文本之后（多光标手感） |
| Backspace / Delete | 宽度>0：删除整块并取消；宽度==0：每行删除光标前/后 1 个字符，选区保持 |
| Ctrl+C / 复制 | 宽度>0：每行片段用 `\n` 连接放入剪贴板；宽度==0：无操作 |
| Ctrl+X / 剪切 | 复制 + 删块 + 取消 |
| Ctrl+V / 粘贴 | 剪贴板行数 == 选区行数 → 逐行对应插入（宽度>0 时先删块）；否则取消列选择后按普通粘贴 |
| 鼠标点击 / Esc / Enter / 其他按键 | 取消列选择（Enter 等继续走原有处理，如自动缩进） |

所有块编辑用 `QTextCursor::beginEditBlock()/endEditBlock()` 包裹（单步撤销），按行**自底向上**处理避免位置偏移。

## 修改文件

### 1. src/ui/editor/CodeEditor.hpp / .cpp（主要改动）

新增状态：`bool columnActive_`、锚点/活动角 `{line, column}`（虚拟列，int）。

新增 public API：
- `bool hasColumnSelection() const`
- `void startColumnSelection()` — 菜单入口：锚点=光标，角=下一行同列（无下一行则同行）
- `void cancelColumnSelection()`
- `void columnCopy() / columnCut() / columnPaste()` — MainWindow 的 editCopy/Cut/Paste 在激活时改调这些

重写/扩展：
- `keyPressEvent`（现位于 CodeEditor.cpp L136）：最前面处理 Alt+Shift+方向键（key ∈ [Key_Left, Key_Down]）；激活时分发 Esc/Backspace/Delete/Tab/可打印字符/Ctrl+C/X/V；其余键先取消再落回原逻辑（保留现有自动缩进、Tab 逻辑在列选未激活时不变）
- `mousePressEvent`（新增 override）：激活时先取消再调用基类
- `paintEvent`（新增 override）：调用基类后，宽度==0 时在每行 clamp 位置用 `cursorRect(QTextCursor)` 画 1px 竖线光标（QPlainTextEdit 的该重载是 public）
- `highlightCurrentLine()`（L169）：追加列选区渲染 —— 每行一个 `QTextEdit::ExtraSelection`（背景 = `palette().color(QPalette::Highlight)`，范围按行 clamp）；列状态变化时手动调用它刷新
- `loadFile()`：成功载入后 `cancelColumnSelection()`（防止文档替换后残留失效位置）

注意：QPlainTextEdit 的 copy/cut/paste 槽不是虚函数，键盘路径在 keyPressEvent 里消费 Ctrl+C/X/V，菜单路径由 MainWindow 显式调用 column* 方法，两边都覆盖。

### 2. src/ui/mainwindow/MainWindow.hpp / .cpp

- `buildMenus()` Edit 菜单（L127-150 区域，Select All 之后）：新增 `tr("Start Co&lumn Selection")` 动作（无快捷键，`statusTip` 提示 "Alt+Shift+Arrows"），连到新槽
- 新增槽 `editColumnSelection()`：`currentEditor()->startColumnSelection()`
- `editCopy/editCut/editPaste`（L509-521）：`if (e->hasColumnSelection()) { e->columnCopy(); return; }` 等三处分支

### 3. i18n/mo_zh_CN.ts

为 MainWindow 新增两条翻译：菜单项"Start Co&lumn Selection"→"开始列选择(&L)"、状态提示。优先 `cmake --build build --target mo_lupdate` 重新生成；lupdate 不可用则手改 .ts（qm 由 qt_add_translations 构建时自动重嵌）。

### 4. tests/unit/test_codeeditor.cpp（新增）+ tests/unit/CMakeLists.txt

按 test_settings/test_document 模式新增 `test_codeeditor` 目标，链接 `Qt6::Test + Mo::Ui`；`QT_QPA_PLATFORM=offscreen` + `QStandardPaths::setTestModeEnabled(true)`（同 test_mainwindow）。用 `QTest::keyClick(editor, Qt::Key_Down, Qt::AltModifier|Qt::ShiftModifier)` 走真实按键路径。用例：

1. 多行建区+复制：3 行文本，Alt+Shift+Down×2 + Right×2 → `columnCopy()` 后剪贴板为预期矩形片段
2. 块输入替换：宽度 2 的 2 行选区上输入 "XY" → 每行替换，`undo()` 一次恢复原文（验证单步撤销）
3. 零宽列 Backspace：每行删 1 字符、选区保持
4. Esc 取消；鼠标点击取消（`QTest::mouseClick`）

## 验证

1. `cmake --build build && ctest --test-dir build`（含新 test_codeeditor，全绿）
2. `QT_QPA_PLATFORM=offscreen` 冒烟运行 `./build/src/mo /tmp/xx.txt` 确认启动无回归
3. 有显示环境时人工验证：Alt+Shift+方向键建区/翻转、输入/删除/复制/粘贴/Esc，及 zh_CN 菜单文案

## 已知取舍

- 虚拟列不做行尾空格填充（短行 clamp 到行尾，不 pad）
- 列选高亮用 `palette Highlight` 色；QSS 中 `selection-background-color` 若与调色板不同会有细微色差（可接受）
- 不做 Alt+鼠标拖拽建区（后续可加）
