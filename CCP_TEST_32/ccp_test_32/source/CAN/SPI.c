/*=======================================================================
**     KPV13 开发库定义文件
**     文件名    : SPI.c
**     工程名    : T5644ATest
**     系统平台  : KPV13
**     编译平台  : CodeWarrior IDE version 2.8
**     创建日期  : 2012-9-28 17:02:25
**     简介      : SPI模块操作
**     版权      : (c) Copyright KeyPower, 2012
**     开发者    : 张鹏 
**     联系方式  : peng.zhang@key-power.com.cn
**     历史版本  :
**                 	V0.1 2012-9-28  实现基本功能 
=======================================================================*/
#include "MPC5644A.h"
#include "IntcInterrupts.h"     /* Implement functions from this file */
#include "ZTypes.h"
#include "SPI.h"
typedef	union _tCTAR{
        vuint32_t R;
	    struct _B{
		vuint32_t DBR:1;
		vuint32_t FMSZ:4;
		vuint32_t CPOL:1;
		vuint32_t CPHA:1;
		vuint32_t LSBFE:1;
		vuint32_t PCSSCK:2;
		vuint32_t PASC:2;
		vuint32_t PDT:2;
		vuint32_t PBR:2;
		vuint32_t CSSCK:4;
		vuint32_t ASC:4;
		vuint32_t DT:4;
		vuint32_t BR:4;
		}B;
} tCTAR;              /* Clock and Transfer Attributes Registers @baseaddress + 0x0C - 0x28 */

#define ERROR_SETAO_OK			0
#define ERROR_SETAO_CHNUMFAIL	1

#if 1

/*
** ===================================================================
**	名称:InitDSPIB
**	参数:	无
**  功能:	初始化SPIB
**  返回值: 
**			待添加
**	注意:
** ===================================================================
*/

