#CCP移植（MPC5644A_32位 & S12XEP100_16位）

##1、和CCP移植相关的资料
	Vector所提供的资料中有几个文档，下图中画红色方框的需要在移植之前看完。
![](http://i.imgur.com/hLbaOeq.jpg)  
		
	AN-IMC-1-001_Integration_of the_Vector_CCP_Driver_with_a_free_CAN_Driver.pdf  介绍了完整的移植过程  
	CCP21.pdf  介绍了2.1版本的CCP协议

##2、CCP文件介绍
移植后的工程中应该包含以下文件：  
![](http://i.imgur.com/1ksZHOC.jpg)  

文件名称|描述
------|--------
ccp.c |Vector提供的CCP Driver的源代码
ccp.h |CCP Driver的头文件
ccppar.h|CCP Driver的配置文件
ccp_can_interface.c|CCP Driver的接口函数
ccp_can_interface.h|CCP Driver接口函数的头文件
boot_can.c|CAN的底层驱动
boot_can.h|CAN底层驱动的头文件

<font color= "red">其中***ccp.c、ccp.h、ccppar.h***由Vector提供，我们不需要进行改动，而其余的文件需要我们针对具体的芯片来进行改写。</font>

##3、CCP Driver在ECU上的集成
###(1)ccppar.h
<font color = "blue">CCP Driver数据类型的声明（针对不同硬件平台需要进行改动）：</font>  

	/*----------------------------------------------------------------------------*/
	/* Platform independant types */	
	#define BYTE    unsigned char	
	#define WORD    unsigned short
	#define DWORD   unsigned long
	#define BYTEPTR unsigned char*
	#define MTABYTEPTR BYTEPTR
	#define ROM
	#define RAM	

<font color = "blue">CCP开关中断宏定义，具体的实现在ccp_can_interface.c中：</font>  

	#define CCP_DISABLE_INTERRUPT disable_interrupt();
	#define CCP_ENABLE_INTERRUPT  enable_interrupt();

<font color = "blue">ECU Address、DTO_ID、CRO_ID：</font>  

	/* CCP Identifiers and Address */
	#define CCP_STATION_ADDR  0x0000      /* Define CCP_STATION_ADDR in Intel Format */
	                                      /* High/Low byte swapped on motorola machines !!! (0x3900) */
	#define CCP_STATION_ID    "Ccptest080709"  /* Plug&Play station identification */
	#define CCP_DTO_ID        0x101         /* CAN identifier ECU -> Master */
	#define CCP_CRO_ID        0x100         /* CAN identifier Master -> ECU */

###(2)ccp_can_interface.c
<font color = "red">该文件实现对底层CAN驱动的封装：</font>  

<font color = "blue">CAN发送：</font>
 
	// -----------------------------------------------------------------------------
	// SENDING an CRM-DTO when receiving an CRO
	// -----------------------------------------------------------------------------
	BYTE ccpSend( BYTEPTR msg )
	{
	  ccpBootTransmitCrm(msg);              // callup CAN-send
	  return 1;
	}
	
-----	
	// -----------------------------------------------------------------------------
	// CONVERT pointer
	// -----------------------------------------------------------------------------
	MTABYTEPTR ccpGetPointer( BYTE addr_ext, DWORD addr )        // get Pointer into
	{                                                            // normal C
	  return (MTABYTEPTR) addr;
	}
	// -----------------------------------------------------------------------------
	// -----------------------------------------------------------------------------
	// CALCULATE new measuerments
	// -----------------------------------------------------------------------------
	void ccpUserBackground( void )
	{
	}
	// -----------------------------------------------------------------------------
	// -----------------------------------------------------------------------------
	// Define
	// -----------------------------------------------------------------------------
	BYTE ccpDisableNormalOperation( MTABYTEPTR a, WORD s )
	{
	  disable_interrupt();                  // disables all interrupts, e.g. TIMER_0
	  return 1;
	}
	// -----------------------------------------------------------------------------
	
<font color = "blue">开关中断：</font>	

	VUINT_32 cur_func_pri;
	// -----------------------------------------------------------------------------
	// Disable/Enable Interrupt Functions
	// -----------------------------------------------------------------------------
	BYTE disable_interrupt_count = 0;            // init counter
	
	void disable_interrupt() {
	   cur_func_pri = INTC.CPR.B.PRI;            //通过改变优先级来禁止中断，先保存当前任务优先级，再把该任务优先级设为15（最高）
       INTC.CPR.B.PRI = 15;                      // set I-Bit
	   disable_interrupt_count++;                // incr. counter
	}
	
	void enable_interrupt() {
	  if (--disable_interrupt_count==0) {        // if interrupts only one time disabled
	    INTC.CPR.B.PRI = cur_func_pri;           //恢复该任务优先级            // enable them
	  }
	}
	// -----------------------------------------------------------------------------
	
###(3)boot_can.c
<font color = "red">该文件中需要完成4个CAN接口函数，也是CCP中CAN模块的底层驱动</font> 
 
<font color = "blue">CAN模块初始化：</font>	
	
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

<font color = "blue">判断CAN是否可以发送：</font>
	
	// -----------------------------------------------------------------------------
	// CAN TRANSMIT (Data Frame)
	// -----------------------------------------------------------------------------
	int ccpBootTransmitCrmPossible( void ) {
	
	  return ((CAN0TFLG_TXE0) == 1);        // return 1 if so
	}

<font color = "blue">CAN发送：</font>
	
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

<font color = "blue">CAN接收：</font>
		
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