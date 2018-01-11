/*****************************************************************************
| Project Name:   C C P - Driver
|    File Name:   CCP.C
|
|  Description: 
|   Implements the CCP module .
|   used by the CANape CAN Calibration Tool.
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

/* CCP Definitions and Parameters */
#include "ccp.h"

#ifndef C_DISABLE_CCP


/******************************************************************************/
/* Version check                                                              */
/******************************************************************************/
#if( CCP_DRIVER_VERSION != 142)
 #error "Source and Header file of CCP-Module inconsistent!"
#endif
#if( CCP_DRIVER_BUGFIX_VERSION != 0)
 #error "Source and Header file of CCP-Module inconsistent!"
#endif

#if( CCP_DRIVER_VERSION > 255)
 #error "Version decreased in CCP-Module"
#endif

/*--------------------------------------------------------------------------*/
/* Performance measurements */

#ifndef CCP_PROFILE

  #define SET_PORT_BIT(i)
  #define RST_PORT_BIT(i)

#else

  /*
  t[0] - ccpCommand
  t[1] - ccpCallBack
  t[2] - ccpBackground
  t[3] - ccpDaq
  */
  unsigned int t0[4],t[4];
  #define SET_PORT_BIT(i) t0[i-1]=ccpGetTimestamp();
  #define RST_PORT_BIT(i) t[i-1]=T3-t0[i-1];

#endif


/*--------------------------------------------------------------------------*/
/* Test */

#ifdef CCP_TESTMODE
  #include <stdio.h>
  static void ccpPrintCANapeSettings( void );
  static void ccpPrintDaqList( CCP_BYTE daq );
#endif


/*--------------------------------------------------------------------------*/
/* ROM */
/*--------------------------------------------------------------------------*/

/*
   Identification
   Must be 0 terminated !!

   This string is used by CANape as the ASAP2 database filename
   The extension .A2L or .DB is added by CANape
*/
#ifdef CCP_EXTERNAL_STATION_ID
   CCP_MEMROM0 extern CCP_ROM CCP_BYTE ccpStationId[];
#else
   CCP_MEMROM0 CCP_ROM CCP_BYTE ccpStationId[] = CCP_STATION_ID;
#endif


/*--------------------------------------------------------------------------*/
/* RAM */
/*--------------------------------------------------------------------------*/

/*
   The following structure containes all RAM locations needed by the CCP driver
   It has to be word aligned on a C167 !!!
*/

/* ##Hp - rename struct ccp */
CCP_RAM struct ccp ccp;


/*--------------------------------------------------------------------------*/
/* CODE */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* Transmit */
/*--------------------------------------------------------------------------*/

/* Send a CRM, if no other message is pending */
void ccpSendCrm( void ) {

  CCP_DISABLE_INTERRUPT;

  if (ccp.SendStatus&CCP_SEND_PENDING) {

    ccp.SendStatus |= CCP_CRM_REQUEST;

  } else {

    ccp.SendStatus |= CCP_CRM_PENDING;
    ccpSend(ccp.Crm);

  }

  CCP_ENABLE_INTERRUPT;
}


/* Send a DTM, if no other message is pending */
#ifdef CCP_DAQ
#ifndef CCP_SEND_QUEUE
static void ccpSendDtm( void ) {

  CCP_DISABLE_INTERRUPT;

  if (ccp.SendStatus&CCP_SEND_PENDING) {

    ccp.SendStatus |= CCP_DTM_REQUEST;

  } else {

    ccp.SendStatus |= CCP_DTM_PENDING;
    ccpSend(ccp.Dtm);

  }

  CCP_ENABLE_INTERRUPT;
}
#endif
#endif


/*--------------------------------------------------------------------------*/
/* Transmit Queue */
/*--------------------------------------------------------------------------*/
#ifdef CCP_SEND_QUEUE

void ccpQueueInit(void) {

  ccp.Queue.len = 0;
  ccp.Queue.rp = 0;
}

CCP_BYTE ccpQueueWrite(ccpMsg_t *msg) {

  if (ccp.Queue.len>=CCP_SEND_QUEUE_SIZE) return 0;
  ccp.Queue.msg[(ccp.Queue.rp+ccp.Queue.len)%CCP_SEND_QUEUE_SIZE] = *msg;
  ccp.Queue.len++;
  return 1;
}

#endif


/*--------------------------------------------------------------------------*/
/* Handle MTAs (Memory-Transfer-Address) */
/*--------------------------------------------------------------------------*/

/* Assign a pointer to a MTA */
#define ccpSetMTA(n,p) ccp.MTA[n] = p;

/* Write n bytes */
static CCP_BYTE ccpWriteMTA( CCP_BYTE n, CCP_BYTE size, CCP_BYTEPTR d ) {

  /* EEPROM write access */
  #ifdef CCP_WRITE_EEPROM
    CCP_BYTE r = ccpCheckWriteEEPROM(ccp.MTA[n],size,d);
    if (r) { /* EEPROM write access */
      ccp.MTA[n] += size;
      return r;
    }
  #endif

  /* Checked ram memory write access */
  #ifdef CCP_WRITE_PROTECTION
    if (!ccpCheckWriteAccess(ccp.MTA[n],size)) {
      ccp.MTA[n] += size;
      return CCP_WRITE_DENIED;
    }
  #endif

   /* double conversion */
   #ifdef CCP_DOUBLE_FLOAT
     if (size==8) {
       *(double*)ccp.MTA[n] = *(float*)d;
       #ifdef CCP_TESTMODE
         if (gDebugLevel>=2) CCPPRINT("[ccpWriteMTA] conversion -> double %g\n",*(double*)ccp.MTA[n]);
       #endif
       ccp.MTA[n] += 8;
       return CCP_WRITE_OK;
     }
   #endif

   while (size-->0) {
     /* COSMIC Compiler Bug: *(ccp.MTA[n]++) = *(d++);  */
     *(ccp.MTA[n]) = *d;
     ccp.MTA[n]++;
     d++;
   }
   return CCP_WRITE_OK;
}

/* Read n bytes */
static void ccpReadMTA( CCP_BYTE n, CCP_BYTE size, CCP_BYTEPTR d ) {

  /* EEPROM read access */
  #ifdef CCP_READ_EEPROM
    if (ccpCheckReadEEPROM(ccp.MTA[n],size,d)) {
      ccp.MTA[n] += size;
      return;
    }
  #endif

  /* double conversion */
  #ifdef CCP_DOUBLE_FLOAT
    if (size==8) {
      *(float*)d = (float)(*(double*)(ccp.MTA[n]));
      #ifdef CCP_TESTMODE
        if (gDebugLevel>=2) CCPPRINT("[ccpReadMTA] conversion -> float %g\n",(double)(*(float*)d));
      #endif
      return;
    }
  #endif

  while (size-->0) {
    *d = *(ccp.MTA[n]);
    d++;
    ccp.MTA[n]++;
  }
}


/*--------------------------------------------------------------------------*/
/* Data Aquisition Setup */
/*--------------------------------------------------------------------------*/

#ifdef CCP_DAQ

