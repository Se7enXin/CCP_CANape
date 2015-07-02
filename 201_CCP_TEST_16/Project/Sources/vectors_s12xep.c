/*
************************************************************************
*
*                  Freescale MC9S12XEP100 ISR Vector Definitions
*
* File Name  : vectors_s12xep.c
* Version    : 1.0
* Date       : February 8, 2008
* Programmer : Eric Shufro
************************************************************************
*/

/*
************************************************************************
*	                EXTERNAL ISR FUNCTION PROTOTYPES
************************************************************************
*/

extern void near _Startup(void);                    /* Startup Routine                          */

extern void near  RTI_ISR(void);    //RTI 







/*
************************************************************************
*	              DUMMY INTERRUPT SERVICE ROUTINES
*
* Description : When a spurious interrupt occurs, the processor will 
*               jump to the dedicated default handler and stay there
*               so that the source interrupt may be identified and
*               debugged.
*
* Notes       : Do Not Modify
************************************************************************
*/

#pragma CODE_SEG __NEAR_SEG NON_BANKED 
__interrupt void software_trap10 (void) {for(;;);}
__interrupt void software_trap12 (void) {for(;;);}
__interrupt void software_trap14 (void) {for(;;);}
__interrupt void software_trap16 (void) {for(;;);}
__interrupt void software_trap18 (void) {for(;;);}
__interrupt void software_trap1A (void) {for(;;);}
__interrupt void software_trap1C (void) {for(;;);}
__interrupt void software_trap1E (void) {for(;;);}
__interrupt void software_trap20 (void) {for(;;);}
__interrupt void software_trap22 (void) {for(;;);}
__interrupt void software_trap24 (void) {for(;;);}
__interrupt void software_trap26 (void) {for(;;);}
__interrupt void software_trap28 (void) {for(;;);}
__interrupt void software_trap2A (void) {for(;;);}
__interrupt void software_trap2C (void) {for(;;);}
__interrupt void software_trap2E (void) {for(;;);}
__interrupt void software_trap30 (void) {for(;;);}
__interrupt void software_trap32 (void) {for(;;);}
__interrupt void software_trap34 (void) {for(;;);}
__interrupt void software_trap36 (void) {for(;;);}
__interrupt void software_trap38 (void) {for(;;);}
__interrupt void software_trap3A (void) {for(;;);}
__interrupt void software_trap3C (void) {for(;;);}
__interrupt void software_trap3E (void) {for(;;);}
__interrupt void software_trap40 (void) {for(;;);}
__interrupt void software_trap42 (void) {for(;;);}
__interrupt void software_trap44 (void) {for(;;);}
__interrupt void software_trap46 (void) {for(;;);}
__interrupt void software_trap48 (void) {for(;;);}
__interrupt void software_trap4A (void) {for(;;);}
__interrupt void software_trap4C (void) {for(;;);}
__interrupt void software_trap4E (void) {for(;;);}
__interrupt void software_trap50 (void) {for(;;);}
__interrupt void software_trap52 (void) {for(;;);}
__interrupt void software_trap54 (void) {for(;;);}
__interrupt void software_trap56 (void) {for(;;);}
__interrupt void software_trap58 (void) {for(;;);}
__interrupt void software_trap5A (void) {for(;;);}
__interrupt void software_trap5C (void) {for(;;);}
__interrupt void software_trap5E (void) {for(;;);}
__interrupt void software_trap60 (void) {for(;;);}
__interrupt void software_trap62 (void) {for(;;);}
__interrupt void software_trap64 (void) {for(;;);}
__interrupt void software_trap66 (void) {for(;;);}
__interrupt void software_trap68 (void) {for(;;);}
__interrupt void software_trap6A (void) {for(;;);}
__interrupt void software_trap6C (void) {for(;;);}
__interrupt void software_trap6E (void) {for(;;);}
__interrupt void software_trap70 (void) {for(;;);}
__interrupt void software_trap72 (void) {for(;;);}
__interrupt void software_trap74 (void) {for(;;);}
__interrupt void software_trap76 (void) {for(;;);}
__interrupt void software_trap78 (void) {for(;;);}
__interrupt void software_trap7A (void) {for(;;);}
__interrupt void software_trap7C (void) {for(;;);}
__interrupt void software_trap7E (void) {for(;;);}
__interrupt void software_trap80 (void) {for(;;);}
__interrupt void software_trap82 (void) {for(;;);}
__interrupt void software_trap84 (void) {for(;;);}
__interrupt void software_trap86 (void) {for(;;);}
__interrupt void software_trap88 (void) {for(;;);}
__interrupt void software_trap8A (void) {for(;;);}
__interrupt void software_trap8C (void) {for(;;);}
__interrupt void software_trap8E (void) {for(;;);}
__interrupt void software_trap90 (void) {for(;;);}
__interrupt void software_trap92 (void) {for(;;);}
__interrupt void software_trap94 (void) {for(;;);}
__interrupt void software_trap96 (void) {for(;;);}
__interrupt void software_trap98 (void) {for(;;);}
__interrupt void software_trap9A (void) {for(;;);}
__interrupt void software_trap9C (void) {for(;;);}
__interrupt void software_trap9E (void) {for(;;);}
__interrupt void software_trapA0 (void) {for(;;);}
__interrupt void software_trapA2 (void) {for(;;);}
__interrupt void software_trapA4 (void) {for(;;);}
__interrupt void software_trapA6 (void) {for(;;);}
__interrupt void software_trapA8 (void) {for(;;);}
__interrupt void software_trapAA (void) {for(;;);}
__interrupt void software_trapAC (void) {for(;;);}
__interrupt void software_trapAE (void) {for(;;);}
__interrupt void software_trapB0 (void) {for(;;);}
__interrupt void software_trapB2 (void) {for(;;);}
__interrupt void software_trapB4 (void) {for(;;);}
__interrupt void software_trapB6 (void) {for(;;);}
__interrupt void software_trapB8 (void) {for(;;);}
__interrupt void software_trapBA (void) {for(;;);}
__interrupt void software_trapBC (void) {for(;;);}
__interrupt void software_trapBE (void) {for(;;);}
__interrupt void software_trapC0 (void) {for(;;);}
__interrupt void software_trapC2 (void) {for(;;);}
__interrupt void software_trapC4 (void) {for(;;);}
__interrupt void software_trapC6 (void) {for(;;);}
__interrupt void software_trapC8 (void) {for(;;);}
__interrupt void software_trapCA (void) {for(;;);}
__interrupt void software_trapCC (void) {for(;;);}
__interrupt void software_trapCE (void) {for(;;);}
__interrupt void software_trapD0 (void) {for(;;);}
__interrupt void software_trapD2 (void) {for(;;);}
__interrupt void software_trapD4 (void) {for(;;);}
__interrupt void software_trapD6 (void) {for(;;);}
__interrupt void software_trapD8 (void) {for(;;);}
__interrupt void software_trapDA (void) {for(;;);}
__interrupt void software_trapDC (void) {for(;;);}
__interrupt void software_trapDE (void) {for(;;);}
__interrupt void software_trapE0 (void) {for(;;);}
__interrupt void software_trapE2 (void) {for(;;);}
__interrupt void software_trapE4 (void) {for(;;);}
__interrupt void software_trapE6 (void) {for(;;);}
__interrupt void software_trapE8 (void) {for(;;);}
__interrupt void software_trapEA (void) {for(;;);}
__interrupt void software_trapEC (void) {for(;;);}
__interrupt void software_trapEE (void) {for(;;);}
__interrupt void software_trapF0 (void) {for(;;);}
__interrupt void software_trapF2 (void) {for(;;);}
__interrupt void software_trapF4 (void) {for(;;);}
__interrupt void software_trapF6 (void) {for(;;);}
__interrupt void software_trapF8 (void) {for(;;);}
__interrupt void software_trapFA (void) {for(;;);}
__interrupt void software_trapFC (void) {for(;;);}
__interrupt void software_trapFE (void) {for(;;);}

