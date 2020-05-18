#include "Command.h"
#include "Chipkarte.h"

Command::Command(boolean pI, byte pCla, byte pIns, byte *pData, byte pDlen){
  i = pI;
  cla = pCla;
  ins = pIns;
  for(byte i = 0; i < pDlen; i++){
    data[i] = *(pData + i);
  }
  dlen = pDlen;
}

Command::Command(){
  i = false;
  cla = 0;
  ins = 0;
  dlen = 0;
}

String Command::getCommandInfo(){
  // Soll durch die Debug- Nachrichten vom Commandparser ersetzt werden
  return "";
}

void Command::setData(byte *b, byte pDlen){
  for(byte i = 0; i < pDlen; i++){
    data[i] = *(b + i);
  }
  dlen = pDlen;
}

byte *Command::getData(){
  return &data[0];
} 

byte Command::getDataLen(){
  return dlen;
}

void Command::setClass_CCRC(byte b){
  cla = b;
}
byte Command::getClass_CCRC(){
  return cla;
}

void Command::setInstruction(byte b){
  ins = b;
}
byte Command::getInstruction(){
  return ins;
}
