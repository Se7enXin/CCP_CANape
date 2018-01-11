/*******************************************************************************

  File Name   : boot_can.c
  Date        : 14.08.2001
  Version     : 1.0
  Desciption  : free CAN driver for CCP without using interrupts

*******************************************************************************/


// -----------------------------------------------------------------------------
// INCLUDE FILES
// -----------------------------------------------------------------------------
//#include <iod60.h>               // standard HC12 IO
#include "ccppar.h"              // CPP config file

#include "boot_can.h"            // CAN driver
#include <MC9S12XEP100.h>
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// DEFINES
// -----------------------------------------------------------------------------

#define CAN_BTR0         0x00C3         // CAN-Bus-Timing
#define CAN_BTR1         0x003A         // 250 KBaud

#define SYNCH            0x0010         // SYNCH-Bit
#define RXF_new              0x0001         // receive buffer
#define TXE0_new             0x0001         // transmit buffer 0

#define REG_BLOCK_ADR    0x0000         // address of register block
#define REG_BLOCK_OFFSET 0x0100         // offset to CAN register

//#define msCAN12 (*((t_msCAN12*)(REG_BLOCK_ADR + REG_BLOCK_OFFSET)))
// -----------------------------------------------------------------------------




// -----------------------------------------------------------------------------
// VARIABLES and Structs
// -----------------------------------------------------------------------------
unsigned int g_ccp_dto_id;              // global DTO-ID
unsigned int g_ccp_cro_id;              // global CRO-ID

typedef volatile struct
{
   WORD  Id;              // ID
   BYTE  IdEx1;           // Extended ID bytes; not used
   BYTE  IdEx2;
   BYTE  DataFld[8];      // Data 0 .. 7
   BYTE  DLC;             // Data length reg.:  X X X X DLC3 DLC2 DLC1 DLC0
   BYTE  PRIO;            // TxBuf priority reg.
   BYTE  unused[2];
} t_MsgObject;


/*
typedef volatile struct
{
  BYTE      CMCR0;        // 0 0 CSWAI SYNCH TLNKEN SLPAK SLPRQ SFTRES
  BYTE      CMCR1;        // 0 0 0 0 0 LOOPB WUPM CLKSRC
  BYTE      CBTR0;        // SJW1 SJW0 BRP5 ... BRP0
  BYTE      CBTR1;        // SAMP TSEG22 TSEG21 TSEG20 TSEG13 ... TSEG10
  BYTE      CRFLG;        // WUPIF RWRNIF TWRNIF RERRIF TERRIF BOFFIF OVRIF RXF
  BYTE      CRIER;        // WUPIE RWRNIE TWRNIE RERRIE TERRIE BOFFIE OVRIE RXE
  BYTE      CTFLG;        // 0 ABTAK2 ABTAK1 ABTAK0 0 TXE2 TXE1 TXE0
  BYTE      CTCR;         // 0 ABTRQ2 ABTRQ1 ABTRQ0 0 TXEIE2 TXEIE1 TXEIE0
  BYTE      CIDAC;        // 0 0 IDAM1 IDAM0 0 0 IDHIT1 IDHIT0
  BYTE      reserved1[5];
  BYTE      CRXERR;       // RXERR7 ... RXERR0
  BYTE      CTXERR;       // TXERR7 ... TXERR0
  BYTE      CIDAR0;       // Filter Masks 0..3
  BYTE      CIDAR1;
  BYTE      CIDAR2;
  BYTE      CIDAR3;
  BYTE      CIDMR0;
  BYTE      CIDMR1;
  BYTE      CIDMR2;
  BYTE      CIDMR3;
  BYTE      CIDAR4;       // Filter Masks 4..7
  BYTE      CIDAR5;
  BYTE      CIDAR6;
  BYTE      CIDAR7;
  BYTE      CIDMR4;
  BYTE      CIDMR5;
  BYTE      CIDMR6;
  BYTE      CIDMR7;
  BYTE      reserved2[29];
  BYTE      PCTLCAN;      // 0 0 0 0 0 0 PUECAN
  BYTE      PORTCAN;      // PCAN7 ... PCAN0
  BYTE      DDRCAN;       // DDRCAN7 ... DDRCAN0
  t_MsgObject   RxBuf;    // Rx-Msg Object
  t_MsgObject   TxBuf[3]; // Tx-Msg Objects
} t_msCAN12;  */
/* STOPSINGLE_OF_MULTIPLE */

