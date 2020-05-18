#ifndef Command_h
#define Command_h
#include "arduino.h"

class Command{
  public:
    Command(boolean pI, byte pCla, byte pIns, byte *pData, byte pDlen);
    Command();
    String getCommandInfo();

    void setClass_CCRC(byte b);
    byte getClass_CCRC();

    void setInstruction(byte b);
    byte getInstruction();

    void setData(byte *b, byte pDlen);
    byte *getData();

    byte getDataLen();
  private:
    byte data[255];
    byte dlen;
    
    boolean i;
    byte cla;
    byte ins;
};

#endif
