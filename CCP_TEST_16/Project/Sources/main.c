#include "Configure.h"
#include "boot_can.h"
#include "ccppar.h"              
#include "ccp.h"
#include "ccp_can_interface.h"


unsigned char send_buffer [8];
unsigned char receive_buffer [8];

unsigned char Cal_var_1_byte=10;
unsigned long Cal_var_2_long=20;
unsigned char Cal_var_3_byte=30;
byte Timer_Counter_1=0,Timer_Counter_2=0;

word Err;

//void timer_init(void);

int  u32_time_cnt;

void delay_100us(int time) {
int j=0;
  for(time;time>0;time--)
  for(j=0;j<132;j++);
}

//====================中断函数================================== 
#pragma CODE_SEG __NEAR_SEG NON_BANKED 
void interrupt 7 RTI_ISR(void)     // 32.75ms timer overflow 
{ 	     
  ++u32_time_cnt;
  
  if(u32_time_cnt == 10)
  
  {
     PORTB_PB0 = !PORTB_PB0;
     u32_time_cnt= 0;
  }

                               
  CRGFLG|=0X80;//Write 1 to clear RTIF bit   
}


void RTI_Init(void) 
{                                           
    RTICTL=0x77;   //8x2^16 =>32,75ms,30.5175Hz                     

    CRGINT=0X80;   //enable RTI Interrupt 
}


void Port_Init(void)
{   
    DDRA = 0xff;  //LCD1100,PA0--4,PA67 D1D2
    PORTA= 0x00;   
       
    DDRB = 0xff;  //LED  PTB0--7,    led init
    PORTB= 0xff;  //LEDs off  
   
    DDRE = 0xFF;  //MOTOR CONTROL
    PORTE= 0x00;  //P      
    
    DDRH = 0x00; // PORTH input
    PTIH = 0X00; // KEY,PH0--5
    PERH = 0xff; // PORTH pull up  
    PPSH = 0x00; // Port H Polarity Select Register-falling edge
    PIEH = 0x02; // PORTH interrut disable but 1,                 
    
    DDRJ = 0X01; // PJ0判断行同步脉冲到达
    //PPSJ = 0x01; // Port J Polarity Select Register-rising EDGE 
    PPSJ = 0x00; // Port J Polarity Select Register-falling EDGE   
    PIEJ = 0X00; // VIDEO SYNC INTERRUPT DISABLED,BUT NOT IN MAIN()
    PERJ = 0xff;
    
    DDRP = 0xff;
    PERP = 0xff;
    PTP_PTP0 = 0;
}


void PLL_Init(void)
{
 
    CLKSEL=0X00;				// disengage PLL to system
    PLLCTL_PLLON=1;			// turn on PLL
    SYNR=0x00 | 0x01; 	// VCOFRQ[7:6];SYNDIV[5:0]
                        // fVCO= 2*fOSC*(SYNDIV + 1)/(REFDIV + 1)
                        // fPLL= fVCO/(2 × POSTDIV) 
                        // fBUS= fPLL/2 
                        // VCOCLK Frequency Ranges  VCOFRQ[7:6]
                        // 32MHz <= fVCO <= 48MHz    00
                        // 48MHz <  fVCO <= 80MHz    01
                        // Reserved                  10
                        // 80MHz <  fVCO <= 120MHz   11				
    REFDV=0x80 | 0x01;  // REFFRQ[7:6];REFDIV[5:0]
                        // fREF=fOSC/(REFDIV + 1)
                        // REFCLK Frequency Ranges  REFFRQ[7:6]
                        // 1MHz <= fREF <=  2MHz        00
                        // 2MHz <  fREF <=  6MHz       01
                        // 6MHz <  fREF <= 12MHz       10
                        // fREF >  12MHz               11                         
                        // pllclock=2*osc*(1+SYNR)/(1+REFDV)=32MHz;
    POSTDIV=0x00;       // 4:0, fPLL= fVCO/(2xPOSTDIV)
                        // If POSTDIV = $00 then fPLL is identical to fVCO (divide by one).
    _asm(nop);          // BUS CLOCK=16M
    _asm(nop);
    while(!(CRGFLG_LOCK==1));	  //when pll is steady ,then use it;
    CLKSEL_PLLSEL =1;		        //engage PLL to system; 

}
 

void main () 
{   
  Port_Init();
  PLL_Init();                                                                     
     
  DisableInterrupts;
  ccpBootInit(0x100,0x101);
  ccpInit();
   
  RTI_Init(); //改为RTI
   
  
  EnableInterrupts;
  
  Cal_var_1_byte=10;
  Cal_var_2_long=20;
  Cal_var_3_byte=30;
   
    while(1) 
    {
    
      if(ccpBootReceiveCro(receive_buffer)) 
      { 
        // if receive buffer full
        ccpCommand(receive_buffer); // call ccpCommand
      }
      

      CCP_DISABLE_INTERRUPT; // Disable Interrupts
     
      
      if (ccpBootTransmitCrmPossible()) 
      { 
        // if new transmission is possible
        ccpSendCallBack(); // call SendCallBack
      }
      
      
      CCP_ENABLE_INTERRUPT; // Enable Interrupts

    }        
}


//这里没有中断没有进去 
/*

void timer_init(void)
{
  TIM_TSCR1=0x90;

  TIM_TSCR2=0x07;
  TIM_TIE=0x01;
  TIM_TIOS_IOS0=1;

}
#pragma CODE_SEG NON_BANKED



#pragma CODE_SEG DEFAULT
#pragma CODE_SEG NON_BANKED
void interrupt 11 timer_10ms(void) 
{
 // DisableInterrupts;
 // TIM_TC0=TIM_TCNT+625;
 // EnableInterrupts;
  
  
  ++u32_time_cnt;
  if(u32_time_cnt == 10)
  
  {
     PORTB_PB0 = !PORTB_PB0;
     u32_time_cnt= 0;
  }
  
  
  Timer_Counter_1++;
  if (Timer_Counter_1>19){
    ccpDaq(1);    
    Timer_Counter_1=0;
  }
  
  Timer_Counter_2++;
  if (Timer_Counter_2>19){
    ccpDaq(2);    
    Timer_Counter_2=0;
  }
    
  
  
}
#pragma CODE_SEG DEFAULT

 */
