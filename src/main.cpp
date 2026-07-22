// SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "app/Application.hpp"

int main(int argc, char *argv[])
{
    mo::app::Application app(argc, argv);
    return app.run();
}
