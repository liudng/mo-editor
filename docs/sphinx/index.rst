Welcome to Hello's documentation!
=================================

Hello is a simple Qt6 text editor focused on multi-tab editing, syntax
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

Hello modules
-------------

.. doxygengroup:: core
   :project: hello
   :content-only:

.. doxygengroup:: ui
   :project: hello
   :content-only:

.. doxygenclass:: hello::core::Document
   :project: hello
   :members:

.. doxygenclass:: hello::core::Settings
   :project: hello
   :members:

.. doxygenclass:: hello::ui::MainWindow
   :project: hello
   :members:
