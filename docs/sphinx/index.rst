Welcome to Mo's documentation!
=================================

Mo is a simple Qt6 text editor focused on multi-tab editing, syntax
highlighting, automatic encoding detection, session restore and find/replace,
running natively on Wayland.

.. toctree::
   :maxdepth: 2
   :caption: Contents:

   architecture
   api

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

API reference
=============

The C++ API reference is generated from the in-tree source by Doxygen and
exposed here through Breathe.

Mo modules
----------

.. doxygengroup:: core
   :project: mo
   :content-only:

.. doxygengroup:: ui
   :project: mo
   :content-only:

.. doxygenclass:: mo::core::Document
   :project: mo
   :members:

.. doxygenclass:: mo::core::Settings
   :project: mo
   :members:

.. doxygenclass:: mo::ui::MainWindow
   :project: mo
   :members:
