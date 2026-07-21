// SPDX-FileCopyrightText: 2026 liudng <liudng@users.noreply.github.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "app/Application.hpp"

int main(int argc, char *argv[])
{
    hello::app::Application app(argc, argv);
    return app.run();
}
