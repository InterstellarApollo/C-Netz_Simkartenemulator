#ifndef ICLParser_h
#define ICLParser_h
#include "ICLParser.h"

ICLParser::ICLParser(byte *data, byte blen){
  // Datenlänge der Nutzdaten ist die gesamtlänge - 1 für das icb1. icb2 & chaining wird noch nicht unterstützt. 
  // Sollte die Datenlaenge 1 sein, wird wahrscheinlich eine Funktion verlangt, die noch nicht implementiert wurde....
  if(blen > 1){
    // Das icb1 steht direkt an erster Stelle
    icb1 = *(data + 0);
    // Gefolgt von den Daten der höheren Schicht
    embedded_data = (data + 1);
    datenlaenge = blen - 1;
  } else if(datenlaenge == 1){
    Serial.println("ICL- Kommando (nicht unterstuetzt...)");
  }else {
    embedded_data = NULL;
    icb1 = 0b00000000;
    datenlaenge = 0;
  }
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
