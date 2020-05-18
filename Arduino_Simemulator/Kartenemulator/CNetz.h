#include <EEPROM.h>

#include "Arduino.h"
#include "Command.h"
#include "Chipkarte.h"

#define RD_EBD  01
#define RD_RUFN 02
#define RD_GEBZ 03

#define WT_RUFN 01

#define EH_GEBZ 01
#define CL_GEBZ 02

#define AUT-1   01

class CNetz{
  public:
    boolean gesperrt;
    static void runInstruction(byte cla, byte ins, byte *data, Command *c);
    
    static void readRufn(Command *c, byte *data);
    static void writeRufn(Command *c, byte *data);

    static void setTBEntry(byte toSet);
    static void clearTBEntry(byte toClear);
    
    static void readEbd(Command *c, byte *data);
};
