// SPDX-FileCopyrightText: 2026 liudng <liudng@users.noreply.github.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QApplication>
#include <memory>

namespace hello::ui {
class MainWindow;
}

namespace hello::app {

class SingleInstance;

class Application : public QApplication
{
    Q_OBJECT

public:
    explicit Application(int &argc, char **argv);
    ~Application() override;

    int run();

private slots:
    void saveSession();

private:
    void loadTranslation();
    void initLogging();

    std::unique_ptr<SingleInstance> singleInstance_;
    std::unique_ptr<hello::ui::MainWindow> mainWindow_;
};

} // namespace hello::app
