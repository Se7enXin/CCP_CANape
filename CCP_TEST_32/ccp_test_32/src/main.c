#include "ccp_includes.h"
//#include "Configure.h"
#include "boot_can.h"
#include "ccppar.h"              
#include "ccp.h"
#include "ccp_can_interface.h"



unsigned char receive_buffer [8];

unsigned char Cal_var_1_byte=10;
unsigned long Cal_var_2_long=20;
unsigned char Cal_var_3_byte=30;

UINT_8	Timer_Counter_1=0,Timer_Counter_2=0;
/*
** ===================================================================
**	名称:Init LED
**	参数:
**  功能:初始化LED驱动通道
**  返回值: 
**	注意:
** ===================================================================
*/
void InitLED()
{
	SIU.PCR[110].B.PA=0;
	SIU.PCR[110].B.OBE=1;
	SIU.GPDO[110].B.PDO=0;
}
/*
** ===================================================================
**	名称:Set LED
**	参数:
**  功能:设置LED驱动通道
**  返回值: 
**	注意:
** ===================================================================
*/
void SetLED(UINT_8 state)
{
	SIU.GPDO[110].B.PDO=state;
}

void delay_100us(int time) 
{
	int j=0;
	for(time;time>0;time--)
    	for(j=0;j<132;j++);
}

/*
** ===================================================================
**	名称:InitFMPLL
**	参数:	
**  功能:  初始化FMPLL
**  返回值: 
**			...
**	注意:	
** ===================================================================
*/
void InitFMPLL()
{

	FLASH_A.BIUCR.R = 0x00009413;
	FLASH_B.BIUCR.R = 0x00009413;
	FLASH_A.BIUCR2.B.LBCFG = 0;
	FLASH_B.BIUCR2.B.LBCFG = 0;

	FMPLL.ESYNCR1.B.EMODE=1;
	FMPLL.ESYNCR1.B.EPREDIV =1;//1
	FMPLL.ESYNCR1.B.EMFD = 60;       //SysClock Frequency=120MHz	
	FMPLL.ESYNCR2.B.ERFD =1;//1  		 

	while (FMPLL.SYNSR.B.LOCK != 1) //判断PLL是否稳定
	{
		//FMPLL没有上锁，说明时钟频率不正确;
	}
	FMPLL.ESYNCR1.B.CLKCFG=7;	

}


void InitSTM()
{
	STM.CR.B.CPS = 119;					//120Mhz /120=1Mhz
	STM.CR.B.TEN = 1;					//Enable
}


UINT_8 can_data[8] = {1,2,3,4,5,6,7,8};

void SetCurINTPRI(UINT_8 pri)
{
	INTC.CPR.B.PRI = pri;
}
	
void Task1()
{

	//ccpBootTransmitCrm(can_data);	
	Timer_Counter_1++;	
	if(Timer_Counter_1 >= 10)
	{
		ccpDaq(1);    
    	Timer_Counter_1=0;			
	}
	
	Timer_Counter_2++;
	if(Timer_Counter_2 >= 100)
	{
		ccpDaq(2);    
    	Timer_Counter_2=0;			
	}
	
}

int main(void) 
{
	int led_flag = 0;
	
    InitFMPLL();							//设置总线频率到120Mhz
	InitSTM();
	
    InitLED();
    
    INTC.CPR.B.PRI = 0;				//make current interrupt priority lowest,let other ISR can run
    InitPIT();
    
    ccpBootInit(0x100,0x101);
    ccpInit();
    	
    Cal_var_1_byte=10;
    Cal_var_2_long=20;
    Cal_var_3_byte=30;
      
    SetCurINTPRI(0);   //改变优先级，这里为什么要加？不加的结果是CAN可以发送，但接收不到。
    
    SetPITHook(1,&Task1);
    

    SetPIT(1,1000);   //us
           
    while(1) 
    {
      if(ccpBootReceiveCro(receive_buffer)) 
      {
      	if(led_flag == 0) led_flag = 1;
      	else led_flag = 0;
        SetLED(led_flag);  //1 light on,0 light off
        
   	 //	ccpBootTransmitCrm(can_data);
        // if receive buffer full
        ccpCommand(receive_buffer); // call ccpCommand    
      }

      CCP_DISABLE_INTERRUPT; // Disable Interrupts
      
      if (ccpBootTransmitCrmPossible()) 
      { 
      	//SetLED(1);
        // if new transmission is possible
        ccpSendCallBack(); // call SendCallBack
      }
            
      CCP_ENABLE_INTERRUPT; // Enable Interrupts
      
    } 
  
  
}



