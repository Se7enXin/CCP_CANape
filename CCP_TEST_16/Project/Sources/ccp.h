/*****************************************************************************
| Project Name:   C C P - Driver
|    File Name:   CCP.H
|
|  Description: 
|   CCP driver example.
|   CANape CAN Calibration Tool.
|
|-----------------------------------------------------------------------------
|               C O P Y R I G H T
|-----------------------------------------------------------------------------
| Copyright (c)  2001-2003 by Vector Informatik GmbH.     All rights reserved.
|-----------------------------------------------------------------------------
|               A U T H O R   I D E N T I T Y
|-----------------------------------------------------------------------------
| Initials     Name                      Company
| --------     ---------------------     -------------------------------------
| Bus          Sabine Bücherl            Vector Informatik GmbH
| Ds           Sven Deckardt             Vector Informatik GmbH
| Hp           Armin Happel              Vector Informatik GmbH
| Tri          Frank Triem               Vector Informatik GmbH
| Za           Rainer Zaiser             Vector Informatik GmbH
|-----------------------------------------------------------------------------
|               R E V I S I O N   H I S T O R Y
|-----------------------------------------------------------------------------
|  Date       Version  Author  Description
| ----------  -------  ------  -----------------------------------------------
| 2000-24-09  1.29.00  Za      - New define CCP_CHECKSUM_BLOCKSIZE
| 2000-29-11  1.30.00  Za      - #ifndef CCP_EXTERNAL_STATION_ID
| 2001-08-02  1.31.00  Za      - new define CCP_DAQ_BASE_ADDR
|                              - new function ccpGetDaqPointer
| 2001-30-05  1.32.00  Za      - Reserved word "data" in KEIL Compiler for C5x5
|                              - Prefix CCP_ for all #defines
| 2001-14-09  1.33.00  Za      - #define CCP_ODT_ENTRY_SIZE
|                              - #define CCP_INTEL,CCP_MOTOROLA
| 2001-28-10  1.34.00  Za      - ccpSend return value removed
|                              - Transmission error handling should be done by the user
| 2002-08-04  1.35.00  Za      - #define CCP_CPUTYPE_32BIT
|                              - Max checksum block size is DWORD on 32 bit CPUs
| 2002-02-06  1.36.00  Za      - #undef CCP_DAQ for drivers without DAQ fixed
|                              - double - float conversion for SHORT_UPLOAD, DNLOAD and DAQ
| 2002-17-07  1.37.00  Ds      - Fixed the version nr. because the version was in 
|                                the comment 1.36 but 135 was define.
|                              - Set #define CCP_DRIVER_VERSION to 137
| 2002-14-11  1.37.01  Hp      - define CCP_MAX_DAQ only if CCP_DAQ is defined
| 2002-27-11  1.37.02  Ds      - delete the query of extended id
| 2003-05-28  1.37.02  Bus     - added V_MEMROM0
| 2003-08-11  1.37.03  Tri     - implemented P_MEM_ROM and P_MEM_RAM to support M16C Mitsubishi.
| 2003-10-14  1.38.00  Tri     - version skipped due to special version for TMS320
| 2003-10-14  1.39.00  Tri     - version skipped due to special version for TMS320
| 2003-10-14  1.40.00  Tri     - merge of versions: 1.37.03, 1.37.02
| 2003-10-16  1.41.00  Ds      - minor bugfix set ROM to CCP_ROM  
| 2003-10-16  1.41.01  Ds      - change the position of CCP_ROM  
| 2003-10-21  1.42.00  Tri     - change the position of CCP_ROM
|***************************************************************************/


#ifndef __CCP_H_
#define __CCP_H_

#include "ccppar.h"

#ifndef C_DISABLE_CCP

/* Declare CCP-protocol version */
#define CCP_VERSION_MAJOR   0x02
#define CCP_VERSION_MINOR   0x01


/* Declare module implementation version number */
#define CCP_DRIVER_VERSION 142
#define CCP_DRIVER_BUGFIX_VERSION 0


/* Basic */
#define CC_CONNECT           0x01
#define CC_SET_MTA           0x02
#define CC_DNLOAD            0x03
#define CC_UPLOAD            0x04
#define CC_TEST              0x05 /* V2.1 */
#define CC_START_STOP        0x06
#define CC_DISCONNECT        0x07
#define CC_START_STOP_ALL    0x08 /* V2.1 */
#define CC_SHORT_UPLOAD      0x0F
#define CC_GET_DAQ_SIZE      0x14
#define CC_SET_DAQ_PTR       0x15
#define CC_WRITE_DAQ         0x16
#define CC_EXCHANGE_ID       0x17
#define CC_GET_CCP_VERSION   0x1B /* V2.1 */
#define CC_DNLOAD6           0x23

