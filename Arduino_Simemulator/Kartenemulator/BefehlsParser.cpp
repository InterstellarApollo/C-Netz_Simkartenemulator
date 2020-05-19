#ifndef BefehlsParser_h
#define BefehlsParser_cpp
#include "BefehlsParser.h"

static byte BefehlsParser::commandLen = 0;
boolean BefehlsParser::gesperrt = true;

static Command *BefehlsParser::decodeCommand(byte *data, byte blen){
  boolean i = (*(data + 0) & 0b10000000) > 0;
  byte cla = *(data + 0) & 0b01111111;
  byte ins = *(data + 1);
  byte dlen = *(data + 2);
  byte *datenblock = (data + 3);
  Command *c = new Command(i, cla, ins, datenblock, dlen);
  return c;
}

static byte *BefehlsParser::encodeCommand(Command *c, byte byteArray[]){
  byte *s7data = c->getData();
  
  byteArray[0] = c->getClass_CCRC();
  byteArray[1] = c->getInstruction();
  byteArray[2] = c->getDataLen();

  for(byte i = 0; i < c->getDataLen(); i++){
    byteArray[3+i] = *(s7data + i);
  }
  commandLen = 3+c->getDataLen();
}

static byte BefehlsParser::getCommandLen(){
  return commandLen;
}

static Command *BefehlsParser::getResponse(Command *c){
    // Command(boolean pI, byte pCla, byte pIns, byte *pData, byte pDlen);
    Command *response = new Command();

    byte insClass = c->getClass_CCRC();
    byte instruction = c->getInstruction();
    byte *data = c->getData();
    
    // Um zu prüfen, ob ein Befehl die Karte direkt (Hier vom Befehlsparser ausgefuehrt) oder eine darauf befindliche / selektierte Applikation (z.B. C-Netz, in dem Fall dann von C-Netz ausgefuehrt) betrifft. 
    boolean generalCommand = false;
 
    switch(c->getClass_CCRC()){
    case CNTR:
      if(instruction == 0xF1){
        generalCommand = true;
        selectApplication(response, data);
      }
      break;
    case STAT: 
      break;
    case WRTE:
      break;
    case READ:
      break;
    case EXEC:
      if(instruction == 0xF1){
        generalCommand = true;
        checkPin(response, data, c->getDataLen());
      }
      break;
    case AUTO:
      break;
  }

  if(!generalCommand){
    if(Chipkarte::selectedApplication == CNETZ){
      Serial.println("C-Netz Befehl: " + (String) instruction + ", Befehlsklasse: " + (String) insClass);
      CNetz::runInstruction(insClass, instruction, data, response);
    }
  }
  return response;
}

static void BefehlsParser::checkPin(Command *c, byte *data, byte pinlen){
  byte ccrc = 0;
  byte asta = 0;
  byte asts = 0;

  ccrc = 0b10000000;     
  asta = 0b0;           
  asts = 0b0;

  Serial.print("Pin: ");
  
  for(byte i = 0; i < pinlen; i++){
    Serial.write(*(data + i));
  }
  Serial.println();
  
  gesperrt = false;
  
  c->setInstruction(asts<<4 + asta);
  c->setClass_CCRC(ccrc);
}

static void BefehlsParser::selectApplication(Command *c, byte *data){
  byte ccrc = 0;
  byte asta = 0;
  byte asts = 0;
  
  char c1 = *(data + 6);
  char c2 = *(data + 7);
  char c3 = *(data + 8);
  Serial.println("Select application " + (String) c1  + (String) c2 + (String) c3);
        
  if(c1 == '0' && c2 == '0' && c3 == '3'){
    Chipkarte::selectedApplication = CNETZ;
    // Nur CCRC wird ausgewertet
    ccrc = 0b10000100 | gesperrt;     
    asta = 0b0;           
    asts = 0b0;           
    c->setData(0, 0);
  } else if(c1 == '0' && c2 == '0' && c3 == '4'){
    Chipkarte::selectedApplication = RFV;
    // Nur CCRC wird ausgewertet
    ccrc = 0b10000100;     
    asta = 0b0;           
    asts = 0b0;    
  } else {
    // Ident- Bit ist 1 und Error- Bit, da der Befehl nicht bekannt ist. 
    ccrc = 0b11000000;
    Serial.println("APP. nicht erkannt! ");
  }

  c->setInstruction(asts<<4 + asta);
  c->setClass_CCRC(ccrc);
}

#endif
