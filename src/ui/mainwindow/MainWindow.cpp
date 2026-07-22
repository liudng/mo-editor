// SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "MainWindow.hpp"

#include "dialogs/FindReplaceDock.hpp"
#include "dialogs/SettingsDialog.hpp"
#include "editor/CodeEditor.hpp"
#include "mo/core/Constants.hpp"
#include "mo/core/Logger.hpp"
#include "mo/core/Settings.hpp"
#include "mo/services/SessionManager.hpp"

#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QIcon>
#include <QKeySequence>
#include <QMenuBar>
#include <QMessageBox>
#include <QTabWidget>
#include <QToolBar>

#include <algorithm>

namespace mo::ui {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(mo::core::constants::kApplicationName);
    resize(1024, 768);

    tabWidget_ = new QTabWidget(this);
    tabWidget_->setTabsClosable(true);
    tabWidget_->setMovable(true);
    setCentralWidget(tabWidget_);

    fileWatcher_ = new QFileSystemWatcher(this);
    connect(fileWatcher_, &QFileSystemWatcher::fileChanged,
            this, &MainWindow::onFileChanged);

    // Find/Replace dock at the bottom edge.
    findDock_ = new FindReplaceDock(this);
    findDock_->setObjectName(QStringLiteral("FindReplaceDock"));
    findDock_->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
    addDockWidget(Qt::BottomDockWidgetArea, findDock_);
    findDock_->hide();

    buildMenus();
    buildToolBar();
    applyTheme();
    applyIconTheme();

    connect(tabWidget_, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);
    connect(tabWidget_, &QTabWidget::tabCloseRequested, this, &MainWindow::onTabCloseRequested);

    recentFiles_ = mo::core::Settings::instance().recentFiles();
    updateRecentFilesMenu();

    connect(&mo::core::Settings::instance(), &mo::core::Settings::themeChanged,
            this, &MainWindow::applyTheme);

    // Restore previous session if any.
    mo::services::SessionManager sm(this);
    if (auto session = sm.loadSession()) {
        for (const auto &f : session->openFiles) {
            openFile(f);
        }
        if (!session->windowGeometry.isEmpty()) {
            restoreGeometry(session->windowGeometry);
        }
        if (!session->windowState.isEmpty()) {
            restoreState(session->windowState);
        }
    }

    if (tabWidget_->count() == 0) {
        newTab();
    }
}

MainWindow::~MainWindow() = default;

CodeEditor *MainWindow::currentEditor() const
{
    return qobject_cast<CodeEditor *>(tabWidget_->currentWidget());
}