/* Optional */
#define CC_GET_CAL_PAGE      0x09
#define CC_SET_S_STATUS      0x0C
#define CC_GET_S_STATUS      0x0D
#define CC_BUILD_CHKSUM      0x0E
#define CC_CLEAR_MEMORY      0x10
#define CC_SET_CAL_PAGE      0x11
#define CC_GET_SEED          0x12
#define CC_UNLOCK            0x13
#define CC_PROGRAM           0x18
#define CC_MOVE_MEMORY       0x19
#define CC_DIAG_SERVICE      0x20
#define CC_ACTION_SERVICE    0x21
#define CC_PROGRAM6          0x22 

/* Vector extensions */
#define CC_PROGRAM_PREPARE   0x1E   /* Prepare for flash kernel download */
#define CC_PROGRAM_START     0x1F   /* Start flash kernel at MTA[0] */

/* Returncodes */
#define CRC_OK                 0x00

/* C1 */
#define CRC_CMD_BUSY           0x10
#define CRC_DAQ_BUSY           0x11
#define CRC_KEY_REQUEST        0x18
#define CRC_STATUS_REQUEST     0x19

/* C2 */
#define CRC_COLD_START_REQUEST 0x20
#define CRC_CAL_INIT_REQUEST   0x21
#define CRC_DAQ_INIT_REQUEST   0x22
#define CRC_CODE_REQUEST       0x23

/* C3 (Errors) */
#define CRC_CMD_UNKNOWN        0x30
#define CRC_CMD_SYNTAX         0x31
#define CRC_OUT_OF_RANGE       0x32
#define CRC_ACCESS_DENIED      0x33
#define CRC_OVERLOAD           0x34
#define CRC_ACCESS_LOCKED      0x35


/* Session Status */
#define SS_CAL                0x01
#define SS_DAQ                0x02
#define SS_RESUME             0x04
#define SS_TMP_DISCONNECTED   0x10
#define SS_CONNECTED          0x20
#define SS_STORE              0x40
#define SS_RUN                0x80

/* Priviledge Level */
#define PL_CAL 0x01
#define PL_DAQ 0x02
#define PL_PGM 0x40

/* Broadcast Station Address */
#define CCP_BROADCAST_STATION_ADDR 0

/*--------------------------------------------------------------------------*/
/* Datatypes */
/*--------------------------------------------------------------------------*/

/*
The types CCP_BYTE, CCP_WORD and CCP_DWORD should be defined in ccppar.h
*/

#ifndef CCP_BYTE
  #ifdef BYTE
    #define CCP_BYTE BYTE
  #else
    #define CCP_BYTE unsigned char
  #endif
#endif
#ifndef CCP_WORD
  #ifdef WORD
    #define CCP_WORD WORD
  #else
    #define CCP_WORD unsigned short
  #endif
#endif
#ifndef CCP_DWORD
  #ifdef DWORD
    #define CCP_DWORD DWORD
  #else
    #define CCP_DWORD unsigned long
  #endif
#endif
#ifndef CCP_BYTEPTR
  #ifdef BYTEPTR
    #define CCP_BYTEPTR BYTEPTR
  #else
    #define CCP_BYTEPTR unsigned char *
  #endif
#endif
#ifndef CCP_MTABYTEPTR
  #ifdef MTABYTEPTR
    #define CCP_MTABYTEPTR MTABYTEPTR
  #else
    #define CCP_MTABYTEPTR CCP_BYTE CCP_MEM_ROM *
  #endif
#endif
#ifndef CCP_DAQBYTEPTR
  #ifdef DAQBYTEPTR
    #define CCP_DAQBYTEPTR DAQBYTEPTR
  #else
    #define CCP_DAQBYTEPTR CCP_BYTE CCP_MEM_ROM *
  #endif
#endif
#ifndef CCP_RAM
  #ifdef RAM
    #define CCP_RAM RAM
  #else
    #define CCP_RAM
  #endif
#endif
#ifndef CCP_ROM
  #ifdef ROM
    #define CCP_ROM ROM
  #else
    #define CCP_ROM
  #endif
