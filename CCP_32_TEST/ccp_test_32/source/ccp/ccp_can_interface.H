/*******************************************************************************

  File Name   : ccp_can_interface.h
  Date        : 27.07.2001
  Version     : 1.0
  Desciption  : additional functions for CCP


*******************************************************************************/
#ifndef _CCP_CAN_INTERFACE_
#define _CCP_CAN_INTERFACE_


#include "ccppar.h"                             // CPP config file

// -----------------------------------------------------------------------------
// PROTOTYPES
// -----------------------------------------------------------------------------

BYTE ccpSend( BYTEPTR msg );
MTABYTEPTR ccpGetPointer( BYTE addr_ext, DWORD addr );

void ccpUserBackground( void );

void disable_interrupt();                                // I-bit SET
void enable_interrupt();                                 // I-bit CLEAR
// -----------------------------------------------------------------------------


#endif