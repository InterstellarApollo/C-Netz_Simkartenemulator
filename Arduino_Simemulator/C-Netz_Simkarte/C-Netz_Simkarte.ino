#include <EEPROM.h>

#define ATRLEN      18

// Befehlsklassen definieren
#define CNTR        02
#define STAT        03
#define WRTE        04
#define READ        05
#define EXEC        06
#define AUTO        07

// EEPROM- Adressen definieren
#define EBD   0
#define GEBZ  9
#define TELB  12

#define TELB_S 5

byte atr[] = {
    59,   136,  142, 254, 
    83,   42,   3,    30, 
    4,    146,  128,  0, 
    65,   50,   54,   1, 
    17,   228
    };

byte sendeFolgeZaehler = 0;
byte empfangsFolgeZaehler = 0;

byte _block[255];
byte *_adresse      = &_block[0];
byte *_steuerfeld   = &_block[1];
byte *_datenlaenge  = &_block[2];
byte *_ICB1         = &_block[3];
byte *_iccrc        = &_block[4];
byte *_APRC         = &_block[5];
byte *_dlng         = &_block[6];
byte *_daten        = &_block[7];
// Prüfsumme muss manuell angehängt werden! 

byte ccAdresse = 1;
byte ftelAdresse = 0;

void setup() {
  pinMode(43, OUTPUT);  // Buchse 1
  pinMode(41, OUTPUT);  // Buchse 2
  pinMode(37, INPUT);   // Buchse 3
  pinMode(39, OUTPUT);  // Buchse 4
  pinMode(20, OUTPUT);  // Buchse 6
  
  
  //pinMode(18, OUTPUT);  // Buchse 5 TX
  //pinMode(19, OUTPUT);  // Buchse 5 RX
  
  pinMode(52, OUTPUT);  // Schalter 1
  pinMode(53, OUTPUT);  // Schalter 2

  digitalWrite(43, LOW);
  digitalWrite(41, LOW);
  
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  Serial.begin(230400);
  
  Serial1.begin(9600, SERIAL_8E2);
  
  waitToResetCard();
}

void loop() {
  while(!waitWithTimeout(100)){
    if(digitalRead(37) == LOW){
      sendeFolgeZaehler = 0;
      empfangsFolgeZaehler = 0;
      waitToResetCard();
    }
  }

  byte adresse = 0;
  byte steuerfeld = 0;
  byte datenLaenge = 0;
  byte chksum = 0;

  boolean timeout = false;

  adresse =                               (timeout = waitWithTimeout(20))?readChar():0;
  steuerfeld =                            (timeout = waitWithTimeout(20))?readChar():0;
  datenLaenge =                           (timeout = waitWithTimeout(20))?readChar():0;
  byte inffeld[datenLaenge];
  for(int index = 0; index < datenLaenge; index++){
    inffeld[index] =                      (timeout = waitWithTimeout(20))?readChar():0;
  }

  if((adresse & 00001111) == 0b00000001){
    ftelAdresse = ((0b11110000 & adresse) >> 4);
  }
  Serial.println("********Datenblock (empf): ********");
  Serial.print((String)adresse + ", ");
  Serial.print((String)steuerfeld + ", ");
  Serial.print((String)datenLaenge + ", ");  
  for(int index = 0; index < datenLaenge; index++){
    Serial.print((String) inffeld[index] + ", ");
  }
  chksum =                                (timeout = waitWithTimeout(20))?readChar():0;
  Serial.println((String) chksum + ", ");





  
  if(timeout){
    parseInffield(inffeld, datenLaenge);
  
    //Serial.println("Checksum: " + (String) chksum);
    empfangsFolgeZaehler++;

    // Antwort bauen: 
    *_adresse = (ccAdresse << 4) + ftelAdresse;
    //Serial.println("Neue Adresse: " + (String) *_adresse);
    *_steuerfeld = (empfangsFolgeZaehler << 5) + (sendeFolgeZaehler << 1);
    //Serial.println("Neues Steuerfeld: " + (String) *_steuerfeld);
    *_datenlaenge = *_dlng + 4;
    //Serial.println("Neue Datenlaenge: " + (String) *_datenlaenge);

    Serial.println("********Datenblock (send.): ********");
    chksum = 0;
    for(int i = 0; i < *_datenlaenge + 3; i++){   // +3? Adresse, Steuerfeld und Datenlänge sind nicht in der Datenlänge mitinbegriffen!
      Serial.print((String)_block[i] + ", ");
      chksum = chksum ^ _block[i];
    }
    _block[*_datenlaenge + 3] = chksum;
    Serial.println((String) +  _block[*_datenlaenge + 3]);

    sendBlock();
  
    sendeFolgeZaehler++;
    Serial.println("-------------------------------");
  } else {
    Serial.println("TIMEOUT");
  }
}

