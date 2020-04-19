# C-Netz_Simkartenemulator

Funktion: 
<br> 
Der Sketch läuft bisher auf einem Arduino Mega, bei dem der "Data" Kontakt vom Kartenleser des Telefons mit beiden Pins (RX und TX) über 
einen Widerstand von Serial1 verbunden werden. Außerdem braucht der Arduino noch eine Verbindung für Reset (Pin 37) und Masse (GND). 
<br>
Debugausgaben werden über den Seriellen Port 0 bzw. USB ausgegeben. 
<br>
Pinout der Simkarte: 
https://en.wikipedia.org/wiki/Smart_card#/media/File:SmartCardPinout.svg 
<br> 
<br>
PS: Das Programm ist bisher noch ziemlich unvollständig und viele Werte sind Hartkodiert.