// -----------------------------------------------------------------------------



// -----------------------------------------------------------------------------
// CAN INITIALISATION
// -----------------------------------------------------------------------------
void ccpBootInit (int cro_id, int dto_id)
{
  g_ccp_dto_id = dto_id;       // handover IDs (CRO, DTO)
  g_ccp_cro_id = cro_id;  

    CAN0CTL0 = 0x01;          
                                
    while (!(CAN0CTL1&0x01)){};

    CAN0CTL1 = 0x80;         
    
    CAN0BTR0 = 0xC3;          
    
    CAN0BTR1 = 0x3A;	      
                              
  
  CAN0IDAC_IDAM = 0;              // use 2*32bit Acceptance Filters

  CAN0IDAR0 = 0x20;                  // TS_PM_1 & TS_PM2 ID 1st Reg Byte = 0x20 both
  CAN0IDAR1 = 0x00;                  // TS_PM_1 2nd Byte = 0x00;TS_PM_2 2nd Byte = 0x20; Need Mask
  CAN0IDAR2 = 0;                     // Not useful in Standard Frame
  CAN0IDAR3 = 0;                     // Not useful in Standard Frame
  
  CAN0IDMR0 = 0xFF;                  // No Mask, care first 3 bits
  CAN0IDMR1 = 0xFF;                  // Mask All
  CAN0IDMR2 = 0;                     // Not useful in Standard Frame
  CAN0IDMR3 = 0;                     // Not useful in Standard Frame

    CAN0CTL0 = 0x00;            
    while ((CAN0CTL1&0x01) != 0){};
    
  CAN0RFLG_RXF = 1;                // clear receive full flag
  CAN0CTL0_RXFRM = 1;                 // clear Received Frame Flag
 
// -----------------------------------------------------------------------------
} /* ccpBootInit */




// -----------------------------------------------------------------------------
// CAN TRANSMIT (Data Frame)
// -----------------------------------------------------------------------------
int ccpBootTransmitCrmPossible( void ) {

  return ((CAN0TFLG_TXE0) == 1);        // return 1 if so
}
// -----------------------------------------------------------------------------
void ccpBootTransmitCrm (unsigned char *msg)
{

  CAN0TBSEL = 0;                  // clear "Tx buffer select Reg" first
  CAN0TBSEL = 0x1;                // select Tx0 buffer

    CAN0TXIDR0=((g_ccp_dto_id<<5)>>8);
    CAN0TXIDR1=(g_ccp_dto_id<<5);
    
    CAN0TXDLR=8;
    CAN0TXDSR0=*msg++;
    CAN0TXDSR1=*msg++;
    CAN0TXDSR2=*msg++;
    CAN0TXDSR3=*msg++;
    CAN0TXDSR4=*msg++;
    CAN0TXDSR5=*msg++;
    CAN0TXDSR6=*msg++;
    CAN0TXDSR7=*msg++;
    
    CAN0TFLG = 0x01;

} /* ccpBootTransmitCrm */
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// CAN RECEIVE (Data Frame)
// -----------------------------------------------------------------------------
int ccpBootReceiveCro (unsigned char *msg)
{
  WORD id;

  if (CAN0RFLG_RXF == 0) return 0;    // check if receive buffer is full

  id = CAN0RXIDR0;
  id = (id<<8) + CAN0RXIDR1;
  id = id >> 5;

  *msg++ = CAN0RXDSR0;           // store received message in buffer
  *msg++ = CAN0RXDSR1;
  *msg++ = CAN0RXDSR2;
  *msg++ = CAN0RXDSR3;
  *msg++ = CAN0RXDSR4;
  *msg++ = CAN0RXDSR5;
  *msg++ = CAN0RXDSR6;
  *msg++ = CAN0RXDSR7;


  CAN0RFLG_RXF = 1;// clear receive full flag
                                 
  CAN0CTL0_RXFRM = 1; // clear Received Frame Flag

  if (id == g_ccp_cro_id) return 1;            // if correctly received, return 1

  return 0;

} /* ccpBootReceiveCro */
// -----------------------------------------------------------------------------
