#include "CNetz.h"

boolean pinNotOk = false;
boolean abfzNull = false;
boolean aprcValid = true;
boolean r1;
boolean r2;
boolean r3;
boolean generalError = false;
boolean ident = true;         // Weil Antwort

static void CNetz::runInstruction(byte cla, byte ins, byte *data, Command *c){
  boolean isInstructionAvailable = false;
  byte asts = 0;

  switch(cla){
    case CNTR:
      break;
    case STAT: 
      break;
    case WRTE:
      switch(ins){       
        case WT_RUFN:
          writeRufn(c, data);
          isInstructionAvailable = true;
        break;
      }
      break;
    case READ:
      switch(ins){
        case RD_EBD:
          readEbd(c, data);
          isInstructionAvailable = true;
        break;        
        case RD_RUFN:
          readRufn(c, data);
          isInstructionAvailable = true;
        break;
      }
      break;
    case EXEC:
      break;
    case AUTO:
      break;
  }

  byte ccrc = 0b00000000;
  
  generalError = !isInstructionAvailable;
  
  ccrc = ccrc | (pinNotOk<<0) | (abfzNull<<1) | (aprcValid<<2) | (r1<<3) | (r2<<4)| (r3<<5) | (generalError<<6) | (ident<<7);
  c->setClass_CCRC(ccrc);
}

static void CNetz::readRufn(Command *c, byte *data){
  byte angeforderterRufnummernsatz = *(data + 0);
  Serial.println("Rufnummernsatz lesen");
  Serial.println("Kurzwahlnummer: " + (String) angeforderterRufnummernsatz);

  byte dlen = 24;
  byte daten[dlen];
                       
  if((*data + 0) == 0){
    // Header lesen
    for(int i = 0; i < 24; i++){              
      daten[i] = EEPROM.read(TELB + i);  
    }                     
  } else {
    // Rufnummernsatz lesen
    for(int i = 0; i < 24; i++){
      *(daten + i) = EEPROM.read(TELB + 24*angeforderterRufnummernsatz + i);
    }                                       
  }
  
  c->setData(&daten[0], dlen);
}

static void CNetz::writeRufn(Command *c, byte *data){
  byte angeforderterRufnummernsatz = *(data + 0);
  Serial.println("Rufnummernsatz schreiben");
  Serial.println("Kurzwahlnummer: " + (String) angeforderterRufnummernsatz);
  String kontaktname = "";
  String kontaktrufnummer = "";
  if(*(data + 9) != 32){
    for(byte i = 1; i < 9; i++){
      if((*(data + i) & 0b11110000) != 0b11110000){
        kontaktrufnummer += (String) ((*(data + i) & 0b11110000) >> 4);
      }
      if((*(data + i) & 0b00001111) != 0b00001111){
        kontaktrufnummer += (String) (*(data + i) & 0b00001111);
      }
    } 
    for(int i = 9; i < 24; i++){
      if(*(data + i) != 32){
        kontaktname += (char) *(data + i);
      }
    } 
    Serial.println("Name: " + kontaktname);
    Serial.println("Nummer: " + kontaktrufnummer);
    for(int i = 0; i < 24; i++){
      EEPROM.update(TELB + 24*angeforderterRufnummernsatz + i, *(data + i));
    } 
    setTBEntry(angeforderterRufnummernsatz); 
  } else {
    Serial.println("Nummernsatz leer. ");
    clearTBEntry(angeforderterRufnummernsatz); 
  }
  c->setData(0, 0);
}

static void CNetz::readEbd(Command *c, byte *data){
  Serial.println("Einbuchdaten lesen");
  byte dlen = 9;
  byte daten[dlen];

  daten[0] = EEPROM.read(EBD + 0);
  daten[1] = EEPROM.read(EBD + 1);
  daten[2] = EEPROM.read(EBD + 2);
  daten[3] = EEPROM.read(EBD + 3);
  daten[4] = EEPROM.read(EBD + 4);
  daten[5] = EEPROM.read(EBD + 5);
  daten[6] = EEPROM.read(EBD + 6);
  daten[7] = EEPROM.read(EBD + 7);
  daten[8] = EEPROM.read(EBD + 8);
  
  c->setData(&daten[0], dlen);
}

static void CNetz::setTBEntry(byte toSet){
  //EEPROM.update(TELB + i, *(data + i));
  toSet--;
  Serial.println("Nummer: " + (String) toSet);
  byte block = toSet / 8;
  byte pos = toSet % 8;
  Serial.println("Stelle " + (String) pos + " in Block " + (String) (block + 1));

  byte phoneBlock = EEPROM.read(TELB + block + 1);
  Serial.println("Gelesen: " + (String) phoneBlock);
  byte mask = 0b10000000 >> pos;
  phoneBlock = phoneBlock & ~mask;
  Serial.println("Zu schreiben: " + (String) phoneBlock);
  EEPROM.update(TELB + block + 1, phoneBlock);
}

static void CNetz::clearTBEntry(byte toClear){
  //EEPROM.update(TELB + i, *(data + i));
  toClear--;
  Serial.println("Nummer: " + (String) toClear);
  byte block = toClear / 8;
  byte pos = toClear % 8;
  Serial.println("Stelle " + (String) pos + " in Block " + (String) (block + 1));

  byte phoneBlock = EEPROM.read(TELB + block + 1);
  Serial.println("Gelesen: " + (String) phoneBlock);
  byte mask = 0b10000000 >> pos;
  phoneBlock = phoneBlock | mask;
  Serial.println("Zu schreiben: " + (String) phoneBlock);
  EEPROM.update(TELB + block + 1, phoneBlock);
}
