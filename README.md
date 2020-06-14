# C-Netz_Simkartenemulator

Funktion:
<br>
Der Sketch läuft bisher auf einem Arduino Mega, bei dem der "Data" Kontakt vom Kartenleser des Telefons mit beiden Pins (RX und TX) über
einen Widerstand von Serial1 verbunden werden (Der Widerstand ist bei meinem Siemens C5 4,7k,
<br>
quasi genau wie hier beschrieben: https://forum.arduino.cc/index.php?topic=114553.0).
<br>
Außerdem braucht der Arduino noch eine Verbindung für Reset (Pin 37) und Masse (GND).
<br>
Debugausgaben werden über den Seriellen Port 0 (USB) ausgegeben. Bevor die "Karte" verwendet werden kann, muss der EEPROM noch mit den Einbuchdaten gefüllt werden,
<br>
also zuerst muss der "EEPROM-Prepare"-Sketch hochgeladen werden, dort ließen sich auch diverse andere Schlüssel eintragen (quasi als Wartungskarte verwendbar, funktioniert mit dem Siemens C5).  
<br>
Pinout der Simkarte:
https://en.wikipedia.org/wiki/Smart_card#/media/File:SmartCardPinout.svg
<br>
Dokumentation für das Kommunikationsprotokoll: http://download.eversberg.eu/mobilfunk/C-Netz-Dokus/FTZ%20171%20TR%2060%20-%20Anhang%201%20Berechtigungskarte%20als%20Prozessorkarte.pdf
<br>
bzw. andere ABC(usw.)-Netz relevante Infos von Osmocom Analog: http://osmocom-analog.eversberg.eu
<br>
<br>
Das Programm funktioniert bisher wahrscheinlich nur mit C-Netz Telefonen,
<br>
die eine ältere Prozessorchipkarte unterstützen (wahrscheinlich implementierte C-Netz Softwareversion 001, siehe unbekannte nicht unterstützte Befehle unten).
<br>
...Außerdem stürzt der Sketch bei meinem Arduino alle ~3 Stunden ab...

<br>
<br>

Eigenheiten und / oder Bugs:
<br>
- Nachdem die PIN eingegeben wurde wird im Bildschirm die Tel-Nr. angezeigt und danach nochmal "Kein Code". (auch bei einer Original TeleKarte)
<br>
Wahrscheinlich ist damit die PIN für das Rufnummernverzeichnis gemeint, die allerdings nicht gesetzt werden kann und auch
<br>
nicht beachtet wird. (Siemens C5)
<br>
<br>
Unterstütze und nicht unterstützte Kommandos:
<br>
- Select Application: Ja
<br>
- Pin prüfen: Ja
<br>
- C-Netz, Einbuchdaten lesen: Ja
<br>
- Rufnummernverzeichnis+Gebührenzähler, Rufnummernsatz einprogrammieren: Ja
<br>
- Rufnummernverzeichnis+Gebührenzähler, Rufnummernsatz lesen: Ja
<br>
- Rufnummernverzeichnis+Gebührenzähler, Gebührenzähler erhöhen: Ja
<br>
<br>
- Rufnummernverzeichnis+Gebührenzähler, Gebührenzähler lesen: Nein
<br>
- Show Application: Nein
<br>
- Unbekannter Befehl: Klasse 0x05 (Read), Befehl 0xf4: Nein
<br>
- Unbekannter Befehl: Klasse 0x05 (Read), Befehl 0cf5: Nein
<br>