/* Clear DAQ list */
static CCP_BYTE ccpClearDaqList( CCP_BYTE daq ) {

  CCP_BYTEPTR p;
  CCP_BYTEPTR pl;

  if (daq>=CCP_MAX_DAQ) return 0;

  /* Clear this daq list to zero */
  p = (CCP_BYTEPTR)&ccp.DaqList[daq];
  pl = p+sizeof(ccpDaqList_t);
  while (p<pl) *p++ = 0;

  /* @@@@ Not DAQ list specific */
  ccp.SessionStatus |= SS_DAQ;
  #ifdef CCP_SEND_SINGLE
    ccp.CurrentDaq = 0;
    ccp.CurrentOdt = 0;
  #endif
  #ifdef CCP_SEND_QUEUE
    ccpQueueInit();
  #endif

  return CCP_MAX_ODT;
}

/* Prepare DAQ */
static CCP_BYTE ccpPrepareDaq(  CCP_BYTE daq, CCP_BYTE last, CCP_BYTE eventChannel, CCP_WORD prescaler ) {

  if (daq>=CCP_MAX_DAQ) return 0;

  ccp.DaqList[daq].eventChannel = eventChannel;
  if (prescaler==0) prescaler = 1;
  ccp.DaqList[daq].prescaler = prescaler;
  ccp.DaqList[daq].cycle = 1;
  ccp.DaqList[daq].last = last;
  ccp.DaqList[daq].flags = DAQ_FLAG_PREPARED;
  return 1;
}

/* Start DAQ */
static CCP_BYTE ccpStartDaq( CCP_BYTE daq ) {

  if (daq>=CCP_MAX_DAQ) return 0;

  ccp.DaqList[daq].flags = DAQ_FLAG_START;
  ccp.SessionStatus |= SS_RUN;

  #ifdef CCP_TIMESTAMPING
    ccpClearTimestamp();
  #endif

  return 1;
}

/* Start all prepared DAQs */
static void ccpStartAllPreparedDaq(void) {

  CCP_BYTE q;

  for (q=0;q<CCP_MAX_DAQ;q++) {
    if (ccp.DaqList[q].flags==DAQ_FLAG_PREPARED) ccp.DaqList[q].flags = DAQ_FLAG_START;
  }
  ccp.SessionStatus |= SS_RUN;

  #ifdef CCP_TIMESTAMPING
    ccpClearTimestamp();
  #endif
}

/* Stop DAQ */
static void ccpStopDaq ( CCP_BYTE daq ) {

  CCP_BYTE i;

  if (daq>=CCP_MAX_DAQ) return;
  ccp.DaqList[daq].flags = 0;

  /* check if all DAQ lists are stopped */
  for (i=0;i<CCP_MAX_DAQ;i++) {
    if (ccp.DaqList[i].flags&DAQ_FLAG_START) return;
  }

  ccp.SessionStatus &= ~SS_RUN;
}

/* Stop all DAQs */
static void ccpStopAllDaq( void ) {

  CCP_BYTE q;

  for (q=0;q<CCP_MAX_DAQ;q++) ccp.DaqList[q].flags = 0;
  ccp.SessionStatus &= ~SS_RUN;
}


/*--------------------------------------------------------------------------*/
/* Data Aquisition Processor */
/*--------------------------------------------------------------------------*/

#ifndef CCP_SEND_SINGLE

/* Sample and transmit a DTM */
static int ccpSampleAndTransmitDtm( CCP_BYTE pid, CCP_BYTE daq, CCP_BYTE odt ) {

  #ifdef CCP_SEND_QUEUE
    CCP_BYTE dtm[8];
  #else
    #define dtm ccp.Dtm
  #endif
  #ifdef CCP_DAQ_BASE_ADDR
    CCP_BYTEPTR p;
  #else
    CCP_DAQBYTEPTR p;
  #endif
  #ifdef CCP_ODT_ENTRY_SIZE
    CCP_BYTE s;
    CCP_BYTE *d,*dl;
    ccpOdtEntry_t *e,*el;
  #else
    CCP_BYTE i;
    ccpOdtEntry_t *e;
  #endif

  /* PID */
  dtm[0] = pid;

  /* Assure data consistency */
  CCP_DISABLE_INTERRUPT;

  /* Sample */
  #ifdef CCP_ODT_ENTRY_SIZE

    e = &ccp.DaqList[daq].odt[odt][0];
    el = e+8;
    d = &dtm[1];
    dl = d+7;
    while (d<dl && e<el && e->ptr) {
      #ifdef CCP_DAQ_BASE_ADDR
        p = (CCP_BYTEPTR)( e->ptr ) + CCP_DAQ_BASE_ADDR;
      #else
        p = e->ptr;
      #endif
      s = e->siz;
      #ifdef CCP_DOUBLE_FLOAT
        if (s==8) {
           *(float*)d = (float)(*(double*)p);
          s = 4;
          #ifdef CCP_TESTMODE
           if (gDebugLevel>=2) CCPPRINT("[ccpSampleAndTransmitDtm] conversion -> float %g\n",*(float*)d);
          #endif
        } else
      #endif
      if (s==4) {
        *(CCP_DWORD*)d = *(CCP_DWORD*)p;
      } else if (s==2) {
        *(CCP_WORD*)d = *(CCP_WORD*)p;
      } else {
        *d = *p;
      }
      d += s;
      e++;
    }

  #else

    e =  &ccp.DaqList[daq].odt[odt][0];
    for (i=1;i<8;i++) {
      #ifdef CCP_DAQ_BASE_ADDR
        p = (CCP_BYTEPTR)( (e++)->ptr ) + CCP_DAQ_BASE_ADDR;
      #else
        p = (e++)->ptr;
      #endif
      if (p) dtm[i] = *p;
    }

  #endif

  /* Optional for CANape: Put a timestamp in the first ODT (Byte6+7) of each DAQ */
  #ifdef CCP_TIMESTAMPING
    if (odt==0) {
      *(CCP_WORD*)&dtm[6] = ccpGetTimestamp();
    }
  #endif

  /* Queue or transmit the DTM */
  #ifdef CCP_SEND_QUEUE

    if (ccp.SendStatus&CCP_SEND_PENDING) {
      if (!ccpQueueWrite((ccpMsg_t*)dtm)) {
        /* Overun */
        CCP_ENABLE_INTERRUPT;
        return 0;
      }
    } else {
      ccp.SendStatus |= CCP_DTM_PENDING;
      ccpSend(dtm);
    }

  #else

    if (ccp.SendStatus&CCP_DTM_REQUEST) {
      /* Overun */
      CCP_ENABLE_INTERRUPT;
      return 0;
    }
    if (ccp.SendStatus&CCP_SEND_PENDING) {
      ccp.SendStatus |= CCP_DTM_REQUEST;
    } else {
      ccp.SendStatus |= CCP_DTM_PENDING;
      ccpSend(dtm);
    }

  #endif

  CCP_ENABLE_INTERRUPT;
  return 1;

}


#else