#endif
/* Vector CAN Driver Intel/Motorola */
#ifdef C_CPUTYPE_BIGENDIAN
  #define CCP_MOTOROLA
#endif
#ifdef C_CPUTYPE_LITTLEENDIAN
  #define CCP_INTEL
#endif
#ifndef CCP_INTEL
  #ifndef CCP_MOTOROLA
    #error Please #define CCP_INTEL or CCP_MOTOROLA as the target system byte order
  #endif
#endif
/* pointer to ROM/RAM */
#if defined (CCP_MEM_ROM)
#else
  #if defined (P_MEM_ROM)
    #define CCP_MEM_ROM P_MEM_ROM
  #else
    #define CCP_MEM_ROM
  #endif
#endif
#if defined (CCP_MEM_RAM)
#else
  #if defined (P_MEM_RAM)
    #define CCP_MEM_RAM P_MEM_RAM
  #else
    #define CCP_MEM_RAM
  #endif
#endif
/* Memory qualifier, which always remains in first position. */
/* Can be used e.g. for compiler/linker which don't place constants into the ROM */
#if defined (CCP_MEMROM0)
#else
  #if defined (V_MEMROM0)
    #define CCP_MEMROM0 V_MEMROM0
  #else
    #define CCP_MEMROM0
  #endif
#endif

/*--------------------------------------------------------------------------*/
/* DAQ Lists, Type Definition */
/*--------------------------------------------------------------------------*/

/* Note:
   - Adressextensions are not used in this example
   - Object size > 1 are not used in this example and must be handled in
     the calibration system
*/

/* The maximum number of ODTs is restricted
   PID=0xFE,0xFF is reserved
*/
#ifdef CCP_DAQ
#ifndef CCP_SEND_QUEUE_OVERRUN_INDICATION
  #if CCP_MAX_DAQ*CCP_MAX_ODT>254
  #error Too many ODTs, maximum is CCP_MAX_DAQ*CCP_MAX_ODT = 254
  #endif
#else
  #if CCP_MAX_DAQ*CCP_MAX_ODT>126
  #error Too many ODTs, maximum is CCP_MAX_DAQ*CCP_MAX_ODT = 126
  #endif
#endif
#endif

/* CCP_DAQBYTEPTR and CCP_MTABYTEPTR may be defined different to CCP_BYTEPTR to save memory
   Example:
     #define CCP_BYTEPTR     unsigned char *
     #define CCP_MTABYTEPTR  huge unsigned char *
     #define CCP_DAQBYTEPTR  unsigned char *
*/
#ifndef CCP_DAQBYTEPTR
  #ifdef CCP_DAQ_BASE_ADDR         /* CCP_DAQBYTEPTR required, if CCP_DAQ_BASE_ADDR is defined */
    #error Please define CCP_DAQBYTEPTR, which is the type of ODT entries
  #else
    #define CCP_DAQBYTEPTR CCP_BYTEPTR
  #endif
#endif
#ifndef CCP_MTABYTEPTR
  #define CCP_MTABYTEPTR CCP_BYTEPTR
#endif


#ifdef CCP_DAQ

/* ODT entry */
typedef struct ccpOdtEntry {
  CCP_DAQBYTEPTR ptr;
  #ifdef CCP_ODT_ENTRY_SIZE
    CCP_BYTE       siz;
  #endif
} ccpOdtEntry_t;

/* ODT */
typedef ccpOdtEntry_t ccpODT_t[7];

/* DAQ list */
#define DAQ_FLAG_START    0x01
#define DAQ_FLAG_SEND     0x02
#define DAQ_FLAG_PREPARED 0x04
#define DAQ_FLAG_OVERRUN  0x80
typedef struct ccpDaqList {
  ccpODT_t    odt[CCP_MAX_ODT];
  CCP_WORD        prescaler;
  CCP_WORD        cycle;
  CCP_BYTE        eventChannel;
  CCP_BYTE        last;
  CCP_BYTE        flags;
} ccpDaqList_t;

#else
  #undef CCP_SEND_QUEUE
  #undef CCP_SEND_SINGLE
#endif

/*--------------------------------------------------------------------------*/
/* Transmit Queue, Type Definition */
/*--------------------------------------------------------------------------*/
#ifdef CCP_SEND_QUEUE

/* Checks */