void MainWindow::buildMenus()
{
    // ---- File ----
    auto *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(QIcon::fromTheme(QStringLiteral("document-new")),
                        tr("&New"), QKeySequence::New, this, &MainWindow::fileNew);
    fileMenu->addAction(QIcon::fromTheme(QStringLiteral("document-open")),
                        tr("&Open..."), QKeySequence::Open, this, &MainWindow::fileOpen);
    fileMenu->addSeparator();
    auto *saveAct = fileMenu->addAction(QIcon::fromTheme(QStringLiteral("document-save")),
                        tr("&Save"), QKeySequence::Save, this, &MainWindow::fileSave);
    saveAct->setData(QStringLiteral("document-save"));
    auto *saveAsAct = fileMenu->addAction(QIcon::fromTheme(QStringLiteral("document-save-as")),
                        tr("Save &As..."), QKeySequence::SaveAs, this, &MainWindow::fileSaveAs);
    saveAsAct->setData(QStringLiteral("document-save-as"));
    auto *saveAllAct = fileMenu->addAction(QIcon::fromTheme(QStringLiteral("document-save-all")),
                        tr("Save A&ll"), QKeySequence(tr("Ctrl+Shift+S")), this, &MainWindow::fileSaveAll);
    saveAllAct->setData(QStringLiteral("document-save-all"));
    fileMenu->addSeparator();
    fileMenu->addAction(tr("Close Ta&b"), QKeySequence::Close, this, &MainWindow::fileCloseTab);
    fileMenu->addSeparator();
    recentFilesMenu_ = fileMenu->addMenu(QIcon::fromTheme(QStringLiteral("document-open-recent")),
                                         tr("Recent &Files"));
    fileMenu->addSeparator();
    fileMenu->addAction(QIcon::fromTheme(QStringLiteral("application-exit")),
                        tr("&Quit"), QKeySequence::Quit, this, &MainWindow::fileQuit);

    // ---- Edit ----
    auto *editMenu = menuBar()->addMenu(tr("&Edit"));
    auto *undoAct = editMenu->addAction(QIcon::fromTheme(QStringLiteral("edit-undo")),
                        tr("&Undo"), QKeySequence::Undo, this, &MainWindow::editUndo);
    undoAct->setData(QStringLiteral("edit-undo"));
    auto *redoAct = editMenu->addAction(QIcon::fromTheme(QStringLiteral("edit-redo")),
                        tr("&Redo"), QKeySequence::Redo, this, &MainWindow::editRedo);
    redoAct->setData(QStringLiteral("edit-redo"));
    editMenu->addSeparator();
    auto *cutAct = editMenu->addAction(QIcon::fromTheme(QStringLiteral("edit-cut")),
                        tr("Cu&t"), QKeySequence::Cut, this, &MainWindow::editCut);
    cutAct->setData(QStringLiteral("edit-cut"));
    auto *copyAct = editMenu->addAction(QIcon::fromTheme(QStringLiteral("edit-copy")),
                        tr("&Copy"), QKeySequence::Copy, this, &MainWindow::editCopy);
    copyAct->setData(QStringLiteral("edit-copy"));
    auto *pasteAct = editMenu->addAction(QIcon::fromTheme(QStringLiteral("edit-paste")),
                        tr("&Paste"), QKeySequence::Paste, this, &MainWindow::editPaste);
    pasteAct->setData(QStringLiteral("edit-paste"));
    editMenu->addAction(QIcon::fromTheme(QStringLiteral("edit-select-all")),
                        tr("Select &All"), QKeySequence::SelectAll, this, &MainWindow::editSelectAll);
    editMenu->addSeparator();
    editMenu->addAction(QIcon::fromTheme(QStringLiteral("edit-find")),
                        tr("&Find..."), QKeySequence::Find, this, &MainWindow::editFind);
    editMenu->addAction(tr("Find &Next"), QKeySequence::FindNext, this, &MainWindow::editFindNext);
    editMenu->addAction(tr("Find Pre&vious"), QKeySequence::FindPrevious, this, &MainWindow::editFindPrev);

    // ---- View ----
    auto *viewMenu = menuBar()->addMenu(tr("&View"));
    toggleLineNumbersAction_ = viewMenu->addAction(tr("Show &Line Numbers"));
    toggleLineNumbersAction_->setCheckable(true);
    toggleLineNumbersAction_->setChecked(mo::core::Settings::instance().showLineNumbers());
    connect(toggleLineNumbersAction_, &QAction::toggled, this, &MainWindow::viewToggleLineNumbers);

    toggleToolBarAction_ = viewMenu->addAction(tr("Show &Toolbar"));
    toggleToolBarAction_->setCheckable(true);
    toggleToolBarAction_->setChecked(true);
    connect(toggleToolBarAction_, &QAction::toggled, this, &MainWindow::viewToggleToolBar);

    viewMenu->addSeparator();
    viewMenu->addAction(QIcon::fromTheme(QStringLiteral("zoom-in")),
                        tr("Zoom &In"), QKeySequence::ZoomIn, this, &MainWindow::viewZoomIn);
    viewMenu->addAction(QIcon::fromTheme(QStringLiteral("zoom-out")),
                        tr("Zoom &Out"), QKeySequence::ZoomOut, this, &MainWindow::viewZoomOut);
    viewMenu->addAction(tr("Zoom &Reset"), QKeySequence(tr("Ctrl+0")), this, &MainWindow::viewZoomReset);

    // ---- Settings ----
    auto *settingsMenu = menuBar()->addMenu(tr("&Settings"));
    settingsMenu->addAction(QIcon::fromTheme(QStringLiteral("configure")),
                            tr("&Preferences..."), this, &MainWindow::editPreferences);

    // ---- Help ----
    auto *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(QIcon::fromTheme(QStringLiteral("help-about")),
                        tr("&About"), this, &MainWindow::helpAbout);
    helpMenu->addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);
}