/* Sample and transmit the next DTM in SEND_SINGLE mode */
static void ccpSampleAndSendNextDtm( void ) {

  CCP_BYTE i,j;
  CCP_DAQBYTEPTR p;
  ccpOdtEntry_t *e;

  /* Request for DTM transmission pending */
  if (ccp.SendStatus&CCP_DTM_REQUEST) return;

  /* Find a DAQ list marked for transmission */
  for (i=0;i<CCP_MAX_DAQ;i++) {

    if (ccp.DaqList[ccp.CurrentDaq].flags&DAQ_FLAG_SEND) {

      /* PID */
      ccp.Dtm[0] = ccp.CurrentDaq*CCP_MAX_ODT+ccp.CurrentOdt;

      /* Sample */
      e =  &ccp.DaqList[ccp.CurrentDaq].odt[ccp.CurrentOdt][0];
      for (j=1;j<8;j++) {
        p = (e++)->ptr;
        if (p) ccp.Dtm[j] = *p;
      }

      /* Send */
      ccpSendDtm();

      /* Increment ODT */
      if (++ccp.CurrentOdt>ccp.DaqList[ccp.CurrentDaq].last) {

        /* DAQ list done */
        ccp.CurrentOdt = 0;
        ccp.DaqList[ccp.CurrentDaq].flags &= ~DAQ_FLAG_SEND;

        /* Increment DAQ */
        if (++ccp.CurrentDaq>=CCP_MAX_DAQ) ccp.CurrentDaq = 0;

      }

      break;

    } else {

      /* Increment DAQ */
      if (++ccp.CurrentDaq>=CCP_MAX_DAQ) ccp.CurrentDaq = 0;

    }


  }

}

#endif

/* Data aquisition */
void ccpDaq( CCP_BYTE eventChannel )
{
  CCP_BYTE q,o;
  #ifndef CCP_SEND_SINGLE
    CCP_BYTE j;
  #endif

  if (!(ccp.SessionStatus&SS_RUN)) return;

  SET_PORT_BIT(4); /* Timingtest */

  for (o=0,q=0; q<CCP_MAX_DAQ; o+=CCP_MAX_ODT,q++) {

    if (!(ccp.DaqList[q].flags&DAQ_FLAG_START)) continue;

    if (ccp.DaqList[q].eventChannel!=eventChannel) continue;
    if (--ccp.DaqList[q].cycle!=0) continue;
    ccp.DaqList[q].cycle = ccp.DaqList[q].prescaler;

    #ifdef CCP_SEND_SINGLE

      /* Just mark DAQ for transmission */
      ccp.DaqList[q].flags |= DAQ_FLAG_SEND;

    #else

      /* Check that the current queue space fits a complete cycle */
      #if defined(CCP_SEND_QUEUE) && defined(CCP_SEND_QUEUE_OVERRUN_INDICATION)
        if (CCP_SEND_QUEUE_SIZE-ccp.Queue.len<=ccp.DaqList[q].last) {
          ccp.DaqList[q].flags |= DAQ_FLAG_OVERRUN;
          continue; /* Skip this DAQ list on overrun */
        }
      #endif

      /* Use BIT7 of PID to indicate overruns (CANape special feature) */
      #ifdef CCP_SEND_QUEUE_OVERRUN_INDICATION

        for (j=0;j<=ccp.DaqList[q].last;j++) {
          if (!ccpSampleAndTransmitDtm((o+j)|(ccp.DaqList[q].flags&DAQ_FLAG_OVERRUN),q,j)) {
            ccp.DaqList[q].flags |= DAQ_FLAG_OVERRUN;
          } else {
            ccp.DaqList[q].flags &= ~DAQ_FLAG_OVERRUN;
          }
        } /* j */

      #else

        for (j=0;j<=ccp.DaqList[q].last;j++) {
          ccpSampleAndTransmitDtm(o+j,q,j);
        } /* j */

      #endif

    #endif

  } /* q */

  /* Check for the next ODT to send */
  #ifdef CCP_SEND_SINGLE
    ccpSampleAndSendNextDtm();
  #endif

  RST_PORT_BIT(4); /* Timingtest */

}

#endif /* CCP_DAQ */


/*--------------------------------------------------------------------------*/
/* Background Processing */
/* Used for Checksum Calculation */
/*--------------------------------------------------------------------------*/

/* Table for CCITT checksum calculation */
#ifdef CCP_CHECKSUM_CCITT
   CCP_MEMROM0 CCP_ROM CCP_WORD CRC16CCITTtab[256] = {

    0x0000,0x1189,0x2312,0x329B,0x4624,0x57AD,0x6536,0x74BF,
    0x8C48,0x9DC1,0xAF5A,0xBED3,0xCA6C,0xDBE5,0xE97E,0xF8F7,
    0x1081,0x0108,0x3393,0x221A,0x56A5,0x472C,0x75B7,0x643E,
    0x9CC9,0x8D40,0xBFDB,0xAE52,0xDAED,0xCB64,0xF9FF,0xE876,
    0x2102,0x308B,0x0210,0x1399,0x6726,0x76AF,0x4434,0x55BD,
    0xAD4A,0xBCC3,0x8E58,0x9FD1,0xEB6E,0xFAE7,0xC87C,0xD9F5,
    0x3183,0x200A,0x1291,0x0318,0x77A7,0x662E,0x54B5,0x453C,
    0xBDCB,0xAC42,0x9ED9,0x8F50,0xFBEF,0xEA66,0xD8FD,0xC974,
    0x4204,0x538D,0x6116,0x709F,0x0420,0x15A9,0x2732,0x36BB,
    0xCE4C,0xDFC5,0xED5E,0xFCD7,0x8868,0x99E1,0xAB7A,0xBAF3,
    0x5285,0x430C,0x7197,0x601E,0x14A1,0x0528,0x37B3,0x263A,
    0xDECD,0xCF44,0xFDDF,0xEC56,0x98E9,0x8960,0xBBFB,0xAA72,
    0x6306,0x728F,0x4014,0x519D,0x2522,0x34AB,0x0630,0x17B9,
    0xEF4E,0xFEC7,0xCC5C,0xDDD5,0xA96A,0xB8E3,0x8A78,0x9BF1,
    0x7387,0x620E,0x5095,0x411C,0x35A3,0x242A,0x16B1,0x0738,
    0xFFCF,0xEE46,0xDCDD,0xCD54,0xB9EB,0xA862,0x9AF9,0x8B70,
    0x8408,0x9581,0xA71A,0xB693,0xC22C,0xD3A5,0xE13E,0xF0B7,
    0x0840,0x19C9,0x2B52,0x3ADB,0x4E64,0x5FED,0x6D76,0x7CFF,
    0x9489,0x8500,0xB79B,0xA612,0xD2AD,0xC324,0xF1BF,0xE036,
    0x18C1,0x0948,0x3BD3,0x2A5A,0x5EE5,0x4F6C,0x7DF7,0x6C7E,
    0xA50A,0xB483,0x8618,0x9791,0xE32E,0xF2A7,0xC03C,0xD1B5,
    0x2942,0x38CB,0x0A50,0x1BD9,0x6F66,0x7EEF,0x4C74,0x5DFD,
    0xB58B,0xA402,0x9699,0x8710,0xF3AF,0xE226,0xD0BD,0xC134,
    0x39C3,0x284A,0x1AD1,0x0B58,0x7FE7,0x6E6E,0x5CF5,0x4D7C,
    0xC60C,0xD785,0xE51E,0xF497,0x8028,0x91A1,0xA33A,0xB2B3,
    0x4A44,0x5BCD,0x6956,0x78DF,0x0C60,0x1DE9,0x2F72,0x3EFB,
    0xD68D,0xC704,0xF59F,0xE416,0x90A9,0x8120,0xB3BB,0xA232,
    0x5AC5,0x4B4C,0x79D7,0x685E,0x1CE1,0x0D68,0x3FF3,0x2E7A,
    0xE70E,0xF687,0xC41C,0xD595,0xA12A,0xB0A3,0x8238,0x93B1,
    0x6B46,0x7ACF,0x4854,0x59DD,0x2D62,0x3CEB,0x0E70,0x1FF9,
    0xF78F,0xE606,0xD49D,0xC514,0xB1AB,0xA022,0x92B9,0x8330,
    0x7BC7,0x6A4E,0x58D5,0x495C,0x3DE3,0x2C6A,0x1EF1,0x0F78
  };