#ifndef CCP_SEND_QUEUE_SIZE
  #define CCP_SEND_QUEUE_SIZE (CCP_MAX_ODT*CCP_MAX_DAQ)
#endif

#ifdef CCP_SEND_SINGLE
  #error Do not use CCP_SEND_SINGLE together with CCP_SEND_QUEUE
#endif

#if CCP_MAX_ODT > CCP_SEND_QUEUE_SIZE
  #error CCP_SEND_QUEUE_SIZE is too small
#endif


typedef struct {
  CCP_BYTE b[8];
} ccpMsg_t;

typedef struct {
  CCP_BYTE     len;
  CCP_BYTE     rp;
  ccpMsg_t msg[CCP_SEND_QUEUE_SIZE];
} ccpQueue_t;

#endif

/*--------------------------------------------------------------------------*/
/* CCP Driver Variables, Type Definition */
/*--------------------------------------------------------------------------*/

#define CCP_MAX_MTA 2
#define CCP_INTERNAL_MTA (CCP_MAX_MTA-1)

/* Return values for ccpWriteMTA and ccpCheckWriteEEPROM */
#define CCP_WRITE_DENIED  0
#define CCP_WRITE_OK      1
#define CCP_WRITE_PENDING 2
#define CCP_WRITE_ERROR   3

/* Bitmasks for ccp.SendStatus */
#define CCP_CRM_REQUEST  0x01
#define CCP_DTM_REQUEST  0x02
#define CCP_USR_REQUEST  0x04
#define CCP_CMD_PENDING  0x08
#define CCP_CRM_PENDING  0x10
#define CCP_DTM_PENDING  0x20
#define CCP_USR_PENDING  0x40
#define CCP_TX_PENDING   0x80
#define CCP_SEND_PENDING (CCP_DTM_PENDING|CCP_CRM_PENDING|CCP_USR_PENDING)

struct ccp {

  CCP_BYTE Crm[8];                           /* CRM Command Return Message buffer */

  CCP_BYTE SessionStatus;
  CCP_BYTE SendStatus;

  CCP_MTABYTEPTR MTA[CCP_MAX_MTA];           /* Memory Transfer Address */

  #ifdef CCP_DAQ

    #ifdef CCP_SEND_QUEUE
      ccpQueue_t Queue;
    #else
      CCP_BYTE Dtm[8];                       /* DTM Data Transmission Message buffer */
    #endif

    #ifdef CCP_SEND_SINGLE
      CCP_BYTE CurrentDaq;
      CCP_BYTE CurrentOdt;
    #endif


    ccpOdtEntry_t *DaqListPtr;               /* Pointer for SET_DAQ_PTR, make it near to save RAM  */

    ccpDaqList_t   DaqList[CCP_MAX_DAQ];     /* DAQ list */

  #endif

  CCP_BYTE UserSessionStatus;                /* Used for GET/SET_SESSION_STATUS */

  #ifdef CCP_SEED_KEY
    CCP_BYTE ProtectionStatus;               /* Resource Protection Status */
  #endif

  #ifdef CCP_CHECKSUM
    #if defined( CCP_CPUTYPE_32BIT )
      CCP_DWORD CheckSumSize;                /* Counter for checksum calculation */
    #else
      CCP_WORD CheckSumSize;                 /* Counter for checksum calculation */
    #endif
  #endif

};

extern CCP_RAM struct ccp ccp;


/*--------------------------------------------------------------------------*/
/* Prototypes */
/*--------------------------------------------------------------------------*/

/* Functions from ccp.c */
/*----------------------*/

/* Initialize */
extern void ccpInit( void );

/* DAQ processor */
extern void ccpDaq( CCP_BYTE eventChannel );

/* Command processor */
extern void ccpCommand( CCP_BYTEPTR msg );

/* Transmit Notification */
/* Returns 0 when the CCP driver is idle */
extern CCP_BYTE ccpSendCallBack( void );

/* Send a pending CRM */
void ccpSendCrm( void );

/* Background Loop */
/* Return 1 (TRUE) when still pending */
/* Used only if Checksum Calculation or EEPROM Programming is required */
extern CCP_BYTE ccpBackground( void );

/* Functions provided externally */
/*-------------------------------*/

/* Transmit a message */
/* Returns 1 (TRUE) when successfull */
extern void ccpSend( CCP_BYTEPTR msg );

/* Background calculation */
#ifndef ccpUserBackground
  extern void ccpUserBackground( void );