#pragma CODE_SEG DEFAULT   

/*
************************************************************************
*	              INTERRUPT VECTOR TABLE 
*
* Notes : Modify as required
************************************************************************
*/

typedef void (*near tIsrFunc)(void);
const tIsrFunc _vect[] @0xC710 = {
        software_trap10,               /* 0x10: Surious Interrupt                      */
        software_trap12,               /* 0x12: System Call Interrupt                  */
        software_trap14,               /* 0x14: MPU Access Error                       */
        software_trap16,               /* 0x16: XGATE software error                   */                        

        software_trap18,               /* 0x18: Reserved                               */
        software_trap1A,               /* 0x1A: Reserved                               */
        software_trap1C,               /* 0x1C: Reserved                               */
        software_trap1E,               /* 0x1E: Reserved                               */
        software_trap20,               /* 0x20: Reserved                               */
        software_trap22,               /* 0x22: Reserved                               */
        software_trap24,               /* 0x24: Reserved                               */
        software_trap26,               /* 0x26: Reserved                               */
        software_trap28,               /* 0x28: Reserved                               */
        software_trap2A,               /* 0x2A: Reserved                               */
        software_trap2C,               /* 0x2C: Reserved                               */
        software_trap2E,               /* 0x2E: Reserved                               */
        software_trap30,               /* 0x30: Reserved                               */
        software_trap32,               /* 0x32: Reserved                               */
        software_trap34,               /* 0x34: Reserved                               */
        software_trap36,               /* 0x36: Reserved                               */
        software_trap38,               /* 0x38: Reserved                               */                                                                                                                                
        software_trap3A,               /* 0x3A: Reserved                               */
        
        software_trap3C,               /* 0x3C: ATD1 Compare Interrupt                 */
        software_trap3E,               /* 0x3E: ATD0 Compare Interrupt                 */

        software_trap40,               /* 0x40: TIM Pulse Accumulator A Overflow       */
        software_trap42,               /* 0x42: TIM Pulse Accumulator Input Edge       */
        software_trap44,               /* 0x44: TIM Timer overflow                     */
        software_trap46,               /* 0x46: TIM Timer channel 7                    */
        software_trap48,               /* 0x48: TIM Timer channel 6                    */
        software_trap4A,               /* 0x4A: TIM Timer channel 5                    */
        software_trap4C,               /* 0x4C: TIM Timer channel 4                    */
        software_trap4E,               /* 0x4E: TIM Timer channel 3                    */                                
        software_trap50,               /* 0x50: TIM Timer channel 2                    */
        software_trap52,               /* 0x52: TIM Timer channel 1                    */
        software_trap54,               /* 0x54: TIM Timer channel 0                    */
        
        software_trap56,               /* 0x56: SCI7                                   */
        software_trap58,               /* 0x58: Periodic timer channel 7               */
        software_trap5A,               /* 0x5A: Periodic timer channel 6               */
        software_trap5C,               /* 0x5C: Periodic timer channel 5               */
        software_trap5E,               /* 0x5E: Periodic timer channel 4               */
        software_trap60,               /* 0x60: Reserved                               */
        software_trap62,               /* 0x62: Reserved                               */
        software_trap64,               /* 0x64: XGATE software trigger 7               */
        software_trap66,               /* 0x66: XGATE software trigger 6               */
        software_trap68,               /* 0x68: XGATE software trigger 5               */
        software_trap6A,               /* 0x6A: XGATE software trigger 4               */
        software_trap6C,               /* 0x6C: XGATE software trigger 3               */
        software_trap6E,               /* 0x6E: XGATE software trigger 2               */
        software_trap70,               /* 0x70: XGATE software trigger 1               */
        software_trap72,               /* 0x72: XGATE software trigger 0               */  
              
        software_trap74,               /* 0x74: Periodic timer channel 3               */
        software_trap76,               /* 0x76: Periodic timer channel 2               */
        software_trap78,               /* 0x78: Periodic timer channel 1               */ 
        software_trap7A,               /* 0x7A: Periodic timer channel 0               */
        
        software_trap7C,               /* 0x7C: High Temperature Interrupt             */
        software_trap7E,               /* 0x7E: Autonomous periodical interrupt (API)  */
        software_trap80,               /* 0x80: Low-voltage interrupt (LVI)            */        
        software_trap82,               /* 0x82: IIC1 Bus                               */
        software_trap84,               /* 0x84: SCI5                                   */
        software_trap86,               /* 0x86: SCI4                                   */
        software_trap88,               /* 0x88: SCI3                                   */        
        software_trap8A,               /* 0x8A: SCI2                                   */
        software_trap8C,               /* 0x8C: PWM emergency shutdown                 */
        software_trap8E,               /* 0x8E: Port P Interrupt                       */
        
        software_trap90,               /* 0x90: CAN 4 transmit                         */        
        software_trap92,               /* 0x92: CAN 4 receive                          */
        software_trap94,               /* 0x94: CAN 4 errors                           */
        software_trap96,               /* 0x96: CAN 4 wake-up                          */
        software_trap98,               /* 0x98: CAN 3 transmit                         */        
        software_trap9A,               /* 0x9A: CAN 3 receive                          */
        software_trap9C,               /* 0x9C: CAN 3 errors                           */
        software_trap9E,               /* 0x9E: CAN 3 wake-up                          */
        software_trapA0,               /* 0xA0: CAN 2 transmit                         */        
        software_trapA2,               /* 0xA2: CAN 2 receive                          */
        software_trapA4,               /* 0xA4: CAN 2 errors                           */
        software_trapA6,               /* 0xA6: CAN 2 wake-up                          */
        software_trapA8,               /* 0xA8: CAN 1 transmit                         */        
        software_trapAA,               /* 0xAA: CAN 1 receive                          */
        software_trapAC,               /* 0xAC: CAN 1 errors                           */
        software_trapAE,               /* 0xAE: CAN 1 wake-up                          */
        software_trapB0,               /* 0xB0: CAN 0 transmit                         */        
        software_trapB2,               /* 0xB2: CAN 0 receive                          */
        software_trapB4,               /* 0xB4: CAN 0 errors                           */
        software_trapB6,               /* 0xB6: CAN 0 wake-up                          */                        
        
        software_trapB8,               /* 0xB8: Flash                                  */
        software_trapBA,               /* 0xBA: FLASH Fault Detect                     */
        software_trapBC,               /* 0xBC: SPI2                                   */
        software_trapBE,               /* 0xBE: SPI1                                   */
        software_trapC0,               /* 0xC0: IIC0                                   */
        software_trapC2,               /* 0xC2: SCI6                                   */
        software_trapC4,               /* 0xC4: CRG Self Clock Mode                    */
        software_trapC6,               /* 0xC6: CRG PLL Lock                           */
        software_trapC8,               /* 0xC8: Pulse Accumulator B Overflow           */
        software_trapCA,               /* 0xCA: Modulus Counter underflow              */
        software_trapCC,               /* 0xCC: Port H                                 */
        software_trapCE,               /* 0xCE: Port J                                 */
        software_trapD0,               /* 0xD0: ATD1                                   */
        software_trapD2,               /* 0xD2: ATD0                                   */
        software_trapD4,            /* 0xD4: SC11                                   */
        software_trapD6,               /* 0xD6: SCI0                                   */                              
        software_trapD8,               /* 0xD8: SPI0                                   */
        software_trapDA,               /* 0xDA: Pulse Accumulator Input Edge           */
        software_trapDC,               /* 0xDC: Pulse Accumulator A Overflow           */
        
        software_trapDE,               /* 0xDE: Standard Timer Overflow                */

        software_trapE0,                     /* 0xE0: Standard Timer Channel 7. Generally    */
                                       /* used for OS Tick interrupt and uC/Probe      */

        software_trapE2,               /* 0xE2: Standard Timer Channel 6               */
        software_trapE4,               /* 0xE4: Standard Timer Channel 5               */
        software_trapE6,               /* 0xE6: Standard Timer Channel 4               */
        software_trapE8,               /* 0xE8: Standard Timer Channel 3               */        
        software_trapEA,               /* 0xEA: Standard Timer Channel 2               */
        software_trapEC,               /* 0xEC: Standard Timer Channel 1               */
        software_trapEE,               /* 0xEE: Standard Timer Channel 0               */ 
                       
        RTI_ISR,               /* 0xF0: Real-Time Interrupt                    */
        software_trapF2,               /* 0xF2: IRQ                                    */
        software_trapF4,               /* 0xF4: XIRQ                                   */

						     
        software_trapF6,                       /* 0xF6: SWI - Used for OS Context Switch       */

        software_trapF8,               /* 0xF8: Unimplemented Instruction Trap         */
        software_trapFA,               /* 0xFA: COP Watchdog Timeout                   */
        software_trapFC,               /* 0xFC: Clock Monitor Fail                     */
        
        _Startup                       /* 0xFE: Reset                                  */
   };																			      