void MainWindow::buildToolBar()
{
    toolBar_ = addToolBar(tr("Main"));
    toolBar_->setObjectName(QStringLiteral("MainToolBar"));

    auto addThemed = [this](const QString &iconName, const QString &text, auto slot) {
        auto *act = toolBar_->addAction(QIcon::fromTheme(iconName), text, this, slot);
        act->setData(iconName);
        return act;
    };

    addThemed(QStringLiteral("document-new"), tr("New"), &MainWindow::fileNew);
    addThemed(QStringLiteral("document-open"), tr("Open"), &MainWindow::fileOpen);
    addThemed(QStringLiteral("document-save"), tr("Save"), &MainWindow::fileSave);
    addThemed(QStringLiteral("document-save-as"), tr("Save As"), &MainWindow::fileSaveAs);
    addThemed(QStringLiteral("document-save-all"), tr("Save All"), &MainWindow::fileSaveAll);
    toolBar_->addSeparator();
    addThemed(QStringLiteral("edit-undo"), tr("Undo"), &MainWindow::editUndo);
    addThemed(QStringLiteral("edit-redo"), tr("Redo"), &MainWindow::editRedo);
    toolBar_->addSeparator();
    addThemed(QStringLiteral("edit-cut"), tr("Cut"), &MainWindow::editCut);
    addThemed(QStringLiteral("edit-copy"), tr("Copy"), &MainWindow::editCopy);
    addThemed(QStringLiteral("edit-paste"), tr("Paste"), &MainWindow::editPaste);
    toolBar_->addSeparator();
    addThemed(QStringLiteral("edit-find"), tr("Find"), &MainWindow::editFind);
}

void MainWindow::applyTheme()
{
    const auto theme = mo::core::Settings::instance().theme();
    if (theme == QStringLiteral("Dark")) {
        loadStyleSheet(QStringLiteral("dark"));
    } else if (theme == QStringLiteral("Light")) {
        loadStyleSheet(QStringLiteral("light"));
    } else {
        setStyleSheet({});
    }

    for (int i = 0; i < tabWidget_->count(); ++i) {
        auto *editor = qobject_cast<CodeEditor *>(tabWidget_->widget(i));
        if (editor) {
            editor->applyHighlightingTheme();
        }
    }
}

void MainWindow::applyIconTheme()
{
    const auto iconTheme = mo::core::Settings::instance().iconTheme();
    if (!iconTheme.isEmpty()) {
        QIcon::setFallbackThemeName(iconTheme);
    }
    const auto toolBars = findChildren<QToolBar *>();
    for (auto *tb : toolBars) {
        const auto actions = tb->actions();
        for (auto *act : actions) {
            const auto name = act->data().toString();
            if (!name.isEmpty()) {
                act->setIcon(QIcon::fromTheme(name));
            }
        }
    }
}

void MainWindow::applyEditorSettingsToAll()
{
    for (int i = 0; i < tabWidget_->count(); ++i) {
        auto *editor = qobject_cast<CodeEditor *>(tabWidget_->widget(i));
        if (editor) {
            editor->applyEditorSettings();
        }
    }
    if (toggleLineNumbersAction_) {
        toggleLineNumbersAction_->setChecked(mo::core::Settings::instance().showLineNumbers());
    }
}

void MainWindow::loadStyleSheet(const QString &name)
{
    if (name.isEmpty()) {
        setStyleSheet({});
        return;
    }
    QFile f(QStringLiteral(":/styles/") + name + QStringLiteral(".qss"));
    if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setStyleSheet(QString::fromUtf8(f.readAll()));
    }
}

void MainWindow::updateRecentFilesMenu()
{
    if (!recentFilesMenu_) {
        return;
    }
    recentFilesMenu_->clear();
    if (recentFiles_.isEmpty()) {
        auto *emptyAct = recentFilesMenu_->addAction(tr("No recent files"));
        emptyAct->setEnabled(false);
        return;
    }
    const qsizetype max = 10;
    const qsizetype count = std::min(recentFiles_.size(), max);
    for (int i = 0; i < count; ++i) {
        const auto &path = recentFiles_.at(i);
        recentFilesMenu_->addAction(path, this, [this, path]() { openFile(path); });
    }
    recentFilesMenu_->addSeparator();
    recentFilesMenu_->addAction(QIcon::fromTheme(QStringLiteral("edit-clear")),
                                tr("Clear &List"), this, [this]() {
        recentFiles_.clear();
        mo::core::Settings::instance().setRecentFiles(recentFiles_);
        updateRecentFilesMenu();
    });
}

void MainWindow::newTab()
{
    auto *editor = new CodeEditor(this);
    connect(editor, &CodeEditor::titleChanged, this, &MainWindow::onEditorTitleChanged);
    connect(editor, &CodeEditor::modificationChanged, this, &MainWindow::onEditorModificationChanged);
    const int idx = tabWidget_->addTab(editor, editor->tabTitle());
    tabWidget_->setCurrentIndex(idx);
}