#endif

/* Generate a pointer from addr_ext and addr */
#if !defined(ccpGetPointer) || defined(__BOOT_H__) /* Not defined as macro */
  extern CCP_MTABYTEPTR ccpGetPointer( CCP_BYTE addr_ext, CCP_DWORD addr );
#endif

#ifdef CCP_DAQ_BASE_ADDR
  extern CCP_DAQBYTEPTR ccpGetDaqPointer( CCP_BYTE addr_ext, CCP_DWORD addr );
#endif

#ifdef CCP_STANDARD
  extern void ccpGetMTA0( CCP_BYTE *e, CCP_DWORD *a);
#endif

/* Check addresses for valid write access */
/* Returns 0 (false) if access denied */
/* Used only, if write protection of memory areas is required */
#ifdef CCP_WRITE_PROTECTION
  #ifndef ccpCheckWriteAccess /* Not defined as macro */
    extern CCP_BYTE ccpCheckWriteAccess( CCP_MTABYTEPTR a, CCP_BYTE size );
  #endif
#endif

/* Flash Kernel Download */
/* Used only of Download of the Flash Kernel is required */  
#ifdef CCP_BOOTLOADER_DOWNLOAD

  extern CCP_BYTE ccpDisableNormalOperation( CCP_MTABYTEPTR a, CCP_WORD size );
  typedef void (*ccpBootLoader_t)(struct ccp *, CCP_BYTEPTR);

#endif

/* Flash Programming */
/* Used only if Flash Programming is required */
#ifdef CCP_PROGRAM

  #define CCP_CALPAGE

  /* If Flash Programming is performed by a special loader, */
  /* the following function will be called on CLEAR_MEMORY command */
  #ifdef CCP_BOOTLOADER

    extern void ccpBootLoaderStartup( struct ccp *ccp, CCP_BYTEPTR com );

  #else

    extern void ccpFlashClear( CCP_MTABYTEPTR a, CCP_DWORD size );
    extern CCP_BYTE ccpFlashProgramm( CCP_BYTEPTR data, CCP_MTABYTEPTR a, CCP_BYTE size );
    /* Returns:
       CCP_WRITE_OK      - FLASH written
       CCP_WRITE_PENDING - FLASH write in progress, call ccpSendCrm when done
    */

  #endif

#endif


/* RAM/ROM Switching */
#ifdef CCP_CALPAGE
  extern void ccpInitCalPage( void );
  extern CCP_DWORD ccpGetCalPage( void );
  extern void ccpSetCalPage( CCP_DWORD a );
#endif


/* Seed&Key*/
#ifdef CCP_SEED_KEY
  extern CCP_DWORD ccpGetSeed( CCP_BYTE resourceMask );
  extern CCP_BYTE ccpUnlock( CCP_BYTE *key );
#endif


/* EEPROM Programing */
#ifdef CCP_WRITE_EEPROM
  /* Return values for ccpCheckWriteEEPROM:
     CCP_WRITE_OK     - EEPROM written
     CCP_WRITE_DENIED - This is not EEPROM
     CCP_WRITE_PENDING - EEPROM write in progress, call ccpSendCrm when done
  */
  CCP_BYTE ccpCheckWriteEEPROM( CCP_MTABYTEPTR addr, CCP_BYTE size, CCP_BYTEPTR data );
#endif
#ifdef CCP_READ_EEPROM
  /* Return values for ccpCheckReadEEPROM:
     0 (FALSE)    - This is not EEPROM
     1 (TRUE)     - EEPROM read
  */
  CCP_BYTE ccpCheckReadEEPROM( CCP_MTABYTEPTR addr, CCP_BYTE size, CCP_BYTEPTR data );
#endif


/*--------------------------------------------------------------------------*/
/* Special Features */
/* Please contact Vector for details */
/*--------------------------------------------------------------------------*/

/* ECU Timestamping */
#ifdef CCP_TIMESTAMPING
  #ifndef ccpGetTimestamp /* Not defined as macro */
    CCP_WORD ccpGetTimestamp( void );
  #endif
  #ifndef ccpClearTimestamp /* Not defined as macro */
    void ccpClearTimestamp( void );
  #endif
#endif


/*--------------------------------------------------------------------------*/
/* Test */
/*--------------------------------------------------------------------------*/

#ifdef CCP_TESTMODE
  extern unsigned char gDebugLevel;
#endif

#endif

#endif
