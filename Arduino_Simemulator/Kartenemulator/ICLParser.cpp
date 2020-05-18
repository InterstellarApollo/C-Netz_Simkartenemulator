#ifndef ICLParser_h
#define ICLParser_h
#include "ICLParser.h"

ICLParser::ICLParser(byte *data, byte blen){
  // Das icb1 steht direkt an erster stelle
  icb1 = *(data);
  // Gefolgt von den Daten der höheren Schicht
  embedded_data = (data + 1);
  // Datenlänge der Nutzdaten ist die gesamtlänge - 1 für das icb1. icb2 & chaining wird noch nicht unterstützt. 
  datenlaenge = blen - 1;
}

byte *ICLParser::getData(){
  return embedded_data;
}

byte ICLParser::getDataLength(){
  return datenlaenge;
}

byte ICLParser::encodeData(byte *data, byte dataLen, byte *tempArray){
  // icb1 wird vorerst mit 0 festgelegt. 
  byte icb1 = 0;
  for(byte i = 0; i < dataLen; i++){
    *(tempArray + i) = *(data + i);
  }
  *(data + 0) = icb1;
  for(byte i = 0; i < dataLen; i++){
    *(data + i + 1) = *(tempArray + i);
  }
  return dataLen + 1;
}

#endif