void parseInffield(byte inffeld[], byte dlen){
  byte icb1 = inffeld[0];

  boolean ICBExtension            = (icb1 & 0b10000000) > 0;
  boolean chaining                = (icb1 & 0b01000000) > 0;
  boolean error                   = (icb1 & 0b00100000) > 0;
  boolean terminate               = (icb1 & 0b00010000) > 0;
  boolean waitingTimeExtension    = (icb1 & 0b00001000) > 0;
  boolean master                  = (icb1 & 0b00000100) > 0;
  boolean confirm                 = (icb1 & 0b00000010) > 0;
  boolean online                  = (icb1 & 0b00000001) > 0;

  //Serial.println("ICB1: " + (String) icb1);

  //Serial.println("online: " +         (String)online);
  //Serial.println("confirm: " +        (String)confirm);
  //Serial.println("master: " +         (String)master);
  //Serial.println("wte: " +            (String)waitingTimeExtension);
  //Serial.println("terminate: " +      (String)terminate);
  //Serial.println("error: " +          (String)error);
  //Serial.println("chaining: " +       (String)chaining);
  //Serial.println("ICBExtension: " +   (String)ICBExtension);

  boolean abortChaining = (chaining && error);

  byte *apdu = &inffeld[1];
  parseAPDU(apdu, dlen - 1);

  
  // Antwort bauen
  
  ICBExtension = false;
  chaining = chaining;      // Auf Chaining folgt antwort mit Chaining und Confirm
  error = abortChaining;
  terminate = false;
  waitingTimeExtension = false;
  master = !master;         // Wenn Request vom Master dann ist CC KEIN Master
  confirm = chaining || abortChaining;
  online = online;
  
  *_ICB1 = 
    (ICBExtension           << 7) + 
    (chaining               << 6) + 
    (error                  << 5) + 
    (terminate              << 4) + 
    (waitingTimeExtension   << 3) + 
    (master                 << 2) + 
    (confirm                << 1) + 
    (online                 << 0) ;

  //Serial.println("Neues ICB1: " + (String) *_ICB1);
}

