  
/** 
 * FILE: Exceptions.c
 *
 *  DESCRIPTION: Setup of IVPR to point to the EXCEPTION_HANDLERS memory area 
 *               defined in the linker command file.
 *               Default setup of the IVORxx registers. 
*/


/*----------------------------------------------------------------------------*/
/* Includes                                                                   */
/*----------------------------------------------------------------------------*/

#include "Exceptions.h" /* Implement functions from this file */

/*----------------------------------------------------------------------------*/
/* Function Implementations                                                   */
/*----------------------------------------------------------------------------*/

#pragma push /* Save the current state */
/* Symbol EXCEPTION_HANDLERS is defined in the application linker command file (.lcf) 
   It is defined to the start of the code memory area used for the .__exception_handlers section. 
*/
/*lint -esym(752, EXCEPTION_HANDLERS) */
__declspec (section ".__exception_handlers") extern long EXCEPTION_HANDLERS;  
#pragma force_active on
#pragma function_align 16 /* We use 16 bytes alignment for Exception handlers */
__declspec(interrupt)
__declspec (section ".__exception_handlers")
void EXCEP_DefaultExceptionHandler(void)
{

}
#pragma force_active off
#pragma pop

__asm void EXCEP_InitExceptionHandlers(void)
{
nofralloc

    /* Set the IVPR to the Exception Handlers memory area defined in the lcf file */ 
    lis     r0, EXCEPTION_HANDLERS@h
    ori     r0, r0, EXCEPTION_HANDLERS@l
    mtivpr  r0

    /* Set all IVOR registers to the Default Exception Handler */
    lis     r0, EXCEP_DefaultExceptionHandler@h
    ori     r0, r0, EXCEP_DefaultExceptionHandler@l

#if !defined(Z0H_CORE)
    /* IVORx registers are "hard-wired" in the e200z0 and z0h cores */

    /* IVOR0 Critical input (SPR 400) */
    mtivor0 r0
    /* IVOR1 Machine check interrupt (SPR 401) */
    mtivor1 r0
    /* IVOR2 Data storage interrupt (SPR 402) */
    mtivor2 r0
    /* IVOR3 Instruction storage interrupt (SPR 403) */
    mtivor3 r0
    /* IVOR4 External input interrupt (SPR 404) */
    mtivor4 r0
    /* IVOR5 Alignment interrupt (SPR 405) */
    mtivor5 r0
    /* IVOR6 Program interrupt (SPR 406) */
    mtivor6 r0
    /* IVOR7 Floating-point unavailable interrupt (SPR 407) */
    mtivor7 r0
    /* IVOR8 System call interrupt (SPR 408) */
    mtivor8 r0
    /* IVOR9 Auxiliary processor (SPR 409) */
    mtivor9 r0
    /* IVOR10 Decrementer interrupt (SPR 410) */
    mtivor10 r0
    /* IVOR11 Fixed-interval timer interrupt (SPR 411) */
    mtivor11 r0
    /* IVOR12 Watchdog timer interrupt (SPR 412) */
    mtivor12 r0
    /* IVOR13 Data TLB error interrupt (SPR 413) */
    mtivor13 r0        
    /* IVOR14 Instruction TLB error interrupt (SPR 414) */
    mtivor14 r0  
    /* IVOR15 Debug interrupt (SPR 415) */
    mtivor15 r0 

    /* IVOR32 SPE APU unavailable interrupt (SPR 528) */
    mtivor32 r0 
    /* IVOR33 SPE floating-point data exception interrupt (SPR 529) */
    mtivor33 r0 
    /* IVOR34 SPE floating-point round exception interrupt (SPR 530) */
    mtivor34 r0

#endif

    blr
}
