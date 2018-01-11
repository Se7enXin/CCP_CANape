/*=======================================================================
**     KPB03 开发库定义文件
**     文件名    : PIT.c
**     工程名    : T5644ATest
**     系统平台  : BMS32
**     编译平台  : CodeWarrior IDE version 2.8
**     创建日期  : 2012-9-27 11:02:27
**     简介      : PIT模块操作
**     版权      : (c) Copyright KeyPower, 2012
**     开发者    : 张鹏 
**     联系方式  : peng.zhang@key-power.com.cn
**     历史版本  :
**                 	V0.1 2012-9-27  实现基本功能 
=======================================================================*/

#include "MPC5644A.h"
#include "IntcInterrupts.h"     /* Implement functions from this file */
#include "ZTypes.h"
#include "PIT.h"
//#include "includes.h"


/*
** ===================================================================
**	类型声明
** ===================================================================
*/
typedef void (*ISRTYPE)(void);
/*
** ===================================================================
**	函数声明
** ===================================================================
*/
void PIT_ISR0(void);
void PIT_ISR1(void);
void PIT_ISR2(void);
void PIT_ISR3(void);
void PIT_ISR(UINT_8 channel);

/*
** ===================================================================
**	PIT模块中断向量函数地址表
** ===================================================================
*/
void*PIT_ISR_ARR[4]={PIT_ISR0,PIT_ISR1,PIT_ISR2,PIT_ISR3};
void*PIT_ISR_HOOK[4]={0,0,0,0};


/*
** ===================================================================
**	名称:PIT_ISRX
**	参数:	
**  功能:  PIT中断服务程序
**  返回值: 
**			...
**	注意:	
** ===================================================================
*/
void PIT_ISR0(void)
{
	PIT_ISR(0);
}
void PIT_ISR1(void)
{
	PIT_ISR(1);
}
void PIT_ISR2(void)
{
	PIT_ISR(2);
}
void PIT_ISR3(void)
{
	PIT_ISR(3);
}
void PIT_ISR(UINT_8 channel)
{
	if(PIT_ISR_HOOK[channel]!=0)
		(*((ISRTYPE)PIT_ISR_HOOK[channel]))();
	PIT.TIMER[channel].TFLG.B.TIF=1;	//ISR end
}
/*
** ===================================================================
**	名称:InitPIT
**	参数:	
**  功能:  初始化PIT中断服务程序
**  返回值: 
**			...
**	注意:	必须在初始化后才可使用PIT其它函数
** ===================================================================
*/
void InitPIT(void)
{
	UINT_8 channel=0;
	while(channel<4)
	{
		PIT.TIMER[channel].TCTRL.B.TIE = 0;
		PIT.TIMER[channel].TCTRL.B.TEN = 0;
		PIT.TIMER[channel].TFLG.B.TIF=1;	//Clear flag
		channel++;	
	}


	INTC_InstallINTCInterruptHandler(PIT_ISR_ARR[0],301,15);
	INTC_InstallINTCInterruptHandler(PIT_ISR_ARR[1],302,15);
	INTC_InstallINTCInterruptHandler(PIT_ISR_ARR[2],303,15);
	INTC_InstallINTCInterruptHandler(PIT_ISR_ARR[3],304,15);
	
}

/*
** ===================================================================
**	名称:SetPITHook
**	参数:	UINT_8 channel		通道
**									0  PIT通道0 
**									1  PIT通道1 
**									2  PIT通道2
**									3  PIT通道3 
**			void*	func		回调函数			
**										
**  功能:  设置PIT回调函数函数类型是void XXXFUNC(void);
**  返回值: 
**			...
**	注意:	
** ===================================================================
*/

void SetPITHook(UINT_8 channel,void* func)
{
	PIT_ISR_HOOK[channel] = func;
}

/*
** ===================================================================
**	名称:SetPIT
**	参数:	UINT_8 channel		通道
**									0  PIT通道0 
**									1  PIT通道1 
**									2  PIT通道2
**									3  PIT通道3 
**			UINT_32	time		PIT周期时间 单位us	
**									0 关闭该通道
**									1000,0000	限制到10s		
**										
**  功能:  设置PIT中断时间
**  返回值: 
**			...
**	注意:	
** ===================================================================
*/
UINT_8 SetPIT(UINT_8 channel,UINT_32 time)
{
	PIT.TIMER[channel].TCTRL.B.TIE = 0;
	PIT.TIMER[channel].TCTRL.B.TEN = 0;
	PIT.TIMER[channel].TFLG.B.TIF=1;	//Clear flag
	
	if(time==0)return ERROR_PIT_OK;
	if(time>10000000UL)return ERROR_PIT_TIMERANGE;
	PIT.TIMER[channel].LDVAL.R = time*SYSTEM_CLOCK;
	
	PIT.TIMER[channel].TCTRL.B.TIE = 1;
	PIT.TIMER[channel].TCTRL.B.TEN = 1;
	return ERROR_PIT_OK;
}