void parseAPDU(byte apdu[], byte dlen){
  boolean I   = apdu[0] & 0b10000000;
  byte cla    = apdu[0] & 0b01111111;
  byte ins    = apdu[1];
  byte s7dlen = apdu[2];

  byte datenblock[dlen];

  for(int i = 0; i < s7dlen; i++){
    datenblock[i] = apdu[i + 3];
  }

  //Serial.println("I: " + (String) I);
  //Serial.println("cla: " + (String) cla);
  //Serial.println("ins: " + (String) ins);
  //Serial.println("s7dlen: " + (String) s7dlen);

  if(cla == CNTR && ins == 0xF1){
    Serial.println("SL-APPL");
    // Antwort: DLNG = 0, CCRC
    *_iccrc = 0b10000000      | 0b00000100;      
    *_APRC  = (0b0000 << 4)   | 0b0000;
    *_dlng  = 0;
    
  } else if(cla == CNTR && ins == 0xF2){
    Serial.println("CL-APPL");
    // Antwort: Leer



    
  } else if(cla == CNTR && ins == 0xF3){
    Serial.println("SH-APPL");
    // Antwort: Länge dess APP- IDN, APP- IDN, Bezeichnung der Applikation "APP- TXT" und Applikationsstatus
    // Kein Weiterer Datensatz: Leer



    
  } else if(cla == STAT && ins == 0xF1){
    Serial.println("CHK-KON");
    // Antwort: CCRC


  } else if(cla == READ && ins == 0x01){
    Serial.println("RD-EBD");
    *_iccrc = 0b10000000      | 0b00000100;      
    *_APRC  = (0b0000 << 4)   | 0b0000;
    *_dlng  = 9;
    
    *(_daten + 0) = EEPROM.read(EBD + 0);   // Rufnummer Teil 1
    *(_daten + 1) = EEPROM.read(EBD + 1);   // Rufnummer Teil 2
    *(_daten + 2) = EEPROM.read(EBD + 2);   // Rufnummer Teil 3

    *(_daten + 3) = EEPROM.read(EBD + 3);   // Sicherungscode Teil 1
    *(_daten + 4) = EEPROM.read(EBD + 4);   // Sicherungscode Teil 2
    
    *(_daten + 5) = EEPROM.read(EBD + 5);   // Kartenkennung (3b) und Sicherheitsschlüssel (5b) Teil 1
    *(_daten + 6) = EEPROM.read(EBD + 6);   // Sicherheitsschlüssel Teil 2
    
    *(_daten + 7) = EEPROM.read(EBD + 7);   // Wartungsschlüssel Teil 1
    *(_daten + 8) = EEPROM.read(EBD + 8);   // Wartungsschlüssel Teil 2
  } else if(cla == READ && ins == 0x02){
    Serial.println("RD-RUFN");
    *_iccrc = 0b10000000      | 0b00000100;      
    *_APRC  = (0b0000 << 4)   | 0b0000;

    byte requestedIndex = datenblock[0];

    if(requestedIndex == 0){
      Serial.println("Header");
      *_dlng  = 24;
      
      byte anzRN = TELB_S;

      *(_daten + 0) = anzRN;
      
      for(int i = 0; i < 24; i++){
        *(_daten + 1 + i) = EEPROM.read(TELB + i);
      }    
    } else {
      *_dlng  = 24;
      Serial.println("Index Nr.: " + (String)requestedIndex);
      for(int i = 0; i < 24; i++){
        *(_daten + i) = EEPROM.read(TELB + 24*requestedIndex + i);
      }  
    }
  } else if(cla == READ && ins == 0x03){
    Serial.println("RD-GEBZ");
    *_iccrc = 0b10000000      | 0b00000100;      
    *_APRC  = (0b0000 << 4)   | 0b0000;
    *_dlng  = 3;
    
    *(_daten + 0) = EEPROM.read(GEBZ + 0);
    *(_daten + 1) = EEPROM.read(GEBZ + 1);
    *(_daten + 2) = EEPROM.read(GEBZ + 2);
  } else if(cla == WRTE && ins == 0x01){
    Serial.println("WT-RUFN");
    byte requestedIndex = datenblock[0];

    boolean blank = false;
    for(int i = 0; i < 24; i++){
      i
      EEPROM.update(TELB + 24*requestedIndex + i, datenblock[i+1]);
    }

    byte index = TELB + requestedIndex /4;
    byte indexBlock = EEPROM.read(index);
    byte bytemaske = ~(0b10000000 >> ((requestedIndex - 1) % 4));
    byte neuerIndexBlock = indexBlock & bytemaske;
    EEPROM.update(index, neuerIndexBlock);
    
    *_iccrc = 0b10000000      | 0b00000100;      
    *_APRC  = (0b0000 << 4)   | 0b0000;
    *_dlng  = 0;
    Serial.println("Index Nr.: " + (String)requestedIndex);
  } else if(cla == EXEC && ins == 0x02){
    Serial.println("EH-GEBZ");
  } else if(cla == AUTO && ins == 0x01){
    Serial.println("CL-GEBZ");
  } else if(cla == EXEC && ins == 0xF1){
    Serial.println("CHK-PIN");
    // Antwort: CCRC
  } else if(cla == EXEC && ins == 0xF2){
    Serial.println("SET-PIN");
  }
}

byte readChar(){
  while(Serial1.available() == 0){
    
  }
  return Serial1.read();
}


void waitToResetCard(){
  Serial.println("-----------------------------");
  Serial.println("Karte wird geresettet");
  while(digitalRead(37) == LOW){
  }
  Serial.println("Reset beendet");
  Serial.println("Bereit, um Daten zu senden. ");

  // Sende ATR:
  Serial1.write(atr, ATRLEN);

  Serial.println("ATR gesendet");

  for(int i = 0; i < ATRLEN; i++){
    if(waitWithTimeout(20)){
      Serial1.read();
    } else {
    
    }
  }
}

boolean waitWithTimeout(int timeout){
  int timeoutCounter = 0;
  
  while(Serial1.available() == 0){
    delay(1);
    timeoutCounter++;
    if(timeoutCounter >= timeout){
      return false;
    }
  }
  return true;
}


void sendBlock(){
  Serial1.write(_block, *_datenlaenge + 4);
  for(int i = 0; i < *_datenlaenge + 4; i++){
    waitWithTimeout(100);
    Serial.print((String) readChar() + ", ");
  }
  Serial.println();
}
