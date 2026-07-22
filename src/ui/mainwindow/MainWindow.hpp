// SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QMainWindow>
#include <QStringList>
#include <memory>

class QFileSystemWatcher;
class QMenu;
class QTabWidget;

namespace mo::ui {

class CodeEditor;
class FindReplaceDialog;

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
    void fileQuit();
    void editFind();
    void editFindNext();
    void editPreferences();
    void helpAbout();

private slots:
    void onTabChanged(int index);
    void onTabCloseRequested(int index);
    void onFileChanged(const QString &path);
    void onSettingsChanged();

private:
    void buildMenus();
    void buildToolBar();
    void applyTheme();
    void updateRecentFilesMenu();
    void loadStyleSheet(const QString &name);

    QTabWidget *tabWidget_ = nullptr;
    QMenu *recentFilesMenu_ = nullptr;
    std::unique_ptr<FindReplaceDialog> findReplaceDialog_;
    QFileSystemWatcher *fileWatcher_ = nullptr;
    QStringList recentFiles_;
};

} // namespace mo::ui
