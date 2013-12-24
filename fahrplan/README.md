30c3 Fahrplan for the Hackover 2013 Badge
=================

This directory contains the Fahrplan for 30c3, converted for the Hackover 2013
Badge, as well as the script to do the conversion from a Frab XML export.

Simply drop all the `*.txt` files in this directory onto the FAT partition on
your Hackover Badge.  Since the Badge crashes for index files greater than about
150 lines, there are two index files: `fahrplan.lst` contains the events for day
1 and 2, and `fahrpla2.lst` contains the events for day 3 and 4. To display
those events, simply move `fahrpla2.lst` to `fahrplan.lst`.

The conversion script is `build-fahrplan.py`, which reads the Frab XML export
from `schedule.xml` in the local folder. You can get an up-to-date export from
`https://events.ccc.de/congress/2013/Fahrplan/schedule.xml`.

TODOs
-----
* Currently, no hyphenation is done yet when converting events.
