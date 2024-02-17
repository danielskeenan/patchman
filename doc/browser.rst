Browser
=======

The Browser is the first window shown when opening Patchman.

.. image:: img/browser.png

Upon launch, the browser will scan all folders set in the settings (see
:doc:`first_time` for how to change these folders); progress is illustrated at
the bottom of the window.

A table lists the type, name, date, rack count, and checksum for each ROM found.
If files are missing, ensure the path they are in is set as a search path in the
settings and that it is a :ref:`supported file type <supported-systems>`.

To edit a file, double-click on its entry.

Further options are available by right-clicking on its entry:

.. image:: img/browser_menu.png

Edit ROM
   Open the :doc:`editor`. Same action as double-clicking on the row.

Create Report
   Create a document visually demonstrating, for each rack in the ROM, the
   location of each DMX address. Suitable for printing and including with a ROM
   chip or control module containing the ROM to document its configuration.

Show Duplicates
   Show all ROM files that contain the same patch information as the selected
   ROM file.

Show in Explorer
   Open the file browser and highlight the file.