void InitDSPIB()
{
	tCTAR reg_ctar;									

	DSPI_B.MCR.B.MSTR	=1;				//SPI MASTER	
	DSPI_B.MCR.B.DCONF	=0;				//SPI selected
	DSPI_B.MCR.B.FRZ	=0;				//Debug Mode not halt transmit
	DSPI_B.MCR.B.PCSIS0	=1;				//PCS0 active low
	DSPI_B.MCR.B.PCSIS1	=1;				//PCS1 active low
	DSPI_B.MCR.B.PCSIS2	=1;				//PCS2 active low
	DSPI_B.MCR.B.PCSIS3	=1;				//PCS3 active low
	DSPI_B.MCR.B.PCSIS4	=1;				//PCS4 active low
	DSPI_B.MCR.B.MTFE	=0;				
	DSPI_B.MCR.B.PCSSE	=0;
	DSPI_B.MCR.B.ROOE	=0;
	DSPI_B.MCR.B.DOZE	=0;
	DSPI_B.MCR.B.DIS_TXF=0;
	DSPI_B.MCR.B.DIS_RXF=0;
	DSPI_B.MCR.B.CLR_TXF=0;
	DSPI_B.MCR.B.CLR_RXF=0;
	DSPI_B.MCR.B.SMPL_PT=2;
	
	DSPI_B.MCR.B.HALT	=1;				//HLAT set before CTAR'S init								
	
	reg_ctar.B.DBR =1;
	reg_ctar.B.FMSZ=0x7;
	reg_ctar.B.CPOL=0x0;
	reg_ctar.B.CPHA=0x0;
	reg_ctar.B.LSBFE=0x0;				//MSB first
	reg_ctar.B.PCSSCK=0x3;
	reg_ctar.B.PASC=0x3;
	reg_ctar.B.PDT=0x3;
	reg_ctar.B.PBR=0x3;
	reg_ctar.B.CSSCK=0x3;
	reg_ctar.B.ASC=0x3;
	reg_ctar.B.DT=0x3;
	reg_ctar.B.BR=0x4;

	
	DSPI_B.CTAR[0].R 	=(VUINT_32)reg_ctar.R;	//f(SCK) = 120M/(2*64)=935khz  0x780A7727UL

	reg_ctar.B.DBR =1;
	reg_ctar.B.FMSZ=0xf;
	reg_ctar.B.CPOL=0x0;
	reg_ctar.B.CPHA=0x1;
	reg_ctar.B.LSBFE=0x0;				//MSB first
	reg_ctar.B.PCSSCK=0x3;
	reg_ctar.B.PASC=0x3;
	reg_ctar.B.PDT=0x3;
	reg_ctar.B.PBR=0x3;
	reg_ctar.B.CSSCK=0x3;
	reg_ctar.B.ASC=0x3;
	reg_ctar.B.DT=0x3;
	reg_ctar.B.BR=0x4;
	
	DSPI_B.CTAR[1].R 	=(VUINT_32)reg_ctar.R;	//f(SCK) = 120M/(2*64)=935khz  0x780A7727UL

	reg_ctar.B.DBR =1;
	reg_ctar.B.FMSZ=0x7;				//frame bit size 8
	reg_ctar.B.CPOL=0x0;
	reg_ctar.B.CPHA=0x0;
	reg_ctar.B.LSBFE=0x0;				//MSB first
	reg_ctar.B.PCSSCK=0x3;
	reg_ctar.B.PASC=0x3;
	reg_ctar.B.PDT=0x3;
	reg_ctar.B.PBR=0x3;
	reg_ctar.B.CSSCK=0x3;
	reg_ctar.B.ASC=0x3;
	reg_ctar.B.DT=0x3;
	reg_ctar.B.BR=0x4;
	
	DSPI_B.CTAR[2].R 	=(VUINT_32)reg_ctar.R;	//f(SCK) = 120M/(2*64)=935khz  0x780A7727UL

	DSPI_B.MCR.B.HALT	=0;										
										
	SIU.PCR[102].R		=0x0603;		//config pin SCK
	SIU.PCR[103].R		=0x0503;		//config pin SIN
	SIU.PCR[104].R		=0x0603;		//config pin SOUT
	SIU.PCR[108].R		=0x0603;		//config pin PCS3
	SIU.PCR[106].R		=0x0603;		//config pin PCS1
	SIU.PCR[107].R		=0x0603;		//config pin PCS2
	SIU.PCR[109].R		=0x0603;		//config pin PCS4									
	SIU.PCR[105].R		=0x0603;		//config pin PCS0									
	SIU.GPDO[108].R		=0x1;			//cs high

	SIU.PCR[202].B.PA=0;
	SIU.PCR[202].B.OBE=1;
	SIU.GPDO[202].B.PDO=1;				//SC_INT 


}
/*
** ===================================================================
**	名称:DSPIB_Send
**	参数:	UINT_8*senddata		发送数据缓冲区
**			UINT_8*recvdata		接收数据缓冲区
**			UINT_16 datanum		发送/接收数据字节数
**  功能:  读取EEPROM中数据
**  返回值: 
**			待添加
**	注意:
** ===================================================================
*/
void DSPIB_Send(UINT_16*senddata,UINT_16*recvdata,UINT_16 datanum)
{
	UINT_8 sbindex=0;
	UINT_32 recive_buf;
//	SIU.GPDO[108].R		=0x0;			//cs low
	recive_buf = 0xfff;					//wait 100us
	while(recive_buf!=0)
	{
		recive_buf--;
	}
	
	while(sbindex<datanum)
	{
		DSPI_B.PUSHR.R = 0x00000000UL|senddata[sbindex]|1L<<19;
		while(DSPI_B.SR.B.TCF==0)
		{
		}

		recive_buf = DSPI_B.POPR.R;
		recvdata[sbindex] = (UINT_16)(recive_buf&0xff);
		
		DSPI_B.SR.B.TCF = 1;
		DSPI_B.SR.B.EOQF = 1;
		sbindex++;
	}
										//wait 100us
	recive_buf = 0xfff;
	while(recive_buf!=0)
	{
		recive_buf--;
	}
//	SIU.GPDO[108].R		=0x1;			//cs high
	recive_buf = 0xfff;
	while(recive_buf!=0)
	{
		recive_buf--;
	}

}
/*
** ===================================================================
**	名称:DSPIB_Send_Ch
**	参数:	UINT_8*senddata		发送数据缓冲区
**			UINT_8*recvdata		接收数据缓冲区
**			UINT_16 datanum		发送/接收数据数据单元数(根据ctar确定)
**			UINT_8 cs			片选信号 	0:AO1
**											1:AO2
**											2:UJA1075
**											3:CPLD
**											4:EEPROM
**  功能:  发送接收SPI数据
**  返回值: 
**			待添加
**	注意:
** ===================================================================
*/
void DSPIB_Send_Ch(UINT_16*senddata,UINT_16*recvdata,UINT_16 datanum,UINT_8 cs)
{
	UINT_8 sbindex=0;
	UINT_32 recive_buf;
	UINT_32 pushr;
	UINT_32 catr[6]={1UL<<28,1UL<<28,1UL<<28,0,1UL<<29|1UL<<31,1UL<<31};//[AO1(B_PCS0),AO2(B_PCS1),UJA,CPLD,EEPROM]
	VUINT_32 temp_cur_pri;
	recive_buf = 0x03f;					//wait 100us
	
	while(recive_buf!=0)
	{
		recive_buf--;
	}
	temp_cur_pri = INTC.CPR.B.PRI;			//进入临界区
	INTC.CPR.B.PRI = 15;	
	while(sbindex<datanum)
	{
		pushr = 0x00000000UL|senddata[sbindex]|1L<<(16+cs)|catr[cs];
		if(sbindex==(datanum-1))
		{
			pushr &= ~(1UL<<31);
		}
		DSPI_B.PUSHR.R =pushr;
		while(DSPI_B.SR.B.TCF==0)
		{
		}

		recive_buf = DSPI_B.POPR.R;
		recvdata[sbindex] = (UINT_16)(recive_buf&0xffff);
		
		DSPI_B.SR.B.TCF = 1;
		DSPI_B.SR.B.EOQF = 1;
		sbindex++;
	}
	INTC.CPR.B.PRI = temp_cur_pri;			//退出临界区										//wait 100us
	recive_buf = 0x01f;
	while(recive_buf!=0)
	{
		recive_buf--;
	}
	recive_buf = 0x01f;
	while(recive_buf!=0)
	{
		recive_buf--;
	}

}



