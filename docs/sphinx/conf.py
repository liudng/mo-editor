# SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
# SPDX-License-Identifier: GPL-3.0-or-later
# Configuration file for the Sphinx documentation builder.

import os
import sys

sys.path.insert(0, os.path.abspath('.'))

project = 'Mo'
author = 'liudng'
copyright = '2026, liudng'
release = '0.1.0'
version = '0.1.0'

extensions = [
    'breathe',
]

breathe_projects = {
    'mo': '../build/docs/xml',
}
breathe_default_project = 'mo'

templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

html_theme = 'sphinx_rtd_theme'
html_static_path = ['_static']

language = 'en'
