#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Convert the 30c3 Fahrplan for the Hackover badge
# Needs schedule.xml in current folder. You can get it at
# https://events.ccc.de/congress/2013/Fahrplan/schedule.xml
#
# Written by Roland Hieber <rohieb@rohieb.name>
#
# TODOs:
# - split into two fahrplan.lst's so badge does not crash when reading > 4KB
# - fetch current schedule.xml
# - hyphenation

import xml.etree.ElementTree as etree
import textwrap
import codecs
import string
from datetime import datetime

LINE_LENGTH = 14
SHORT_ROOMS = {
    "Saal 1": "S.1",
    "Saal 17": "S.17",
    "Saal 2": "S.2",
    "Saal 6": "S.6",
    "Saal G": "S.G",
    "Lounge": "Lnge"
}

wr = textwrap.TextWrapper(width=LINE_LENGTH, expand_tabs=False)
events = []
doc = etree.parse("schedule.xml")
root = doc.getroot()

def sanitize(s):
    s = s.replace(u"„",'"').replace(u"”",'"').replace(u"“",'"'). \
        replace(u"–","-").replace(u"—","-").replace(u"…","..."). \
        replace(u"‚","'").replace(u"‘","'").replace(u"’","'")
    s = s.encode("latin1", errors="replace")
    return s.strip()

def write_index(filename, events):
    events = sorted(events,
        key=lambda x: datetime.strftime(x[1], "%Y-%m-%dT%H:%M"))
    f = open(filename, "w")
    for e in events:
        f.write("%s %s|evnt%d.txt\n" % (datetime.strftime(e[1], "%a %H:%M"),
            SHORT_ROOMS[e[2]], int(e[0])))
    f.close()

for event in root.iter("event"):
    id = event.attrib["id"]
    title = event.find("title").text
    subtitle = event.find("subtitle").text
    abstract = event.find("abstract").text
    desc = event.find("description").text
    room = event.find("room").text
    date = datetime.strptime(event.find("date").text.rpartition("+")[0],
        "%Y-%m-%dT%H:%M:%S")
    track = event.find("track").text
    persons = []
    for p in event.find("persons").iter("person"):
        persons.append(p.text)
    
    rawfile = open("evnt%d.txt" % int(id), "wb")
    f = codecs.EncodedFile(rawfile, "latin1")

    f.write(sanitize(datetime.strftime(date, "%b %d, %H:%M")))
    f.write("\n")
    f.write(sanitize(room))
    f.write("\n\n")
    f.write(wr.fill(sanitize(title)))
    f.write("\n")

    if subtitle:
        f.write("\n")
        f.write(wr.fill(sanitize(subtitle)))
        f.write("\n")
    
    f.write("-"*LINE_LENGTH)
    f.write("\n")

    if len(persons) > 0:
        f.write(wr.fill(sanitize(", ".join(persons))))
        f.write("\n\n")

    if track and len(track) > 0:
        f.write(wr.fill("Track: %s" % sanitize(track)));
        f.write("\n\n")

    if not abstract:
        abstract = ""
    if not desc:
        desc = ""
    t = "\n\n".join((abstract.strip(), desc.strip()))
    if len(t) > 0:
        f.write(wr.fill(sanitize(t)))

    f.close()

    events.append((id, date, room))

write_index("fahrplan.lst", events)