#endif

#ifndef CCP_CHECKSUM_BLOCKSIZE
  #define CCP_CHECKSUM_BLOCKSIZE 256
#endif

CCP_BYTE ccpBackground( void ) {

  SET_PORT_BIT(3); /* Timingtest */

  /* CCP command pending */
  #ifdef CCP_CMD_NOT_IN_INTERRUPT
    CCP_DISABLE_INTERRUPT;
    if (ccp.SendStatus&CCP_CMD_PENDING) {
      ccp.SendStatus &= ~CCP_CMD_PENDING;
      ccpCommand(CCP_RX_DATA_PTR);
    }
    CCP_ENABLE_INTERRUPT;
  #endif

  /* Call the user backgound function */
  ccpUserBackground();

  /* CCP checksum calculation */
  #ifdef CCP_CHECKSUM

    /*
       Checksum algorithm is not defined by the standard
       Type is defined by CCP_CHECKSUM_TYPE, Maximum blocksize is 64K
    */

    /* Checksum calculation in progress */
    if (ccp.CheckSumSize) {

      register CCP_BYTE n;
      #ifndef CCP_CHECKSUM_CCITT
        register CCP_BYTE b;
      #endif

      if (ccp.CheckSumSize<=(CCP_CHECKSUM_BLOCKSIZE-1)) {
        n = (CCP_BYTE)ccp.CheckSumSize;
        ccp.CheckSumSize = 0;
      } else {
        n = 0;
        ccp.CheckSumSize -= CCP_CHECKSUM_BLOCKSIZE;
      }

      do {
        #ifdef CCP_CHECKSUM_CCITT

          /* CRC */
          #ifdef CCP_MOTOROLA
            (*(CCP_WORD*)&ccp.Crm[4]) = CRC16CCITTtab[ccp.Crm[5] ^ *(ccp.MTA[CCP_INTERNAL_MTA]++)] ^ ccp.Crm[4];
          #else
            (*(CCP_WORD*)&ccp.Crm[4]) = CRC16CCITTtab[ccp.Crm[4] ^ *(ccp.MTA[CCP_INTERNAL_MTA]++)] ^ ccp.Crm[5];
          #endif

        #else

          /* Compiler Error BSO Tasking 16x */
          /* *(CCP_CHECKSUM_TYPE*)&ccp.Crm[4] += *(ccp.MTA[CCP_INTERNAL_MTA]++); does not work */

          /* Sum */
          b = *(ccp.MTA[CCP_INTERNAL_MTA]);
          *(CCP_CHECKSUM_TYPE*)&ccp.Crm[4] += b;
          ccp.MTA[CCP_INTERNAL_MTA]++;

        #endif
      } while (--n!=0);

      /* Checksum calculation finished ? */
      if (ccp.CheckSumSize) {
        RST_PORT_BIT(3); /* Timingtest */
        return 1; /* Still pending */
      }

      ccpSendCrm();

    } /* ccp.CheckSumSize */
  #endif

  RST_PORT_BIT(3); /* Timingtest */

  return 0;
}


/*--------------------------------------------------------------------------*/
/* Command Processor */
/*--------------------------------------------------------------------------*/

