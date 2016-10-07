Fahrplan for the Hackover 2013 Badge
=================

This directory contains the Fahrplan, converted for the Hackover 2013
Badge, as well as the script to do the conversion from a Frab XML export.

1. Drop all the `evnt*.txt` files into the `fahrplan/` directory on the FAT
   partition of your Hackover Badge.

2. Since the Badge does not have enough memory and crashes with index files
   greater than 150 lines, there are two index files: `fahrplan.lst`
   contains the events for day 1 and 2, and `fahrpla2.lst` contains the events
   for day 3 and 4. Copy one of them to `fahrplan/fahrplan.lst` on the Badge.
   (You can copy both, but only the one named `fahrplan.lst` is displayed.)

The conversion script is `build-fahrplan.py`, which reads the Frab/Pentabarf XML
export from `schedule.xml` in the local folder. You can get an up-to-date export
from `https://events.ccc.de/congress/2013/Fahrplan/schedule.xml` and run the
script without arguments to update the event and index files.

TODOs
-----
* Currently, no hyphenation is done yet when converting events.
