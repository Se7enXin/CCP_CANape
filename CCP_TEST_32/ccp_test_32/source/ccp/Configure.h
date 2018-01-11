#include <hidef.h>            
#include <MC9S12XEP100.h>       

#pragma LINK_INFO DERIVATIVE "MC9S12XEP100"


#define ABS_EC1  0x67fe4016 
#define ABS_TEST 0x67fe4216

#define HCU_EC3  0x67fe0442
#define NO_ERR 0x00
#define ERR_BUFFER_FULL 0x80 

typedef unsigned char byte;
typedef unsigned int word;
typedef unsigned long  dword;
word ABS_EC1data[4];
byte *p,*t;
byte length, index;
byte rxdata[8];
dword RXID;
byte test[4],a=0;
byte errorflag = NO_ERR;
struct
{
  byte ABSActive ;
  byte BrakeState;
  byte BrakePedalPos;
  word RearAxisBrakeMaxForce;
}ABS_EC1_msg;
struct
{
  word MtrRegenerateTorque;
} HCU_EC3_msg;