void ccpCommand( CCP_BYTEPTR com ) {

  SET_PORT_BIT(1); /* Timingtest */

  #define cmd com[0]
  #define ctr com[1]

  /* Handle CONNECT or TEST command */
  if (cmd==CC_CONNECT||cmd==CC_TEST) {

    #define stationAddr (*(CCP_WORD*)&com[2]) /* Has to be Intel-Format ! */

    #ifdef CCP_TESTMODE
      if (gDebugLevel) CCPPRINT("\n%u: %s addr=%u\n",ctr,cmd==CC_CONNECT?"CONNECT":"TEST",stationAddr);
    #endif

    /* This station */
    if (stationAddr==CCP_STATION_ADDR||stationAddr==CCP_BROADCAST_STATION_ADDR) { /* This station */

      if (cmd==CC_CONNECT) {
        #ifdef CCP_DAQ
          if (!(ccp.SessionStatus&SS_TMP_DISCONNECTED)) {
            ccpStopAllDaq();
            ccp.SendStatus = 0; /* Clear all transmission flags */
          }
        #endif
        ccp.SessionStatus |= SS_CONNECTED;
        ccp.SessionStatus &= ~SS_TMP_DISCONNECTED;

        #ifdef CCP_TESTMODE
          if (gDebugLevel) CCPPRINT("*** connected ***\n");
        #endif
      }

      /* Responce */
      /* Station addresses in Intel Format */
      ccp.Crm[0] = 0xFF;
      ccp.Crm[1] = CRC_OK;
      ccp.Crm[2] = ctr;
      ccp.Crm[3] = 0xFE;
      *(CCP_WORD*)&ccp.Crm[4] = CCP_STATION_ADDR;
      *(CCP_WORD*)&ccp.Crm[6] = CCP_BROADCAST_STATION_ADDR;

      /* responce */
      /* |||| */
    }

    /* Another station */
    else {

      /* If connected, temporary disconnect */
      if (ccp.SessionStatus&SS_CONNECTED) {

        ccp.SessionStatus &= ~SS_CONNECTED;
        ccp.SessionStatus |= SS_TMP_DISCONNECTED;

        #ifdef CCP_TESTMODE
          if (gDebugLevel) CCPPRINT("*** temporary disconnect ***\n");
        #endif

      }

      RST_PORT_BIT(1); /* Timingtest */

      /* no responce */
      return;

    }

  }

  /* Handle other commands only if connected */
  else if (ccp.SessionStatus&SS_CONNECTED) {

    /* prepare the responce */
    ccp.Crm[0] = 0xFF;
    ccp.Crm[1] = CRC_OK;
    ccp.Crm[2] = ctr;

    switch (cmd) {

      case CC_DISCONNECT:
        {
          #define disconnectCmd com[2]
          #define disconnectStationAddr (*(CCP_WORD*)&com[4])

          ccp.SessionStatus &= ~SS_CONNECTED;
          #ifdef CCP_TESTMODE
            if (gDebugLevel) CCPPRINT("%u: DISCONNECT cmd=%u\n",ctr,disconnectCmd);
          #endif

          if (disconnectCmd==0x00) { /* Temporary */
            ccp.SessionStatus |= SS_TMP_DISCONNECTED;
            #ifdef CCP_TESTMODE
              if (gDebugLevel) CCPPRINT("*** temporary disconnect ***\n");
            #endif
          } else {           /* End of session */
            #ifdef CCP_DAQ
              ccpStopAllDaq();
            #endif
            #ifdef CCP_SEED_KEY
              ccp.ProtectionStatus = 0; /* Clear Protection Status */
            #endif
            #ifdef CCP_TESTMODE
              if (gDebugLevel) CCPPRINT("*** end of session ***\n");
            #endif
          }
        }
        break;

      case CC_EXCHANGE_ID: /* Exchange Station Identifications */
        {
          CCP_BYTE i;
          #define masterId com[2]
          for(i=0;ccpStationId[i]!=0;i++) ;
          ccp.Crm[3] = i; /* Lenght of slave device identifier */
          ccp.Crm[4] = 0;
          /* Build the Resource Availability and Protection Mask */
          ccp.Crm[5] = PL_CAL; /* Default: Calibration available */
          ccp.Crm[6] = 0;      /* Default: No Protection */
          #ifdef CCP_SEED_KEY
            ccp.Crm[6] |= PL_CAL;   /* Protected Calibration */
          #endif
          #ifdef CCP_DAQ
            ccp.Crm[5] |= PL_DAQ;     /* Data Acquisition */
            #ifdef CCP_SEED_KEY
              ccp.Crm[6] |= PL_DAQ;   /* Protected Data Acquisition */
            #endif
          #endif
          #if defined(CCP_PROGRAM) || defined(CCP_BOOTLOADER_DOWNLOAD)
            ccp.Crm[5] |= PL_PGM;     /* Flash Programming */
            #ifdef CCP_SEED_KEY
              ccp.Crm[6] |= PL_PGM;   /* Protected Flash Programming */
            #endif
          #endif
          ccp.Crm[7] = CCP_DRIVER_VERSION; /* Driver version number */
          ccpSetMTA(0,(CCP_MTABYTEPTR)ccpStationId);
          #ifdef CCP_TESTMODE
            if (gDebugLevel) CCPPRINT("%u: EXCANGE_ID master_id=%u\n",ctr,masterId);
          #endif
        }
        break;

      #ifdef CCP_SEED_KEY

      case CC_GET_SEED: /* Get Seed for Key */
        {
          #define privilegeLevel com[2]
          ccp.Crm[3] = 0; /* Protection Status: No key required */
          *(CCP_DWORD*)&ccp.Crm[4] = 0;
          #ifdef CCP_SEED_KEY
            /* Keys required for CAL or PGM */
            switch (privilegeLevel) {
              case PL_CAL:
                ccp.Crm[3] = (0==(ccp.ProtectionStatus&PL_CAL)); /* Protection Status */
                *(CCP_DWORD*)&ccp.Crm[4] = ccpGetSeed(PL_CAL);
                break;
              case PL_PGM:
                ccp.Crm[3] = (0==(ccp.ProtectionStatus&PL_PGM)); /* Protection Status */
                *(CCP_DWORD*)&ccp.Crm[4] = ccpGetSeed(PL_PGM);
                break;
              case PL_DAQ:
                ccp.Crm[3] = (0==(ccp.ProtectionStatus&PL_DAQ)); /* Protection Status */
                *(CCP_DWORD*)&ccp.Crm[4] = ccpGetSeed(PL_DAQ);
                break;
              default:
                ccp.Crm[1] = CRC_CMD_SYNTAX;
                /* Error */
            }
          #endif
          #ifdef CCP_TESTMODE
            if (gDebugLevel) CCPPRINT("%u: GET_SEED resource=%02X, protection_status=%u\n",ctr,privilegeLevel,ccp.Crm[3]);
          #endif
        }
        break;

      case CC_UNLOCK: /* Unlock Protection */
        {
          #define key com[2] /* Key may be up to 6 Bytes */
          /* Check key */
          ccp.ProtectionStatus |= ccpUnlock(&com[2]); /* Reset the appropriate resource protection mask bit */
          ccp.Crm[3] = ccp.ProtectionStatus; /* Current Protection Status */
          #ifdef CCP_TESTMODE
            if (gDebugLevel) CCPPRINT("%u: UNLOCK protection_status=%02X\n",ctr,ccp.ProtectionStatus);
          #endif
        }
        break;

      #endif /* CCP_SEED_KEY */

      case CC_SET_MTA: /* Set transfer address */
        {
          #define mta     com[2]
          #define addrExt com[3]
          #define addr    (*(CCP_DWORD*)&com[4])
         if (mta<CCP_MAX_MTA-1) {
            ccpSetMTA(mta,ccpGetPointer(addrExt,addr));
         } else {
            ccp.Crm[1] = CRC_OUT_OF_RANGE;
         }
          #ifdef CCP_TESTMODE
            if (gDebugLevel) CCPPRINT("%u: SET_MTA %u to %08lX,%u\n",ctr,mta,addr,addrExt);
          #endif

          #undef mta
          #undef addrExt
          #undef addr
        }
        break;

      case CC_DNLOAD: /* Download */
        {
          CCP_BYTE r;
          #define size com[2]
          #ifdef CCP_SEED_KEY
            if (!(ccp.ProtectionStatus&PL_CAL)) {
              ccp.Crm[1] = CRC_ACCESS_DENIED;
              r = 0;
            }
            else
          #endif
          r = ccpWriteMTA(0,size,&com[3]);
          #ifdef CCP_STANDARD
            ccpGetMTA0((CCP_BYTE*)&ccp.Crm[3],(CCP_DWORD*)&ccp.Crm[4]);
          #endif
          if (r==CCP_WRITE_PENDING) return; /* EEPROM write pending */
          if (r==CCP_WRITE_DENIED||r==CCP_WRITE_ERROR) ccp.Crm[1] = CRC_ACCESS_DENIED; /* No write access */
          #ifdef CCP_TESTMODE
            if (gDebugLevel) {
              int i;
              CCPPRINT("%u: DNLOAD n=%u, ",ctr,size);
              for (i=3;i<3+size&&i<8;i++) CCPPRINT("%02X ",com[i]);
              CCPPRINT("\n");
            }
          #endif

          #undef size      /* avoid compiler re-definition warning */
        }
        break;

      case CC_DNLOAD6: /* Download */
        {
          CCP_BYTE r;
          #ifdef CCP_SEED_KEY
            if (!(ccp.ProtectionStatus&PL_CAL)) {
              ccp.Crm[1] = CRC_ACCESS_DENIED;
              r = 0;
            }
            else
          #endif
          r = ccpWriteMTA(0,6,&com[2]);
          #ifdef CCP_STANDARD
            ccpGetMTA0((CCP_BYTE*)&ccp.Crm[3],(CCP_DWORD*)&ccp.Crm[4]);
          #endif
          if (r==CCP_WRITE_PENDING) return; /* EEPROM write pending */
          if (r==CCP_WRITE_DENIED||r==CCP_WRITE_ERROR) ccp.Crm[1] = CRC_ACCESS_DENIED; /* No write access */
          #ifdef CCP_TESTMODE
            if (gDebugLevel) {
              int i;
              CCPPRINT("%u: DNLOAD6 ",ctr);
              for (i=2;i<8;i++) CCPPRINT("%02X ",com[i]);
              CCPPRINT("\n");
          }
          #endif
        }
        break;

      case CC_UPLOAD: /* Upload */
        {
          #define size com[2]
          ccpReadMTA(0,size,&ccp.Crm[3]);
          #ifdef CCP_TESTMODE
            if (gDebugLevel) {
              int i;
              CCPPRINT("%u: UPLOAD n=%u, ",ctr,size);
              for (i=3;i<3+size;i++) CCPPRINT("%02X ",ccp.Crm[i]);
              CCPPRINT("\n");
            }
          #endif

          #undef size      /* avoid compiler re-definition warning */
        }
        break;

      case CC_SHORT_UPLOAD: /* Upload with Address */
        {
          #define size    com[2]
          #define addrExt com[3]
          #define addr    (*(CCP_DWORD*)&com[4])
          ccpSetMTA(CCP_INTERNAL_MTA,ccpGetPointer(addrExt,addr));
          ccpReadMTA(CCP_INTERNAL_MTA,size,&ccp.Crm[3]);
          #ifdef CCP_TESTMODE
            if (gDebugLevel) {
              int i;
              CCPPRINT("%u: SHORT_UPLOAD p=%08lX,%u,n=%u, ",ctr,addr,addrExt,size);
              for (i=3;i<3+size&&i<8;i++) CCPPRINT("%02X ",ccp.Crm[i]);
              CCPPRINT("\n");
            }
          #endif

          #undef size
          #undef addrExt
          #undef addr
        }
        break;

      case CC_GET_DAQ_SIZE: /* Return the size of a DAQ list and clear */
        {
          #define daqList com[2]
          #define daqId   (*(CCP_DWORD*)&com[4])
          #ifdef CCP_DAQ
            ccpStopDaq(daqList); /* Stop this daq list */

            ccp.Crm[3] = ccpClearDaqList(daqList); /* Number of  ODTs */
            ccp.Crm[4] = daqList*CCP_MAX_ODT; /* PID of the first ODT */
            #ifdef CCP_TESTMODE
              if (gDebugLevel) CCPPRINT("%u: GET_DAQ_SIZE daq=%u,id=%lu,size=%u,pid=%u\n",ctr,daqList,daqId,ccp.Crm[3],daqList*CCP_MAX_ODT);
            #endif
          #else
            ccp.Crm[3] = 0;
            ccp.Crm[4] = 0;
            #ifdef CCP_TESTMODE
              if (gDebugLevel) CCPPRINT("%u: GET_DAQ_SIZE daq=%u,size=0\n",ctr,daqList,daqId);
            #endif
          #endif
          #undef daqList
          #undef daqId
        }
        break;

      #ifdef CCP_DAQ

      case CC_SET_DAQ_PTR: /* Set DAQ pointer */
        {
          #define comDaq  com[2]
          #define comOdt  com[3]
          #define comIdx  com[4]
          if (comDaq>=CCP_MAX_DAQ||comOdt>=CCP_MAX_ODT||comIdx>7) {
            ccp.Crm[1] = CRC_CMD_SYNTAX;
            ccp.DaqListPtr = 0;
          } else {
            ccp.DaqListPtr = &ccp.DaqList[comDaq].odt[comOdt][comIdx];
          }
          #ifdef CCP_TESTMODE
            if (gDebugLevel) CCPPRINT("%u: SET_DAQ_PTR daq=%u,odt=%u,idx=%u\n",ctr,comDaq,comOdt,comIdx);
          #endif

          #undef comDaq
          #undef comOdt
          #undef comIdx
        }
        break;

      case CC_WRITE_DAQ: /* Write DAQ entry */
        {
          #define writeDaqSize    com[2]
          #define writeDaqAddrExt com[3]
          #define writeDaqAddr    (*(CCP_DWORD*)&com[4])
          if (
            #ifdef CCP_ODT_ENTRY_SIZE
              #ifdef CCP_DOUBLE_FLOAT
                (writeDaqSize!=8) &&
              #endif
              (writeDaqSize!=1&&writeDaqSize!=2&&writeDaqSize!=4)
            #else
              writeDaqSize!=1
            #endif
            || ccp.DaqListPtr==0) {
            ccp.Crm[1] = CRC_CMD_SYNTAX;
          } else {
            #ifdef CCP_DAQ_BASE_ADDR
              ccp.DaqListPtr->ptr = ccpGetDaqPointer(writeDaqAddrExt,writeDaqAddr);
            #else
              ccp.DaqListPtr->ptr = (CCP_DAQBYTEPTR)ccpGetPointer(writeDaqAddrExt,writeDaqAddr);
            #endif
            #ifdef CCP_ODT_ENTRY_SIZE
              ccp.DaqListPtr->siz = writeDaqSize;
            #endif
          }
          #ifdef CCP_TESTMODE
            if (gDebugLevel) CCPPRINT("%u: WRITE_DAQ size=%u,ext=%u,addr=%lu\n",ctr,writeDaqSize,writeDaqAddrExt,writeDaqAddr);
          #endif

          #undef writeDaqSize
          #undef writeDaqAddrExt
          #undef writeDaqAddr
        }
        break;

      case CC_START_STOP: /* Cyclic aquisition start/stop */
        {
          #define ssCmd                 com[2]  /* Start or Stop */
          #define ssDaq                 com[3]  /* DAQ list */
          #define ssLast                com[4]  /* Last ODT to send */
          #define ssEventChannel        com[5]  /* Event Channel Number */
          #define ssPrescaler (*(CCP_WORD*)&com[6]) /* Prescaler */

          #ifdef CCP_SEED_KEY
            if (!(ccp.ProtectionStatus&PL_DAQ))
              ccp.Crm[1] = CRC_ACCESS_DENIED;
            else
          #endif
          if (!(ccp.SessionStatus&SS_DAQ)) { /* Not initialized */
           ccp.Crm[1] = CRC_DAQ_INIT_REQUEST;
          }
          else {
            switch (ssCmd) {
              case 0: /* stop */
                ccpStopDaq(ssDaq);
                break;
              case 1: /* start */
                ccpPrepareDaq(ssDaq,ssLast,ssEventChannel,ssPrescaler);
                ccpStartDaq(ssDaq);
                #ifdef CCP_TESTMODE
                  if (gDebugLevel) ccpPrintDaqList(ssDaq);
                #endif
                break;
              case 2: /* prepare */
                ccpPrepareDaq(ssDaq,ssLast,ssEventChannel,ssPrescaler);
                #ifdef CCP_TESTMODE
                  if (gDebugLevel) ccpPrintDaqList(ssDaq);
                #endif
                break;
              default:
                ccp.Crm[1] = CRC_CMD_SYNTAX;
                break;
            }
          }

          #ifdef CCP_TESTMODE
            if (gDebugLevel) CCPPRINT("%u: START_STOP cmd=%u,daq=%u,last=%u,eventChannel=%u,prescaler=%u\n",ctr,ssCmd,ssDaq,ssLast,ssEventChannel,ssPrescaler);
          #endif

          #undef ssCmd
          #undef ssDaq
          #undef ssLast
          #undef ssEventChannel
          #undef ssPrescaler
        }
        break;

      case CC_START_STOP_ALL: /* Cyclic aquisition start/stop */
        {
          #define ssCmd                 com[2]  /* Start or Stop */

          #ifdef CCP_SEED_KEY
            if (!(ccp.ProtectionStatus&PL_DAQ))
              ccp.Crm[1] = CRC_ACCESS_DENIED;
            else
          #endif
          if (!(ccp.SessionStatus&SS_DAQ)) { /* Not initialized */
           ccp.Crm[1] = CRC_DAQ_INIT_REQUEST;
          } else {
            switch (ssCmd) {
              case 0: /* Stop */
                ccpStopAllDaq();
                break;
              case 1: /* Start */
                ccpStartAllPreparedDaq();
                break;
              default:
                ccp.Crm[1] = CRC_CMD_SYNTAX;
                break;
            }
          }
          #ifdef CCP_TESTMODE
            if (gDebugLevel) CCPPRINT("%u: START_STOP_ALL cmd=%u\n",ctr,ssCmd);
          #endif

          #undef ssCmd
        }
        break;

      #endif /* CCP_DAQ */

      #ifdef CCP_CHECKSUM

      case CC_BUILD_CHKSUM: /* Build Checksum */
        {
          CCP_DWORD s;

          /* Initialize Responce */
          ccp.Crm[3] = sizeof(CCP_CHECKSUM_TYPE); /* Checksum Size */
          #ifdef CCP_CHECKSUM_CCITT               /* Checksum */
            *(CCP_DWORD*)&ccp.Crm[4] = 0xFFFFFFFF;
          #else
            *(CCP_DWORD*)&ccp.Crm[4] = 0;
          #endif
          ccp.MTA[CCP_INTERNAL_MTA] = ccp.MTA[0];        /* MTA[0] is not affected */
          #ifdef CCP_MOTOROLA
            s = (*(CCP_WORD*)&com[4]) | ((*(CCP_WORD*)&com[2])<<16);
          #else
            s = (*(CCP_WORD*)&com[2]) | ((*(CCP_WORD*)&com[4])<<16);
          #endif
          #ifdef CCP_TESTMODE
            if (gDebugLevel) CCPPRINT("%u: BUILD_CHKSUM size=%u\n",ctr,s);
          #endif
          #ifndef CCP_CPUTYPE_32BIT
            if (s&0xffff0000) ccp.Crm[1] = CRC_OUT_OF_RANGE; /* Range, max. 64K-1 on <32Bit CPUs */
            ccp.CheckSumSize = (CCP_WORD)s;
          #else
            ccp.CheckSumSize = s;
          #endif
          if (ccp.Crm[1]==0) {
            RST_PORT_BIT(1); /* Timingtest */
            return; /* Checksum calculation will be performed by ccpBackground() */
          }

        }
        break;

      #endif /* CCP_CHECKSUM */

      /* Flash Programming Kernel Download */
      #ifdef CCP_BOOTLOADER_DOWNLOAD

      case CC_PROGRAM_PREPARE: /* Prepare flash kernel download */
        {
          #ifdef CCP_TESTMODE
            if (gDebugLevel) CCPPRINT("%u: PROGRAM_PREPARE\n",ctr);
          #endif
          if (!ccpDisableNormalOperation(ccp.MTA[0],*(CCP_WORD*)&com[2])) {
            ccp.Crm[1] = CRC_ACCESS_DENIED;
          }
        }
        break;

      case CC_PROGRAM_START:   /* Start flash kernel */
        {
          #ifdef CCP_TESTMODE
            if (gDebugLevel) CCPPRINT("%u: PROGRAM_START\n",ctr);
          #endif
          #ifdef CCP_SEED_KEY
            if (!(ccp.ProtectionStatus&PL_PGM))
              ccp.Crm[1] = CRC_ACCESS_DENIED;
            else
          #endif
          ((ccpBootLoader_t)ccp.MTA[0])(&ccp,com);
        }
        break;

      #endif /* CCP_BOOTLOADER_DOWNLOAD */
                           
      /* Flash Programming */
      #ifdef CCP_PROGRAM 

      case CC_CLEAR_MEMORY: /* Clear Memory */
        {
          CCP_DWORD s;

          #ifdef CCP_SEED_KEY
            if (!(ccp.ProtectionStatus&PL_PGM))
              ccp.Crm[1] = CRC_ACCESS_DENIED;
            else
          #endif

          #ifdef CCP_BOOTLOADER
            /* Transfer control to the CCP bootloader */
            ccpBootLoaderStartup(&ccp,com); /* Never returns */
          #else
            /* Clear flash sector */
            #ifdef CCP_MOTOROLA
              s = (*(CCP_WORD*)&com[4]) | ((*(CCP_WORD*)&com[2])<<16);
            #else
              s = (*(CCP_WORD*)&com[2]) | ((*(CCP_WORD*)&com[4])<<16);
            #endif
            ccpFlashClear(ccp.MTA[0],s);
          #endif

          #ifdef CCP_TESTMODE
            if (gDebugLevel) CCPPRINT("%u: CLEAR_MEMORY size=%u\n",ctr,s);
          #endif
        }
        break;

      /* Flash Programming */
      #ifndef CCP_BOOTLOADER
      
      case CC_PROGRAM: /* Program */
        {
          CCP_BYTE r;
          #define size com[2]
          #ifdef CCP_SEED_KEY
            if (!(ccp.ProtectionStatus&PL_PGM)) {
              ccp.Crm[1] = CRC_ACCESS_DENIED;
              r = 0;
            }
            else
          #endif
          r = ccpFlashProgramm(&com[3],ccp.MTA[0],size);
          ccp.MTA[0] += size;
          if (r==CCP_WRITE_PENDING) return; /* FLASH write pending */
          #ifdef CCP_TESTMODE
            if (gDebugLevel) {
              CCPPRINT("%u: PROGRAM ",ctr);
              if (size==0) {
                CCPPRINT("EOS\n");
              } else {
                int i;
                for (i=0;i<size;i++) CCPPRINT("%02X ",com[3+i]);
                CCPPRINT("\n");
              }
            }
          #endif

          #undef size
        }
        break;

      case CC_PROGRAM6: /* Program */
        {
          CCP_BYTE r;
          #ifdef CCP_SEED_KEY
            if (!(ccp.ProtectionStatus&PL_PGM)) {
              ccp.Crm[1] = CRC_ACCESS_DENIED;
              r = 0;
            }
            else
          #endif
          r = ccpFlashProgramm(&com[2],ccp.MTA[0],6);
          ccp.MTA[0] += 6;
          if (r==CCP_WRITE_PENDING) return; /* FLASH write pending */
          #ifdef CCP_TESTMODE
            if (gDebugLevel) {
              int i;
              CCPPRINT("%u: PROGRAM6: ",ctr);
              for (i=0;i<6;i++) CCPPRINT("%02X ",com[2+i]);
              CCPPRINT("\n");
            }
          #endif
        }
        break;
        
      #endif /* !CCP_BOOTLOADER */
      #endif /* CCP_PROGRAM */
      
      #ifdef CCP_CALPAGE

      case CC_SET_CAL_PAGE: /* Select Calibration Page */
        {
          ccpSetCalPage((CCP_DWORD)ccp.MTA[0]);
          #ifdef CCP_TESTMODE
            if (gDebugLevel) CCPPRINT("%u: SEL_CAL_PAGE id=%08Xh\n",ctr,(CCP_DWORD)ccp.MTA[0]);
          #endif
        }
        break;

      case CC_GET_CAL_PAGE: /* Get Active Calibration Page */
        {
          ccp.Crm[3] = 0; /* Address Extension */
          *(CCP_DWORD*)&ccp.Crm[4] = ccpGetCalPage(); /* Address */
          #ifdef CCP_TESTMODE
            if (gDebugLevel) CCPPRINT("%u: GET_CAL_PAGE -> id=%08Xh\n",ctr,*(CCP_DWORD*)&ccp.Crm[4]);
          #endif
        }
        break;
                       
      #endif /* CCP_CALPAGE */
      
      #ifdef CCP_SET_SESSION_STATUS

      case CC_SET_S_STATUS: /* Set Session Status */
        {
          /* Set Resume and Store mode in SessionStatus */
          ccp.SessionStatus &= ~(SS_STORE|SS_RESUME);
          ccp.SessionStatus |= (com[2]&(SS_STORE|SS_RESUME));

          /* Save as UserSessionStatus */
          ccp.UserSessionStatus = com[2];

          #ifdef CCP_TESTMODE
            if (gDebugLevel) CCPPRINT("%u: SET_S_STATUS status=%02X\n",ctr,ccp.UserSessionStatus);
          #endif
        }
        break;

      case CC_GET_S_STATUS: /* Get Session Status */
        {
          ccp.Crm[3] = ccp.UserSessionStatus;
          ccp.Crm[4] = 0; /* No additional status */

          #ifdef CCP_TESTMODE
            if (gDebugLevel) CCPPRINT("%u: GET_S_STATUS -> status=%02X\n",ctr,ccp.Crm[3]);
          #endif
        }
        break;
      
      #endif /* CCP_SET_SESSION_STATUS */
      
      case CC_GET_CCP_VERSION: /* Get Version */
        {
          ccp.Crm[3] = CCP_VERSION_MAJOR;
          ccp.Crm[4] = CCP_VERSION_MINOR;
          #ifdef CCP_TESTMODE
            if (gDebugLevel) CCPPRINT("%u: GET_VERSION -> major=%02X minor=%02X\n",ctr,ccp.Crm[3],ccp.Crm[4]);
          #endif
        }
        break;

      default: /* unknown */
        {
          ccp.Crm[1] = CRC_CMD_UNKNOWN;
          #ifdef CCP_TESTMODE
            if (gDebugLevel) CCPPRINT("*** UNKNOWN COMMAND ***\n");
          #endif
          break;
        }

    } /* switch */


    /* Responce */
    /* |||| */
  }

  /* Not connected */
  else {

    RST_PORT_BIT(1); /* Timingtest */

    /* No responce */
    return;
  }

  ccpSendCrm();

  RST_PORT_BIT(1); /* Timingtest */
}


