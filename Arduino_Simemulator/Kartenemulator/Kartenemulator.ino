#include "UBlockParser.h"
#include "ICLParser.h"
#include "BefehlsParser.h"
#include "Command.h"

#define ATRLEN      18
byte atr[] = {
    59,   136,  142, 254, 
    83,   42,   3,    30, 
    4,    146,  128,  0, 
    65,   50,   54,   1, 
    17,   228
};

byte uBlock[255];
byte temp_array[255];

void setup() {
  // IO wird mit Serial1 RX & TX verbunden 
  
  pinMode(43, OUTPUT);    // Buchse 1 (Masse)
  pinMode(41, OUTPUT);    // Buchse 2 (Masse)         <--- Massepin (Karte)
  pinMode(37, INPUT);     // Buchse 3 (Reset)         <--- Pin fuer Reset
  
  digitalWrite(43, LOW);  // Masse für CC
  digitalWrite(41, LOW);  //
  
  pinMode(4, OUTPUT);     // Anzeige-LED              <--- LED- Ausgang
  digitalWrite(4, HIGH);  //
   
  Serial.begin(230400);   // Hoehere Baud-Rate, dass kein Timeout auftritt

  // Zur Kommunikation mit Chipkarte (9600 Baud, Even parity, 8 Datenbits, 2 Stoppbits)
  Serial1.begin(9600, SERIAL_8E2);
}

void loop() {
  UBlockParser *uBlockParser;
  ICLParser *iclParser;

  digitalWrite(4, LOW);  
  
  while(!(Serial1.available() > 0) && digitalRead(37) != LOW){
    // Hier passiert nix, kein Reset, keine Daten. 
  }
  
  if(digitalRead(37) == LOW){
    // Die Chipkarte soll geresettet werden
    UBlockParser::zaehlerReset();
    BefehlsParser::gesperrt = true;
    waitToResetCard();    
  } else {
  }

  // Datenlaenge ohne Nutzbare Daten beträgt 4: Adresse, Sfeld, Dlen, Checksumme (XOR)
  // Die Datenlaenge wird einfach draufaddiert
  byte dlen = 4;
  boolean timeout = false;
  for(int i = 0; i < dlen; i++){
    timeout = waitWithTimeout(20);
    uBlock[i] = readChar();

    if(i == 2){
      dlen = dlen + uBlock[i];
    }
  }

  if(!timeout){
    digitalWrite(4, HIGH); 
    //49 9 0 56
    uBlockParser = new UBlockParser(&uBlock[0], dlen);
    
    byte *infFeld = uBlockParser->getData();
    byte infFeldLen = uBlockParser->getDataLength();
  
    for(byte i = 0; i < infFeldLen + 4; i++){
      Serial.print((String) uBlock[i] + ", ");
    }
    Serial.println();

    if(infFeldLen != 0){
      iclParser = new ICLParser(infFeld, infFeldLen);
      byte *apdu = iclParser->getData();
      byte apduLen = iclParser->getDataLength();
    
      Command *c = BefehlsParser::decodeCommand(apdu, apduLen);
      Serial.println(c->getCommandInfo());
      Command *r = BefehlsParser::getResponse(c);
      BefehlsParser::encodeCommand(r, uBlock);
    
      apduLen = BefehlsParser::getCommandLen();
      byte iclLen = iclParser->encodeData(&uBlock[0], apduLen, &temp_array[0]);
      byte uBlockLen = uBlockParser->encodeData(&uBlock[0], iclLen, &temp_array[0]);

      sendBlock(&uBlock[0], uBlockLen);
    
      delete(c);
      delete(r);
    } else {
      // Wahrscheinlich REJ- Kommando oder RES der Zaehler
      Serial.println("Leeres Kommando oder verworfene Daten. ");
      byte uBlockLen = uBlockParser->encodeData(&uBlock[0], 0, &temp_array[0]);
      sendBlock(&uBlock[0], uBlockLen);
    }

  Serial.println("---------");
  } else {
    // Fehler, Telefon sollte den Block nach einem Timeout wiederholen
    Serial.println("Timeout");
  }
  
  delete(uBlockParser);
  delete(iclParser);
}



boolean waitWithTimeout(int timeout){
  int timeoutCounter = 0;
  
  while(Serial1.available() == 0){
    delay(1);
    timeoutCounter++;
    if(timeoutCounter >= timeout){
      return true;
    }
  }
  return false;
}

byte readChar(){
  return Serial1.read();
}

void waitToResetCard(){
  Serial.println("Karte wird geresettet");
  while(digitalRead(37) == LOW){
  }
  Serial.println("Reset beendet");
  Serial.println("Bereit, um Daten zu senden. ");

  // Sende ATR:
  Serial1.write(atr, ATRLEN);

  Serial.println("ATR gesendet");
  
  for(int i = 0; i < ATRLEN; i++){
    boolean timeout = waitWithTimeout(20);
    if(!timeout){
      Serial1.read();
    } else {
      Serial.println("Feedback timeout");
    }
  }
}

void sendBlock(byte *block, byte dlen){
  Serial1.write(block, dlen);
  for(int i = 0; i < dlen; i++){
    waitWithTimeout(100);
    Serial.print((String) readChar() + ", ");
  }
  Serial.println();
}
