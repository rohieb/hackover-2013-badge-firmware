Mini-Dokumentation: Hackover-Badge-Firmware
===========================================

Komponenten
-----------

In diesem Sourcetree befindet sich der Quellcode für

* die Firmware der Hackover-Badge
* Levelkonverter für Hackio und Gladio
* einen Bildkonverter, der Bilder in Titelbilder für die Badge umwandelt
* einen Simulator, der Gladio und Hackio auf einem PC spielbar macht
* einen lange nicht angefassten Port von Hackio für emscripten

Voraussetzungen
---------------

### Allgemeine Caveats

An einigen Stellen wird mit Binärformaten gearbeitet; dabei werden derzeit nur
Little-Endian-Architekturen unterstützt. Das wird für wenige Leute ein Problem
sein, aber wenn Dinge auf einer SPARC-Maschine unerwartet explodieren, liegt es
daran.

Das zieh ich irgendwann noch gerade, aber solange sich niemand bei mir
beschwert, hat es geringe Priorität.

### Firmware

* gcc
* gcc für ARM (arm-none-eabi) mit newlib

### Levelkonverter

* g++
* Boost >= 1.50.0
* Ein anderer C++-Compiler kann in der Makefile eingerichtet werden

### Bild-Konverter

* g++ 
* SFML 2.x

### Simulator

* g++ oder clang++
* Boost >= 1.50.0
* gtkmm 3.x
* die Levelkonverter

### Hackio-Browserport

* emscripten
* SDL 1.2 (Header reichen)

Kompilation und Deployment
--------------------------

### Firmware

Im Hauptverzeichnis

    make

aufrufen (Nein? Doch! Oh!).

Um die neue Firmware für die Badge des Hackovers 2013 zu bauen, muss in der Makefile
die Zeile

    OPTDEFINES += -DHOB_2013

auskommentiert werden.

Die gebaute Datei heißt

    firmware.bin

Dann die Badge an den USB-Port anschließen, das Steuerkreuz nach oben (Badge
2014) oder in der Mitte (Badge 2013) drücken, den Reset-Taster betätigen, und
das jetzt erscheinende USB-MSC-Gerät mounten.

Aufgrund eines Bugs in der Firmware des LPC1343 kann es einen Moment dauern, bis
der USB-"Stick" auftaucht. Es ist mir mit meinem Laptop schon passiert, dass der
Kernel das Gerät wieder abhängt, bevor es sich identifiziert hat; in diesem Fall
das Steuerkreuz erneut in die geeignete Stellung halten und den Reset-Taster
drücken. Der Bug ist ein nicht initialisierter Zähler, der einmal überlaufen
muss, bevor der USB-Treiber im LPC1343 den richtigen Zustand erreicht; beim
zweiten Reset wird der Zähler näher am Überlauf sein als beim ersten.

Ist das Gerät eingebunden, so sollte sich im Mountpoint eine "Datei"
firmware.bin befinden, die jetzt mit der selbstgebauten firmware.bin
überschrieben wird:

    dd conv=nocreat,notrunc if=firmware.bin of=/media/foobar/firmware.bin
    sync

Dann den USB-Stick unmounten. Die neue Firmware ist auf der Badge.

### Dataflash

Der Dataflash wird mit einem FAT-Image (dataflash.img) beschrieben. Die
einfachste Methode, dieses Image zu modifizieren ist, es zu mounten:

    mount -o loop dataflash.img /some/directory

...und dann darin herumzuschreiben. Dazu, welche Daten wozu da sind, steht
weiter unten mehr.

Caveat: "FAT" heißt hier FAT-12. Das bedeutet Dateinamen im 8.3-Format, sonst
kann die Badge-Firmware nicht damit umgehen.

Der Dataflash kann auch auf der Badge direkt modifiziert werden. Dazu die Badge
an den USB-Port anschließen und im Menü den Punkt "USB-Modus" auswählen. Es wird
sich ein 512KB großer USB-"Stick" melden, der gemountet werden oder auf den ein
modifiziertes Image z.B. mit dd aufgespielt werden kann.

### Level- und Bildkonverter

Im Hauptverzeichnis

    make tools

oder in badge/tools

    make

Die erstellten Programme liegen in badge/tools. Die Levelkonverter nehmen eine
Liste von Dateien, die auf .lv enden und legen neben diesen Dateien enstprechende
.lvl-Dateien an. Diese sind die Binärversionen der Level.