/*--------------------------------------------------------------------------*/
/* Send notification callback */
/* ccpSend must not fail, when called from this function */
/*--------------------------------------------------------------------------*/
CCP_BYTE ccpSendCallBack( void ) {

  SET_PORT_BIT(2); /* Timingtest */

  /* Clear all pending flags, except for CCP_CMD_PENDING */
  ccp.SendStatus &= ~CCP_SEND_PENDING;

  /* Send a CRM message */
  if (ccp.SendStatus&CCP_CRM_REQUEST) {
    ccp.SendStatus &= ~CCP_CRM_REQUEST;
    ccpSendCrm();
    RST_PORT_BIT(2); /* Timingtest */
    return 1;
  }

  /* Send a DAQ message */
  #ifdef CCP_DAQ
    if (ccp.SessionStatus&SS_RUN) {

      /* Send a  DAQ message (DTM) from the queue */
      #ifdef CCP_SEND_QUEUE

        CCP_DISABLE_INTERRUPT;
        if (ccp.Queue.len) {
          ccp.SendStatus |= CCP_DTM_PENDING;
          ccpSend((CCP_BYTEPTR)&ccp.Queue.msg[ccp.Queue.rp]);
          ccp.Queue.rp++;
          if (ccp.Queue.rp>=CCP_SEND_QUEUE_SIZE) ccp.Queue.rp = 0;
          ccp.Queue.len--;
          CCP_ENABLE_INTERRUPT;
          RST_PORT_BIT(2); /* Timingtest */
          return 1;
        }
        CCP_ENABLE_INTERRUPT;

      /* Send a pending DAQ message (DTM) */
      #else

        if (ccp.SendStatus&CCP_DTM_REQUEST) {
          ccp.SendStatus &= ~CCP_DTM_REQUEST;
          ccpSendDtm();
          RST_PORT_BIT(2); /* Timingtest */
          return 1;
        }

      #endif
    }
  #endif

  RST_PORT_BIT(2); /* Timingtest */
  return 0;
}


