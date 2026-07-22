// SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "MainWindow.hpp"

#include "dialogs/FindReplaceDialog.hpp"
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
#include <QKeySequence>
#include <QMenuBar>
#include <QMessageBox>
#include <QIcon>
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

    findReplaceDialog_ = std::make_unique<FindReplaceDialog>(this);

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

void MainWindow::buildMenus()
{
    auto *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(tr("&New"), QKeySequence::New, this, &MainWindow::fileNew);
    fileMenu->addAction(tr("&Open..."), QKeySequence::Open, this, &MainWindow::fileOpen);
    fileMenu->addAction(tr("&Save"), QKeySequence::Save, this, &MainWindow::fileSave);
    fileMenu->addAction(tr("Save &As..."), QKeySequence::SaveAs, this, &MainWindow::fileSaveAs);
    fileMenu->addSeparator();
    recentFilesMenu_ = fileMenu->addMenu(tr("Recent &Files"));
    fileMenu->addSeparator();
    fileMenu->addAction(tr("&Quit"), QKeySequence::Quit, this, &MainWindow::fileQuit);

    auto *editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(tr("&Find..."), QKeySequence::Find, this, &MainWindow::editFind);
    editMenu->addAction(tr("Find &Next"), QKeySequence::FindNext, this, &MainWindow::editFindNext);

    menuBar()->addMenu(tr("&View"));

    auto *settingsMenu = menuBar()->addMenu(tr("&Settings"));
    settingsMenu->addAction(tr("&Preferences..."), this, &MainWindow::editPreferences);

    auto *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(tr("&About"), this, &MainWindow::helpAbout);
    helpMenu->addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);
}

void MainWindow::buildToolBar()
{
    auto *tb = addToolBar(tr("Main"));
    tb->setObjectName(QStringLiteral("MainToolBar"));

    // Helper: add an action with a themed icon, storing the icon name on the
    // action so applyIconTheme() can re-resolve it when the fallback changes.
    auto addThemed = [tb, this](const QString &iconName, const QString &text,
                                auto slot) {
        auto *act = tb->addAction(QIcon::fromTheme(iconName), text, this, slot);
        act->setData(iconName);
        return act;
    };

    addThemed(QStringLiteral("document-new"), tr("New"), &MainWindow::fileNew);
    addThemed(QStringLiteral("document-open"), tr("Open"), &MainWindow::fileOpen);
    addThemed(QStringLiteral("document-save"), tr("Save"), &MainWindow::fileSave);
    addThemed(QStringLiteral("document-save-as"), tr("Save As"), &MainWindow::fileSaveAs);
    tb->addSeparator();
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
        // System: clear custom sheet, follow the platform palette.
        setStyleSheet({});
    }

    // The stylesheet change above may have flipped the widget palette, so we
    // must refresh the KSyntaxHighlighting color theme on every open editor
    // to keep syntax colors readable in both light and dark modes.
    for (int i = 0; i < tabWidget_->count(); ++i) {
        auto *editor = qobject_cast<CodeEditor *>(tabWidget_->widget(i));
        if (editor) {
            editor->applyHighlightingTheme();
        }
    }
}

void MainWindow::applyIconTheme()
{
    // When the system icon theme (often Adwaita on GNOME) lacks standard
    // action icons, QIcon::fromTheme returns empty pixmaps and the toolbar
    // shows text-only buttons. Setting a well-stocked fallback theme (e.g.
    // Breeze) fills those gaps so toolbar icons always render.
    const auto iconTheme = mo::core::Settings::instance().iconTheme();
    if (!iconTheme.isEmpty()) {
        QIcon::setFallbackThemeName(iconTheme);
    }
    // Force toolbar and menu actions to re-resolve their icons against the
    // updated fallback theme.
    const auto toolBars = findChildren<QToolBar *>();
    for (auto *tb : toolBars) {
        const auto actions = tb->actions();
        for (auto *act : actions) {
            // Re-set the icon from the same theme name so Qt re-resolves it
            // through the fallback chain.
            const auto name = act->data().toString();
            if (!name.isEmpty()) {
                act->setIcon(QIcon::fromTheme(name));
            }
        }
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
    const qsizetype max = 10;
    const qsizetype count = std::min(recentFiles_.size(), max);
    for (int i = 0; i < count; ++i) {
        const auto &path = recentFiles_.at(i);
        recentFilesMenu_->addAction(path, this, [this, path]() { openFile(path); });
    }
}

void MainWindow::newTab()
{
    auto *editor = new CodeEditor(this);
    const int idx = tabWidget_->addTab(editor, tr("Untitled"));
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
    if (!editor->loadFile(path)) {
        delete editor;
        QMessageBox::warning(this, tr("Open File"), tr("Cannot read %1.").arg(path));
        return;
    }
    const int idx = tabWidget_->addTab(editor, fi.fileName());
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
    auto *w = tabWidget_->widget(index);
    tabWidget_->removeTab(index);
    delete w;
}

void MainWindow::onTabChanged(int index) { Q_UNUSED(index); }

void MainWindow::onTabCloseRequested(int index) { closeTab(index); }

void MainWindow::onFileChanged(const QString &path)
{
    mo::core::Logger::info("File changed externally: " + path);
}

void MainWindow::onSettingsChanged()
{
    applyTheme();
    applyIconTheme();
}

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
    auto *editor = qobject_cast<CodeEditor *>(tabWidget_->currentWidget());
    if (editor) {
        editor->save();
    } else {
        newTab();
    }
}

void MainWindow::fileSaveAs()
{
    auto *editor = qobject_cast<CodeEditor *>(tabWidget_->currentWidget());
    if (!editor) {
        return;
    }
    const auto path = QFileDialog::getSaveFileName(this, tr("Save File As"));
    if (!path.isEmpty()) {
        editor->saveAs(path);
        tabWidget_->setTabText(tabWidget_->currentIndex(), QFileInfo(path).fileName());
    }
}

void MainWindow::fileQuit() { close(); }

void MainWindow::editFind()
{
    findReplaceDialog_->show();
    findReplaceDialog_->raise();
    findReplaceDialog_->activateWindow();
}

void MainWindow::editFindNext()
{
    auto *editor = qobject_cast<CodeEditor *>(tabWidget_->currentWidget());
    if (editor) {
        editor->findNext(findReplaceDialog_->searchText());
    }
}

void MainWindow::editPreferences()
{
    SettingsDialog dlg(this);
    connect(&dlg, &SettingsDialog::settingsChanged, this, &MainWindow::onSettingsChanged);
    dlg.exec();
}

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
