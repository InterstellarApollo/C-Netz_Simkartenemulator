#ifndef UBlockParser_h
#define UBlockParser_h
#include "UBlockParser.h"

byte UBlockParser::empfangsfolgezaehler = 0;
byte UBlockParser::sendefolgezaehler = 0;

static byte UBlockParser::embedded_data_backup[255];
static byte UBlockParser::datenlaenge_backup;


UBlockParser::UBlockParser(byte *data, byte blen){
  byte berechnete_kontrollsumme = 0;
  byte adressByte;
  for(byte i = 0; i < blen - 1; i++){
    berechnete_kontrollsumme = berechnete_kontrollsumme ^ *(data + i);
  }

  adressByte    = *(data + 0);
  steuerfeld    = *(data + 1);
  datenlaenge   = *(data + 2);
  embedded_data =  (data + 3);
  
  kontrollsumme = *(data + blen - 1);
  if(berechnete_kontrollsumme == kontrollsumme){
    kontrollsumme_ok = true;
  } else {
    kontrollsumme_ok = false;
    sendRej = true;
    Serial.println("CHKSUM - Error: Gesendet: " + (String) kontrollsumme + " Berechnet: " + (String) berechnete_kontrollsumme);
  }

  sender = (adressByte & 0b11110000) >> 4;
  empfaenger = adressByte & 0b00001111;
  
  Serial.println("Eingehender Block von: " + (String) sender + " nach: " + (String) empfaenger);
  if(empfaenger == CCADRESS){
    Serial.println("Block empfangen. ");
    empfaengerIstCC = true;

    if(((steuerfeld & 0b00010001) == 0b00000000)){
      UBlockParser::empfangsfolgezaehler++;
      
      // Der Empfangsfolgezaehler zaehlt von 0-7
      empfangsfolgezaehler = empfangsfolgezaehler & 0b0111;
      iCommand = true;
    } else if (((steuerfeld & 0b00001001) == 0b00001001)){
      Serial.println("REJ- Befehl! ");
      if(sendefolgezaehler == steuerfeld >> 5){
        Serial.println("Zaehler stimmen ueberein. ");
      } else {        
        Serial.println("Zaehler werden uebernommen: " + (String) sendefolgezaehler + " --> " + (String) (steuerfeld >> 5));
        sendefolgezaehler = steuerfeld >> 5;
      }
      rejCommand = true;
    } else if((steuerfeld == 0b11101111)){
      Serial.println("RES- Befehl! ");
      resCommand = true;
    } else {
      Serial.println("Ungültiges Steuerfeld, REJ");
      sendRej = true;
      //UBlockParser::empfangsfolgezaehler++;
      // Daten werden vernichtet, Block ist ungueltig
      datenlaenge = 0;
    }
  } else {
    empfaengerIstCC = true;
  }
}

byte *UBlockParser::getData(){
  return (embedded_data);
}

byte UBlockParser::getDataLength(){
  return datenlaenge;
}

static void UBlockParser::zaehlerReset(){
  UBlockParser::empfangsfolgezaehler = 0;
  UBlockParser::sendefolgezaehler = 0;
}

byte UBlockParser::encodeData(byte *data, byte dataLen, byte *tempArray){
  if(sendRej){
    return encodeREJ(data, dataLen, tempArray);
  } else if(iCommand){
    return encodeI(data, dataLen, tempArray);
  } else if(rejCommand){
    return rejResponse(data, dataLen, tempArray);
  } else if(resCommand){
    return resResponse(data, dataLen, tempArray);
  }
}


byte UBlockParser::encodeREJ(byte *data, byte dataLen, byte *tempArray){
  byte csum = 0;
  
  byte destAdress = 3;
  byte sourceAdress = CCADRESS;
  
  byte adressFeld = destAdress + (sourceAdress << 4);
  byte sfeld = (sendefolgezaehler << 5) + 0b000001001;
  byte dlen = 0;
  
  *(data + 0) = adressFeld;
  *(data + 1) = sfeld;
  *(data + 2) = dlen;
  
  for(byte i = 0; i < dataLen + 3; i++){
    csum = csum ^ *(data + i);
  }
  
  *(data + 3 + dataLen) = csum;
  
  //UBlockParser::sendefolgezaehler++;
  Serial.println("REJ. ");
  return 4;
}

byte UBlockParser::resResponse(byte *data, byte dataLen, byte *tempArray){
  return 0;
}

byte UBlockParser::rejResponse(byte *data, byte dataLen, byte *tempArray){
  for(byte i = 0; i < datenlaenge_backup; i++){
    *(data + i) = embedded_data_backup[i];
  }
  Serial.println();
  Serial.println("REJ-response. ");
  
  UBlockParser::sendefolgezaehler++;
  UBlockParser::sendefolgezaehler = UBlockParser::sendefolgezaehler & 0b0111;
  
  return datenlaenge_backup;
}

byte UBlockParser::encodeI(byte *data, byte dataLen, byte *tempArray){
  byte destAdress = sender;
  byte sourceAdress = empfaenger;

  Serial.println();
  Serial.println("Zu sendender Block von: " + (String) sourceAdress + " nach: " + (String) + destAdress );
  
  byte adressfeld = destAdress + (sourceAdress << 4);
  //EVZ um 5 nach links, SVZ um 1 nach links. Sprich: SSS0EEE0
  byte sfeld = (empfangsfolgezaehler << 5) + (sendefolgezaehler << 1);
  
  byte dlen = dataLen;
  byte csum = 0;
  for(byte i = 0; i < dataLen; i++){
    *(tempArray + i) = *(data + i);
  }
  *(data + 0) = adressfeld;
  *(data + 1) = sfeld;
  *(data + 2) = dlen;
  for(byte i = 0; i < dataLen; i++){
    *(data + i + 3) = *(tempArray + i);
  }  

  for(byte i = 0; i < dataLen + 3; i++){
    csum = csum ^ *(data + i);
  }
  
  *(data + 3 + dataLen) = csum;
  
  datenlaenge_backup = dataLen + 4;
  for(byte i = 0; i < datenlaenge_backup; i++){
    embedded_data_backup[i] = *(data + i);
  }
  UBlockParser::sendefolgezaehler++;
  UBlockParser::sendefolgezaehler = UBlockParser::sendefolgezaehler & 0b0111;
  
  return dataLen + 4;
}


#endif
