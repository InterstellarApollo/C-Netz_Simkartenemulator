#include "arduino.h"
#include "Command.h"
#include "Chipkarte.h"
#include "CNetz.h"

class BefehlsParser{
  public:
    static boolean gesperrt;
    
    static Command *decodeCommand(byte *data, byte dlen);
    
    static byte *encodeCommand(Command *c, byte *byteArray);
    static byte getCommandLen();
    
    static Command *getResponse(Command *c);
  private:
    static byte commandLen;

    static void selectApplication(Command *c, byte *data);
    static void checkPin(Command *c, byte *data, byte pinlen);
};
