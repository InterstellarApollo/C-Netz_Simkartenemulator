# C-Netz_Simkartenemulator

Funktion: 
<br> 
Der Sketch läuft bisher auf einem Arduino Mega, bei dem der "Data" Kontakt vom Kartenleser des Telefons mit beiden Pins (RX und TX) über 
einen Widerstand von Serial1 verbunden werden. Außerdem braucht der Arduino noch eine Verbindung für Reset (Pin 37) und Masse (GND). 
<br>
Debugausgaben werden über den Seriellen Port 0 (USB) ausgegeben. 
<br>
Pinout der Simkarte: 
https://en.wikipedia.org/wiki/Smart_card#/media/File:SmartCardPinout.svg 
<br>
Dokumentation für das Kommunikationsprotokoll: http://download.eversberg.eu/mobilfunk/C-Netz-Dokus/FTZ%20171%20TR%2060%20-%20Anhang%201%20Berechtigungskarte%20als%20Prozessorkarte.pdf
<br>
<br>
PS: Das Programm ist bisher noch ziemlich unvollständig und viele Werte sind Hartkodiert.