/*--------------------------------------------------------------------------*/
/* Initialization */
/*--------------------------------------------------------------------------*/

void ccpInit( void ) {

  /* Initialize all CCP variables to zero */
  CCP_BYTEPTR p;
  CCP_BYTEPTR pl;
  p = (CCP_BYTEPTR)&ccp;
  pl = p+sizeof(ccp);
  while (p<pl) *p++ = 0;

}


/*--------------------------------------------------------------------------*/
/* Test */
/* Screen output for test and diagnostics */
/*--------------------------------------------------------------------------*/

#ifdef CCP_TESTMODE

#ifdef CCP_DAQ

/* Print all DAQ lists to screen */
static void ccpPrintDaqList( CCP_BYTE daq )
{

  CCP_BYTE i,j,f;

  if (daq>=CCP_MAX_DAQ) return;
  CCPPRINT("DAQ-List %u:\n",daq);
  CCPPRINT(" event channel=%u,",ccp.DaqList[daq].eventChannel);
  CCPPRINT(" prescaler=%u,",ccp.DaqList[daq].prescaler);
  CCPPRINT(" last=%u,",ccp.DaqList[daq].last);
  CCPPRINT(" flags=%u\n",ccp.DaqList[daq].flags);
  for (j=0;j<CCP_MAX_ODT;j++) {
    for (i=0,f=1;i<=6;i++) {
      if (ccp.DaqList[daq].odt[j][i].ptr) {
        if (f) { CCPPRINT(" PID %u: ",daq*CCP_MAX_ODT+j); f = 0; }
        #ifdef CCP_ODT_ENTRY_SIZE
          CCPPRINT("[%p,%u],",ccp.DaqList[daq].odt[j][i].ptr,ccp.DaqList[daq].odt[j][i].siz);
        #else
          CCPPRINT("[%p],",ccp.DaqList[daq].odt[j][i].ptr);
        #endif
      }
    }
    if (!f) CCPPRINT("\n");
  } /* j */
}

#endif

#endif

#endif