UINT_16 WD_SR=0; //1
UINT_16 MC_R = 0;//3
UINT_16 INT_CR=0;//5
UINT_16 INT_SR=0;//7
void InitUJA1075A()
{
	UINT_16 senddata=0;
	UINT_16 recvdata=0;
	


	
	//write INT_SR
	senddata=0x7000;
	DSPIB_Send_Ch(&senddata,&recvdata,1,2);	
	senddata=0x6000|(recvdata&0xfffUL);
	DSPIB_Send_Ch(&senddata,&recvdata,1,2);	
	
	//write INT_CR  WIC1 int enablewrite STBCC to 1
	senddata=0x40F3;
	DSPIB_Send_Ch(&senddata,&recvdata,1,2);	
	
	//write WD_SR
	senddata=0x0F00;
	DSPIB_Send_Ch(&senddata,&recvdata,1,2);	
	
	//write MC_R
	senddata=0x2E00;
	DSPIB_Send_Ch(&senddata,&recvdata,1,2);
	senddata=0x3000;
	DSPIB_Send_Ch(&senddata,&recvdata,1,2);
	MC_R = recvdata;
	
	//Read 
	senddata=0x5000;
	DSPIB_Send_Ch(&senddata,&recvdata,1,2);
	INT_CR = recvdata;
	senddata=0x7000;
	DSPIB_Send_Ch(&senddata,&recvdata,1,2);
	INT_SR = recvdata;
	senddata=0x1000;
	DSPIB_Send_Ch(&senddata,&recvdata,1,2);
	WD_SR = recvdata;
}
void GetUJA1075A()
{
	UINT_16 senddata=0;
	UINT_16 recvdata=0;
	senddata=0x0F00;
	DSPIB_Send_Ch(&senddata,&recvdata,1,2);	
	senddata=0x3000;
	DSPIB_Send_Ch(&senddata,&recvdata,1,2);
	MC_R = recvdata;
	senddata=0x5000;
	DSPIB_Send_Ch(&senddata,&recvdata,1,2);
	INT_CR = recvdata;
	senddata=0x7000;
	DSPIB_Send_Ch(&senddata,&recvdata,1,2);
	INT_SR = recvdata;
	senddata=0x1000;
	DSPIB_Send_Ch(&senddata,&recvdata,1,2);
	WD_SR = recvdata;
}
/*
** ===================================================================
**	名称:SleepUJA1075A
**	参数:	UINT_8	wake_cfg_can CAN唤醒配置
**								 0 关闭CAN唤醒,同时CAN收发器不能使用
**								 1 打开CAN模块，使能CAN唤醒
**			UINT_8	wake_cfg_cc  CC唤醒配置
**								 0 关闭CC唤醒
**							     1 上升沿唤醒
**								 2 下降沿唤醒
**								 3  上升沿下降沿均唤醒
**			UINT_8  wake_cfg_key Key唤醒
**								 0 关闭CC唤醒
**							     1 上升沿唤醒
**								 2 下降沿唤醒
**								 3  上升沿下降沿均唤醒								  
**  功能:  设置唤醒源
**  返回值: 
**			待添加
**	注意:
** ===================================================================
*/
void SleepUJA1075A(UINT_8 wake_cfg_can,UINT_8 wake_cfg_cc,UINT_8 wake_cfg_key)
{
	/*	MC=01
		INTH=HIGH
		One wake-up enabled and no wake-up pending*/
	UINT_16 senddata=0;
	UINT_16 recvdata=0;
	
	//Clear all interrupt flag
	senddata=0x7000;
	DSPIB_Send_Ch(&senddata,&recvdata,1,2);	
	senddata=0x6000|(recvdata&0xfffUL);
	DSPIB_Send_Ch(&senddata,&recvdata,1,2);	
	
	//write STBCC to 1
	senddata=0x4000;//40fb
	if(wake_cfg_can!=0)
	{
		senddata|=0x0008;
	}
	senddata|=(wake_cfg_cc&0x3)<<6;
	senddata|=(wake_cfg_key&0x3)<<4;
	DSPIB_Send_Ch(&senddata,&recvdata,1,2);
	
	//write MC=01
	senddata=0x2400;
	DSPIB_Send_Ch(&senddata,&recvdata,1,2);	


		//set pin to INH
	SIU.PCR[219].R		=0;
	SIU.PCR[84].R		=0;
	SIU.PCR[83].R		=0;
	SIU.PCR[107].R		=0;
	SIU.PCR[102].R		=0;
	SIU.PCR[103].R		=0;
	SIU.PCR[104].R		=0;
	SIU.PCR[90].R		=0;
	SIU.PCR[89].R		=0;	
	
	
}
/*
** ===================================================================
**	名称:SetAO
**	参数:	UINT_32 channel		通道0 AO1 AO2
**			UINT_16 vol			电压值 mV 0-5000
**  功能:   设置AO输出的电压值
**  返回值: 
**			待添加
**	注意:
** ===================================================================
*/
UINT_8 SetAO(UINT_8 ch,UINT_16 vol)
{
	UINT_16 sendata;
	UINT_16 recdata;
	if(ch>1)
		return ERROR_SETAO_CHNUMFAIL;
	if(vol>5000)
		vol = 5000;
	sendata=((UINT_16)vol*4095/5000);
	DSPIB_Send_Ch(&sendata,&recdata,1,(UINT_8)ch);
	return ERROR_SETAO_OK;
}

#else

#endif