void MainWindow::openFile(const QString &path)
{
    if (path.isEmpty()) {
        return;
    }
    const QFileInfo fi(path);
    if (!fi.exists() || !fi.isFile()) {
        QMessageBox::warning(this, tr("Open File"), tr("Cannot open %1.").arg(path));
        return;
    }

    auto *editor = new CodeEditor(this);
    connect(editor, &CodeEditor::titleChanged, this, &MainWindow::onEditorTitleChanged);
    connect(editor, &CodeEditor::modificationChanged, this, &MainWindow::onEditorModificationChanged);
    if (!editor->loadFile(path)) {
        delete editor;
        QMessageBox::warning(this, tr("Open File"), tr("Cannot read %1.").arg(path));
        return;
    }
    const int idx = tabWidget_->addTab(editor, editor->tabTitle());
    tabWidget_->setTabToolTip(idx, fi.absoluteFilePath());
    tabWidget_->setCurrentIndex(idx);

    const auto absolute = fi.absoluteFilePath();
    if (!fileWatcher_->files().contains(absolute)) {
        fileWatcher_->addPath(absolute);
    }

    recentFiles_.removeAll(path);
    recentFiles_.prepend(path);
    while (recentFiles_.size() > 10) {
        recentFiles_.removeLast();
    }
    mo::core::Settings::instance().setRecentFiles(recentFiles_);
    updateRecentFilesMenu();
}

