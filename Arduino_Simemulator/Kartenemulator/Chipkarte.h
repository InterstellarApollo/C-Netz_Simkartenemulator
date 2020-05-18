#ifndef Chipkarte_h
#define Chipkarte_h

#include "arduino.h"

#define CNTR  02
#define STAT  03
#define WRTE  04
#define READ  05
#define EXEC  06
#define AUTO  07

#define OEKART  1 // = Oeffentliches Kartentelefon
#define BTX     2 // = BTX
#define CNETZ   3 // = Mobilfunknetz C
#define RFV     4 // = Rufnummernverzeichnuss und Gebuehrenzaehler

#define CCADRESS 1

#define EBD   0   // Position der Einbuchdaten im EEPROM
#define GEBZ  9   // Position des Gebuehrenzaehlers im EEPROM
#define TELB  12  // Position des Telefonbuchs im EEPROM

class Chipkarte{
  public:
    static byte selectedApplication;
    static boolean master;
};

#endif
