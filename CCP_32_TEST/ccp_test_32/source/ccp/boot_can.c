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



#include "ccp_includes.h"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// DEFINES
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------




// -----------------------------------------------------------------------------
// VARIABLES and Structs
// -----------------------------------------------------------------------------
unsigned int g_ccp_dto_id;              // global DTO-ID
unsigned int g_ccp_cro_id;              // global CRO-ID


// -----------------------------------------------------------------------------
// CAN INITIALISATION
// -----------------------------------------------------------------------------
void ccpBootInit (int cro_id, int dto_id)
{
  g_ccp_dto_id = dto_id;       // handover IDs (CRO, DTO)
  g_ccp_cro_id = cro_id;

  InitCAN(1,250000,0,0,0,0);
   
// -----------------------------------------------------------------------------
} /* ccpBootInit */


// -----------------------------------------------------------------------------
// CAN TRANSMIT (Data Frame)
// -----------------------------------------------------------------------------
int ccpBootTransmitCrmPossible( void ) 
{
	return CanTransmitPossible();
 // return ((CAN0TFLG_TXE0) == 1);        // return 1 if so
}
// -----------------------------------------------------------------------------
void ccpBootTransmitCrm (unsigned char *msg)
{
		
	SendCANMsg(1,0,1,g_ccp_dto_id,8,msg);
		
} /* ccpBootTransmitCrm */
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// CAN RECEIVE (Data Frame)
// -----------------------------------------------------------------------------
int ccpBootReceiveCro (unsigned char *msg)
{
	UINT_8 ext=0;
	UINT_8 dataframe=1;
	UINT_32 canid=100;
	UINT_8 datalen=8;
	
    if(ERROR_OK == GetCANMsg(1,&ext,&dataframe,&canid,&datalen,msg))
    {
		//return 1;    
    	if (canid == g_ccp_cro_id) return 1;  // if correctly received, return 1	
    }
  
 //	if (canid == g_ccp_cro_id) return 1;  // if correctly received, return 1
            
  return 0;

} /* ccpBootReceiveCro */
// -----------------------------------------------------------------------------
