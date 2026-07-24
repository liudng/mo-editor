# Mo — A Qt6 Text Editor

![CI](https://github.com/liudng/mo-editor/actions/workflows/ci.yml/badge.svg)
![License](https://img.shields.io/badge/license-GPL--3.0--or--later-blue.svg)
![Qt](https://img.shields.io/badge/Qt-6.8%2B-green.svg)
![C++](https://img.shields.io/badge/C%2B%2B-20-orange.svg)
![Platform](https://img.shields.io/badge/platform-Wayland-purple.svg)

Mo is a lightweight, multi-tab text editor built with Qt6 Widgets. It targets
the Wayland desktop and focuses on fast editing of plain text and source code,
with syntax highlighting, automatic encoding detection, and full session restore.

## Features

- Multi-tab document interface with split views
- Syntax highlighting powered by KSyntaxHighlighting
- Automatic encoding detection via libuchardet
- Session restore (reopens files and cursor positions)
- Find / Replace dialog with regular expressions
- Settings dialog (fonts, theme, editor behavior)
- Wayland-only rendering (no X11 fallback)
- English (default) and Simplified Chinese (zh_CN) translations
- Reproducible builds with SBOM generation
- Packaged as tgz, deb, rpm, and Flatpak

## Requirements

Built and tested on Debian 13 (trixie). Install the build dependencies:

```bash
sudo apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    git \
    extra-cmake-modules \
    qt6-base-dev \
    qt6-tools-dev \
    qt6-tools-dev-tools \
    qt6-svg-dev \
    libkf6syntaxhighlighting-dev \
    libuchardet-dev \
    breeze-icon-theme
```

Minimum versions: CMake 3.31, Qt 6.8, C++20 compiler (GCC 14+ or Clang 18+).

## Build

Mo uses CMake with the Ninja generator. The recommended way is via presets:

```bash
# Configure (default Debug build with Ninja)
cmake --preset default

# Build
cmake --build --preset default

# Run the test suite
ctest --preset test
```

Alternatively, without presets:

```bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
```

## Usage

```bash
./build/src/mo [file1 file2 ...]
```

If no files are given and a previous session exists, the last session is
restored. Pass `--help` for the full list of options.

## Packaging

Packaging targets are provided for tarball, deb, rpm, and Flatpak:

```bash
# Build all packaging targets (configured with the release preset)
cmake --preset release
cmake --build --preset release --target package

# Or produce a specific format
cmake --build build-release --target package-tgz
cmake --build build-release --target package-deb
cmake --build build-release --target package-rpm

# Flatpak (requires flatpak-builder)
flatpak-builder --user --install build-flatpak packaging/flatpak/io.github.liudng.mo.yaml
```

## License

Copyright (C) 2026 liudng

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

See [LICENSE](LICENSE) for the full text, and [LICENSES/](LICENSES/) for the
REUSE-compliant license files.

---

## 中文说明

Mo 是一个基于 Qt6 Widgets 构建的轻量级多标签文本编辑器。它面向 Wayland
桌面，专注于快速编辑纯文本与源代码，提供语法高亮、自动编码检测以及完整的会话恢复功能。

### 功能特性

- 多标签文档界面，支持分屏
- 基于 KSyntaxHighlighting 的语法高亮
- 基于 libuchardet 的自动编码检测
- 会话恢复（重新打开文件并恢复光标位置）
- 支持正则表达式的查找 / 替换对话框
- 设置对话框（字体、主题、编辑器行为）
- 仅支持 Wayland 渲染（不提供 X11 回退）
- 英语（默认）与简体中文（zh_CN）翻译
- 可复现构建，并生成 SBOM
- 提供 tgz、deb、rpm 与 Flatpak 打包

### 依赖要求

在 Debian 13（trixie）上构建与测试。安装构建依赖：

```bash
sudo apt-get install -y \
    build-essential cmake ninja-build git extra-cmake-modules \
    qt6-base-dev qt6-tools-dev qt6-tools-dev-tools qt6-svg-dev \
    libkf6syntaxhighlighting-dev libuchardet-dev \
    breeze-icon-theme
```

最低版本要求：CMake 3.31、Qt 6.8、C++20 编译器（GCC 14+ 或 Clang 18+）。

### 构建方法

```bash
# 配置（默认 Debug，使用 Ninja）
cmake --preset default

# 构建
cmake --build --preset default

# 运行测试
ctest --preset test
```

### 运行

```bash
./build/src/mo [文件1 文件2 ...]
```

### 打包

```bash
cmake --preset release
cmake --build --preset release --target package
```

支持 tgz、deb、rpm 与 Flatpak 四种打包格式。

### 许可证

版权所有 (C) 2026 liudng

本程序为自由软件：您可依据自由软件基金会发布的 GNU 通用公共许可证（第 3 版或更高版本）
重新分发和/或修改它。完整文本请参见 [LICENSE](LICENSE)。
