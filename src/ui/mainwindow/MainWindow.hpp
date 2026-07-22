// SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QMainWindow>
#include <QStringList>
#include <memory>

class QFileSystemWatcher;
class QMenu;
class QTabWidget;
class QToolBar;
class QDockWidget;

namespace mo::ui {

class CodeEditor;
class FindReplaceDock;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void openFile(const QString &path);
    void newTab();
    void closeTab(int index);

    void saveSession();

protected:
    void closeEvent(QCloseEvent *event) override;

public slots:
    void fileNew();
    void fileOpen();
    void fileSave();
    void fileSaveAs();
    void fileSaveAll();
    void fileCloseTab();
    void fileQuit();

    void editUndo();
    void editRedo();
    void editCut();
    void editCopy();
    void editPaste();
    void editSelectAll();
    void editFind();
    void editFindNext();
    void editFindPrev();
    void editPreferences();

    void viewToggleLineNumbers(bool on);
    void viewToggleToolBar(bool on);
    void viewZoomIn();
    void viewZoomOut();
    void viewZoomReset();

    void helpAbout();

private slots:
    void onTabChanged(int index);
    void onTabCloseRequested(int index);
    void onFileChanged(const QString &path);
    void onSettingsChanged();
    void onEditorTitleChanged(const QString &title);
    void onEditorModificationChanged(bool modified);

private:
    void buildMenus();
    void buildToolBar();
    void applyTheme();
    void applyIconTheme();
    void applyEditorSettingsToAll();
    void updateRecentFilesMenu();
    void loadStyleSheet(const QString &name);
    void updateTabTitle(int index);
    CodeEditor *currentEditor() const;

    QTabWidget *tabWidget_ = nullptr;
    QToolBar *toolBar_ = nullptr;
    QMenu *recentFilesMenu_ = nullptr;
    QAction *toggleLineNumbersAction_ = nullptr;
    QAction *toggleToolBarAction_ = nullptr;
    FindReplaceDock *findDock_ = nullptr;
    QFileSystemWatcher *fileWatcher_ = nullptr;
    QStringList recentFiles_;
};

} // namespace mo::ui
