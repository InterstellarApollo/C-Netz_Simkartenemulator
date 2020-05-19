#include "arduino.h"
#include "Chipkarte.h"

class UBlockParser{
  public:
    UBlockParser(byte *data, byte blen);
    byte *getData();
    byte getDataLength();

    byte encodeData(byte *data, byte dataLen, byte *tempArray);
    
    static void zaehlerReset();

    boolean iCommand;
    boolean rejCommand;
    boolean resCommand;
    boolean sendRej = false;
  private:
    byte encodeI(byte *data, byte dataLen, byte *tempArray);
    byte encodeREJ(byte *data, byte dataLen, byte *tempArray);
    byte resResponse(byte *data, byte dataLen, byte *tempArray);
    byte rejResponse(byte *data, byte dataLen, byte *tempArray);

    static byte embedded_data_backup[255];
    static byte datenlaenge_backup;
  
    byte *embedded_data;
    byte datenlaenge;
    boolean kontrollsumme_ok;
    boolean empfaengerIstCC;

    byte sender;
    byte empfaenger;
    byte steuerfeld;
    byte kontrollsumme;
    
    static byte sendefolgezaehler;
    static byte empfangsfolgezaehler;
};
