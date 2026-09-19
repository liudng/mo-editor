// SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Application.hpp"
#include "SingleInstance.hpp"
#include "mo/core/Constants.hpp"
#include "mo/core/Logger.hpp"
#include "mo/core/Settings.hpp"
#include "mainwindow/MainWindow.hpp"

#include <QCommandLineParser>
#include <QFileInfo>
#include <QLibraryInfo>
#include <QLocale>
#include <QSettings>
#include <QStandardPaths>
#include <QTranslator>

namespace mo::app {

Application::Application(int &argc, char **argv)
    : QApplication(argc, argv)
{
    setOrganizationName(mo::core::constants::kOrganizationName);
    setApplicationName(mo::core::constants::kApplicationName);
    setApplicationDisplayName(mo::core::constants::kApplicationName);
    setApplicationVersion(mo::core::constants::kApplicationVersion);
    setDesktopFileName(mo::core::constants::kApplicationId);

    initLogging();
    loadTranslation();
    parseCommandLine();

    singleInstance_ = std::make_unique<SingleInstance>(this);
    if (!singleInstance_->tryLock()) {
        mo::core::Logger::warning("Another instance is already running; exiting");
        if (!filesToOpen_.isEmpty()) {
            // Hand the file arguments over to the running instance.
            singleInstance_->forwardToRunningInstance(filesToOpen_);
        }
        return;
    }

    // A second instance may hand over its file arguments while we run.
    connect(singleInstance_.get(), &SingleInstance::openFilesRequested, this,
            [this](const QStringList &files) {
                if (!mainWindow_) {
                    return;
                }
                for (const auto &file : files) {
                    mainWindow_->openFile(file);
                }
                mainWindow_->show();
                mainWindow_->raise();
                mainWindow_->activateWindow();
            });

    mo::core::Settings::instance().load();

    mainWindow_ = std::make_unique<mo::ui::MainWindow>(filesToOpen_);
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
    mo::core::Settings::instance().save();
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
        if (appTranslator.load(QStringLiteral(":/i18n/mo_zh_CN.qm"))) {
            installTranslator(&appTranslator);
        } else {
            mo::core::Logger::warning("Failed to load zh_CN translation resource");
        }
    }
}

void Application::initLogging()
{
    mo::core::Logger::init();
}

void Application::parseCommandLine()
{
    QCommandLineParser parser;
    parser.setApplicationDescription(
        tr("A fast and lightweight multi-tab text editor"));
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument(QStringLiteral("files"),
                                 tr("Files to open, each in its own tab."),
                                 QStringLiteral("[files...]"));
    parser.process(*this);

    // Normalize to absolute paths so that later session restore and recent
    // files work regardless of the working directory.
    for (const auto &file : parser.positionalArguments()) {
        filesToOpen_.append(QFileInfo(file).absoluteFilePath());
    }
}

} // namespace mo::app
