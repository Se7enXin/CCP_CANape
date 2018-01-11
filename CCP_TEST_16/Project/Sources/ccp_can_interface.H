/*******************************************************************************

  File Name   : ccp_can_interface.h
  Date        : 27.07.2001
  Version     : 1.0
  Desciption  : additional functions for CCP


*******************************************************************************/

#include "ccppar.h"                             // CPP config file

// -----------------------------------------------------------------------------
// PROTOTYPES
// -----------------------------------------------------------------------------
void irq_dummy(void);                                    // empty ISR

BYTE ccpSend( BYTEPTR msg );
MTABYTEPTR ccpGetPointer( BYTE addr_ext, DWORD addr );

void ccpUserBackground( void );

void disable_interrupt();                                // I-bit SET
void enable_interrupt();                                 // I-bit CLEAR
// -----------------------------------------------------------------------------
