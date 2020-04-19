#include <EEPROM.h>

#define EBD   0
#define GEBZ  9
#define TELB  12

#define TELB_S

void setup() {
    byte rufn00 = 0b01100010;     // Rufnummer Teil 1
    byte rufn01 = 0b01011100;     // Rufnummer Teil 2
    byte rufn02 = 0b11000101;     // Rufnummer Teil 3

    byte sc00 = 0b01110001;       // Sicherungscode Teil 1
    byte sc01 = 0b10000100;       // Sicherungscode Teil 2
    
    byte kkshs00 = 0b01100000;    // Kartenkennung (3b) und Sicherheitsschlüssel (5b) Teil 1
    byte kkshs01 = 0b00000000;    // Sicherheitsschlüssel Teil 2
    
    byte ws00 = 0b11111111;       // Wartungsschlüssel Teil 1
    byte ws01 = 0b11111111;       // Wartungsschlüssel Teil 2

    EEPROM.update(EBD + 0, rufn00);
    EEPROM.update(EBD + 1, rufn01);
    EEPROM.update(EBD + 2, rufn02);
    EEPROM.update(EBD + 3, sc00);
    EEPROM.update(EBD + 4, sc01);
    EEPROM.update(EBD + 5, kkshs00);
    EEPROM.update(EBD + 6, kkshs01);
    EEPROM.update(EBD + 7, ws00);
    EEPROM.update(EBD + 8, ws01);

    EEPROM.update(GEBZ + 0, 0b00000000);
    EEPROM.update(GEBZ + 1, 0b00000000);
    EEPROM.update(GEBZ + 2, 0b00000011);

    for(int i = 0; i < 24; i++){              // Telefonbuch leer füllen
      EEPROM.update(TELB + i, 0b11111111);    //
    }                                         //
}

void loop() {
}
