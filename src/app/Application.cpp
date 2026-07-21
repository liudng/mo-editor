// SPDX-FileCopyrightText: 2026 liudng <liudng@users.noreply.github.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Application.hpp"
#include "SingleInstance.hpp"
#include "hello/core/Constants.hpp"
#include "hello/core/Logger.hpp"
#include "hello/core/Settings.hpp"
#include "mainwindow/MainWindow.hpp"

#include <QLibraryInfo>
#include <QLocale>
#include <QSettings>
#include <QStandardPaths>
#include <QTranslator>

namespace hello::app {

Application::Application(int &argc, char **argv)
    : QApplication(argc, argv)
{
    setOrganizationName(hello::core::constants::kOrganizationName);
    setApplicationName(hello::core::constants::kApplicationName);
    setApplicationDisplayName(hello::core::constants::kApplicationName);
    setApplicationVersion(hello::core::constants::kApplicationVersion);
    setDesktopFileName(hello::core::constants::kApplicationId);

    initLogging();
    loadTranslation();

    singleInstance_ = std::make_unique<SingleInstance>(this);
    if (!singleInstance_->tryLock()) {
        hello::core::Logger::warning("Another instance is already running; exiting");
        return;
    }

    hello::core::Settings::instance().load();

    mainWindow_ = std::make_unique<hello::ui::MainWindow>();
    mainWindow_->show();
}

Application::~Application() = default;

int Application::run()
{
    if (!mainWindow_) {
        // Another instance already holds the lock; quit immediately.
        return 0;
    }
    connect(this, &QCoreApplication::aboutToQuit, this, &Application::saveSession);
    return exec();
}

void Application::saveSession()
{
    if (mainWindow_) {
        mainWindow_->saveSession();
    }
    hello::core::Settings::instance().save();
}

void Application::loadTranslation()
{
    // Qt's own translation (dialogs, standard shortcuts).
    static QTranslator qtTranslator;
    if (qtTranslator.load(QLocale::system(), QStringLiteral("qt"), QStringLiteral("_"),
                          QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
        installTranslator(&qtTranslator);
    }

    // Application translation; load zh_CN resource when locale is Chinese.
    static QTranslator appTranslator;
    const auto locale = QLocale().name();
    if (locale.startsWith(QStringLiteral("zh"))) {
        if (appTranslator.load(QStringLiteral(":/i18n/hello_zh_CN.qm"))) {
            installTranslator(&appTranslator);
        } else {
            hello::core::Logger::warning("Failed to load zh_CN translation resource");
        }
    }
}

void Application::initLogging()
{
    hello::core::Logger::init();
}

} // namespace hello::app
