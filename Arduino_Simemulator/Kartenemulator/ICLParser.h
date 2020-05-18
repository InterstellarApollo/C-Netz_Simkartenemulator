#include "Arduino.h"

class ICLParser{
  public:
    ICLParser(byte *data, byte blen);
    byte *getData();
    byte getDataLength();

    byte encodeData(byte *data, byte dataLen, byte *tempArray);
  private: 
    byte *embedded_data;
    byte datenlaenge = 0;
    
    static boolean master_request;
    static boolean chaining;
    static boolean online;
    byte icb1;
};