badge/tools/mktitleimg nimmt als Parameter eine Bilddatei und legt im
Arbeitsverzeichnis eine Datei titleimg.dat an, die in den Dataflash kopiert
werden kann, um das Titelbild der Badge zu ändern.

### Simulator

Im Hauptverzeichnis

    make mock

oder im Verzeichnis mock/

    make

Zum Starten des Simulators in einem der Verzeichnisse

    make play

Das erstellt aus allen .lv-Dateien in den Verzeichnisen badge/jumpnrun/levels/ und
badge/gladio/levels/ die zugehörigen .lvl-Dateien, macht sie dem Simulator bekannt
und startet diesen danach. So kann man zum Beispiel Level probespielen, die man
gerade baut, ohne jedes mal Dinge auf die Badge kopieren zu müssen.

### Hackio-Browserport

Im Hauptverzeichnis

    make html

bzw. im Verzeichnis browser/

    make

Dataflash
---------

Der Dataflash beinhaltet:

* Das Titelbild (titleimg.dat)
* den Fahrplan (Verzeichnis fahrplan/)
* Hackio-Level und -Spielstand (Verzeichnis hackio/)
* Gladio-Level und -Highscores (Verzeichnis gladio/)

### Titelbild

Ein Titelbild kann mit dem Bild-Konverter aus allen Bildformaten erstellt
werden, die SFML2 versteht; das sind alle gängigen. Das Bild muss 96x65 Pixel
groß sein. Farben werden nach Helligkeit in ein monochromes Bild umgerechnet,
aber wie gut das Ergebnis der Umwandlung aussieht ist mehr oder weniger
Glückssache. Im Zweifel ist es sinnvoller, das Bild schon schwarz/weiß in den
Konverter zu werfen.

### Fahrplan

Der Fahrplan ist eine Liste (fahrplan/fahrplan.lst); jede Zeile bezeichnet
einen Eintrag. Das Format einer Zeile ist

    <titel>|<datei>

Der Titel (nicht länger als 14 Zeichen) wird im Fahrplanmenü angezeigt, die
Datei ist eine mit iso-8859-1 kodierte Textdatei, deren Zeilen nicht länger als
14 Zeichen sind.

### Hackio-Level

Die Datei hackio/levels.lst ist eine Auflistung der vorhandenen Level, die dem
Format der fahrplan.lst folgt. Die Level-Dateien sind keine Textdateien, sondern
in einem Binärformat kodiert, das vom Levelkonverter badge/tools/level-converter
aus einem Textformat erstellt wird.

Das Textformat, aus dem die Level erstellt werden, ist selbsterklärend. Der
geneigte Leser mag im Editor seiner Wahl (also emacs) die Datei
badge/jumpnrun/levels/smb.lv öffnen, seinen Editor so umstellen, dass er
überlange Zeilen nicht wrappt (so dass er scrollen muss, um zum Ende der Zeile
zu kommen), und wenn er dann nicht versteht, wie man damit Level bauen kann,
kann ich ihm auch nicht helfen. Es ist nicht weit von ASCII-Art entfernt.

Einzige potentielle Überraschungsquelle: Der ini-ähnliche Teil unter dem quasi
gemalten Level wird tatsächlich geparst. Wenn man lustig ist, kann man sich
eigene Buchstaben für Gegnertypen einrichten, aber im Zweifel ist das
sinnvollste, die Liste einfach in eigene Level reinzukopieren.

### Gladio-Level

Die Textdatei gladio/gld\_lvls.lst ist eine Auflistung der Leveldateien in der
Reihenfolge, in der sie gespielt werden. Eine Leerzeile bedeutet einen
Weltensprung in der Levelzählung (z.B. von 1-3 nach 2-1 statt 1-4). Die Level
snd in einem Binärformat kodiert, das vom Levelkonverter
badge/tools/gladio\_level\_convert erstellt wird.

Das Textformat für Gladio-Level ist zeilenbasiert. Leerzeilen oder Zeilen, die
mit # anfangen, werden ignoriert, alle anderen Zeilen müssen dem Format

    <pause> <y-position> <gegnername>

entsprechen.

Die Struktur von Gladio-Leveln ist die eines zeitlichen (nicht räumlichen)
Ablaufs. Jede Zeile bedeutet: Nach <pause> ticks wird an der vertikalen Position
<y-position> der Gegner <gegnername> gespawnt. Der Ablauf ist zeitlich organisiert,
damit Gegner auch von hinten kommen können.

Simulator
---------

Der Simulator wird bedient mit den Tasten E, S, D, F (Steuerkreuz) sowie
J und K (Knöpfe).