void MainWindow::closeTab(int index)
{
    if (index < 0 || index >= tabWidget_->count()) {
        return;
    }
    auto *editor = qobject_cast<CodeEditor *>(tabWidget_->widget(index));
    if (editor && editor->document()->isModified()) {
        const auto title = editor->tabTitle();
        auto ret = QMessageBox::question(this, tr("Close Tab"),
            tr("\"%1\" has been modified. Close anyway?").arg(title),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (ret != QMessageBox::Yes) {
            return;
        }
    }
    auto *w = tabWidget_->widget(index);
    tabWidget_->removeTab(index);
    delete w;
    if (tabWidget_->count() == 0) {
        newTab();
    }
}

void MainWindow::updateTabTitle(int index)
{
    if (index < 0 || index >= tabWidget_->count()) {
        return;
    }
    auto *editor = qobject_cast<CodeEditor *>(tabWidget_->widget(index));
    if (editor) {
        tabWidget_->setTabText(index, editor->tabTitle());
    }
}

void MainWindow::onTabChanged(int index) { Q_UNUSED(index); }

void MainWindow::onTabCloseRequested(int index) { closeTab(index); }

void MainWindow::onFileChanged(const QString &path)
{
    mo::core::Logger::info("File changed externally: " + path);
    // Find the editor for this file and prompt the user.
    for (int i = 0; i < tabWidget_->count(); ++i) {
        auto *editor = qobject_cast<CodeEditor *>(tabWidget_->widget(i));
        if (editor && editor->filePath() == path) {
            if (!QFileInfo::exists(path)) {
                auto ret = QMessageBox::question(this, tr("File Removed"),
                    tr("\"%1\" was removed or renamed. Keep the tab open?")
                        .arg(QFileInfo(path).fileName()),
                    QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
                if (ret == QMessageBox::No) {
                    closeTab(i);
                }
            } else {
                auto ret = QMessageBox::question(this, tr("File Changed"),
                    tr("\"%1\" was modified by another program. Reload?")
                        .arg(QFileInfo(path).fileName()),
                    QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
                if (ret == QMessageBox::Yes) {
                    editor->loadFile(path);
                }
            }
            break;
        }
    }
}

void MainWindow::onSettingsChanged()
{
    applyTheme();
    applyIconTheme();
    applyEditorSettingsToAll();
}

void MainWindow::onEditorTitleChanged(const QString &title)
{
    auto *editor = qobject_cast<CodeEditor *>(sender());
    if (!editor) return;
    const int idx = tabWidget_->indexOf(editor);
    if (idx >= 0) {
        tabWidget_->setTabText(idx, title);
    }
}

void MainWindow::onEditorModificationChanged(bool modified)
{
    Q_UNUSED(modified);
    auto *editor = qobject_cast<CodeEditor *>(sender());
    if (!editor) return;
    const int idx = tabWidget_->indexOf(editor);
    if (idx >= 0) {
        tabWidget_->setTabText(idx, editor->tabTitle());
    }
}

// ---- File slots ----

void MainWindow::fileNew() { newTab(); }

void MainWindow::fileOpen()
{
    const auto path = QFileDialog::getOpenFileName(this, tr("Open File"));
    if (!path.isEmpty()) {
        openFile(path);
    }
}

void MainWindow::fileSave()
{
    auto *editor = currentEditor();
    if (editor) {
        editor->save();
    }
}

void MainWindow::fileSaveAs()
{
    auto *editor = currentEditor();
    if (!editor) {
        return;
    }
    const auto path = QFileDialog::getSaveFileName(this, tr("Save File As"));
    if (!path.isEmpty()) {
        editor->saveAs(path);
        const int idx = tabWidget_->currentIndex();
        tabWidget_->setTabText(idx, editor->tabTitle());
        tabWidget_->setTabToolTip(idx, QFileInfo(path).absoluteFilePath());
    }
}

void MainWindow::fileSaveAll()
{
    for (int i = 0; i < tabWidget_->count(); ++i) {
        auto *editor = qobject_cast<CodeEditor *>(tabWidget_->widget(i));
        if (editor && editor->document()->isModified()) {
            if (editor->filePath().isEmpty()) {
                // Switch to this tab so Save As dialog makes sense.
                tabWidget_->setCurrentIndex(i);
                fileSaveAs();
            } else {
                editor->save();
            }
        }
    }
}

void MainWindow::fileCloseTab()
{
    closeTab(tabWidget_->currentIndex());
}

void MainWindow::fileQuit() { close(); }

// ---- Edit slots ----

void MainWindow::editUndo()
{
    if (auto *e = currentEditor()) e->undo();
}

void MainWindow::editRedo()
{
    if (auto *e = currentEditor()) e->redo();
}

void MainWindow::editCut()
{
    if (auto *e = currentEditor()) e->cut();
}

void MainWindow::editCopy()
{
    if (auto *e = currentEditor()) e->copy();
}

void MainWindow::editPaste()
{
    if (auto *e = currentEditor()) e->paste();
}

void MainWindow::editSelectAll()
{
    if (auto *e = currentEditor()) e->selectAll();
}

void MainWindow::editFind()
{
    findDock_->show();
    findDock_->raise();
    findDock_->focusSearch();
}

void MainWindow::editFindNext()
{
    findDock_->findNext(currentEditor());
}

void MainWindow::editFindPrev()
{
    findDock_->findPrev(currentEditor());
}

void MainWindow::editPreferences()
{
    SettingsDialog dlg(this);
    connect(&dlg, &SettingsDialog::settingsChanged, this, &MainWindow::onSettingsChanged);
    dlg.exec();
}

// ---- View slots ----

void MainWindow::viewToggleLineNumbers(bool on)
{
    mo::core::Settings::instance().setShowLineNumbers(on);
    mo::core::Settings::instance().save();
    for (int i = 0; i < tabWidget_->count(); ++i) {
        auto *editor = qobject_cast<CodeEditor *>(tabWidget_->widget(i));
        if (editor) {
            editor->setShowLineNumbers(on);
        }
    }
}

void MainWindow::viewToggleToolBar(bool on)
{
    if (toolBar_) {
        toolBar_->setVisible(on);
    }
}

void MainWindow::viewZoomIn()
{
    if (auto *e = currentEditor()) e->zoomFont(1);
}

void MainWindow::viewZoomOut()
{
    if (auto *e = currentEditor()) e->zoomFont(-1);
}

void MainWindow::viewZoomReset()
{
    auto *e = currentEditor();
    if (!e) return;
    QFont f(mo::core::Settings::instance().fontFamily(),
            mo::core::Settings::instance().fontSize());
    f.setStyleHint(QFont::Monospace);
    f.setFixedPitch(true);
    e->setFont(f);
}

// ---- Help slots ----

void MainWindow::helpAbout()
{
    QMessageBox::about(this, tr("About mo"),
        tr("<h3>mo %1</h3>"
           "<p>A simple Qt6 text editor.</p>"
           "<p>License: GPL-3.0-or-later</p>")
            .arg(mo::core::constants::kApplicationVersion));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // Check for unsaved changes before closing.
    for (int i = 0; i < tabWidget_->count(); ++i) {
        auto *editor = qobject_cast<CodeEditor *>(tabWidget_->widget(i));
        if (editor && editor->document()->isModified()) {
            tabWidget_->setCurrentIndex(i);
            auto ret = QMessageBox::question(this, tr("Unsaved Changes"),
                tr("There are unsaved changes. Quit anyway?"),
                QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
            if (ret != QMessageBox::Yes) {
                event->ignore();
                return;
            }
            break;
        }
    }
    saveSession();
    event->accept();
}

void MainWindow::saveSession()
{
    mo::services::SessionManager sm(this);
    mo::services::SessionState state;
    for (int i = 0; i < tabWidget_->count(); ++i) {
        auto *editor = qobject_cast<CodeEditor *>(tabWidget_->widget(i));
        if (editor && !editor->filePath().isEmpty()) {
            state.openFiles.append(editor->filePath());
        }
    }
    state.windowGeometry = saveGeometry();
    state.windowState = saveState();
    sm.saveSession(state);
}

} // namespace mo::ui
