# SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
# SPDX-License-Identifier: GPL-3.0-or-later
# Configuration file for the Sphinx documentation builder.

import os
import sys

sys.path.insert(0, os.path.abspath('.'))

project = 'Mo'
author = 'Liu Dong'
copyright = '2026, Liu Dong'
release = '0.1.0'
version = '0.1.0'

extensions = []

# Conditionally enable Breathe (Doxygen bridge) only if the extension is
# installed AND the Doxygen XML output exists. This allows Sphinx to build
# standalone when Doxygen/Breathe are not available.
doxygen_xml = os.path.abspath('../build/docs/xml')
try:
    import breathe
    extensions.append('breathe')
    breathe_projects = {'mo': doxygen_xml}
    breathe_default_project = 'mo'
except ImportError:
    pass

# Templates and static assets — only list directories that actually exist.
templates_path = ['_templates'] if os.path.isdir('_templates') else []
html_static_path = ['_static'] if os.path.isdir('_static') else []

exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

# Use the Read the Docs theme if available, otherwise fall back to the
# built-in alabaster theme (always available with Sphinx).
try:
    import sphinx_rtd_theme
    html_theme = 'sphinx_rtd_theme'
except ImportError:
    html_theme = 'alabaster'

language = 'en'
