/*=======================================================================
**     KPV13 开发库定义文件
**     文件名    : CAN.c
**     工程名    : T5644ATest
**     系统平台  : KPV13
**     编译平台  : CodeWarrior IDE version 5.9.0
**     创建日期  : 2012-9-24 18:07:46
**     简介      : CAN模块操作
**     版权      : (c) Copyright KeyPower, 2012
**     开发者    : 张鹏 
**     联系方式  : peng.zhang@key-power.com.cn
**     历史版本  :
**                 	V0.1 2012-9-24  实现基本功能 
**					V0.2 2012-9-25	添加软件缓冲区,修复一些BUG
**					V0.3 2012-11-26 修正CAN 通道和接插件定义不一致问题
**					V0.4 2012-11-28 修正CAN接收扩展帧时只收到18位的问题
**					V0.5 2012-12-27 修正CANID扩展帧和标准帧不能同时接受问题
**					V0.6 2013-01-25 添加初始化超过1.42us超时
=======================================================================*/
#include "MPC5644A.h"
#include "IntcInterrupts.h"     /* Implement functions from this file */
#include "ZTypes.h"
#include "CAN.h"
//#include "SPI.h"
/*
** ===================================================================
**	函数声明
** ===================================================================
*/
void OnCANTMsg(UINT_8 channel,UINT_8 MBindex);
void OnCANRMsg(UINT_8 channel,UINT_8 MBindex);

void OnCANTMsgCAN0MB0(void);
void OnCANTMsgCAN1MB0(void);
void OnCANTMsgCAN2MB0(void);

void OnCANRMsgCAN0MB12(void);
void OnCANRMsgCAN0MB13(void);
void OnCANRMsgCAN0MB14(void);
void OnCANRMsgCAN0MB15(void);

void OnCANRMsgCAN1MB12(void);
void OnCANRMsgCAN1MB13(void);
void OnCANRMsgCAN1MB14(void);
void OnCANRMsgCAN1MB15(void);

void OnCANRMsgCAN2MB12(void);
void OnCANRMsgCAN2MB13(void);
void OnCANRMsgCAN2MB14(void);
void OnCANRMsgCAN2MB15(void);

UINT_8 SetCANFilter(UINT_8 channel,UINT_8 MBindex,UINT_8 iren,\
					UINT_32 acccode,UINT_32 mask);

UINT_8 SendCANMsgMB(UINT_8 channel,UINT_8 MBindex,UINT_8 ext,\
				UINT_8 dataframe,UINT_32 canid,UINT_8 datalen,UINT_8 *data);
UINT_8 GetCANMsgMB(UINT_8 channel,UINT_8 MBindex,UINT_8 *ext,\
				UINT_8 *dataframe,UINT_32 *canid,UINT_8 *datalen,UINT_8 *data);
/*
** ===================================================================
**	CAN模块中断向量函数地址表
** ===================================================================
*/
void*OnCANRMsgArr[3][16];
void*OnCANTMsgArr[3][16]={};
CANRCBTYPE OnCANRmsgCB[3]={0L,0L,0L};
/*
** ===================================================================
**	CAN模块中断向量表--VECTORNUM
** ===================================================================
*/
UINT_16	CAN_VECTOR_SNUM[3]={155,283,176} ;

/*
** ===================================================================
**	CAN模块发送软件缓冲区
** ===================================================================
*/
#define		CAN_TMSGBUF_NUM 							250
UINT_8 		CAN_TMSG_ext 		[3][CAN_TMSGBUF_NUM];
UINT_8 		CAN_TMSG_dataframe 	[3][CAN_TMSGBUF_NUM];
UINT_32 	CAN_TMSG_canid 		[3][CAN_TMSGBUF_NUM];
UINT_8 		CAN_TMSG_datalen 	[3][CAN_TMSGBUF_NUM];
UINT_8 		CAN_TMSG_data 		[3][CAN_TMSGBUF_NUM][8];

UINT_8 		CAN_TMSG_in	[3]	=	{0,0,0};
UINT_8 		CAN_TMSG_out[3]	=	{0,0,0};
UINT_8 		CAN_TMSG_num[3]	=	{0,0,0};

/*
** ===================================================================
**	CAN模块接收软件缓冲区
** ===================================================================
*/
#define 	CAN_MSGBUF_NUM 								40
UINT_8 		CAN_MSG_ext 		[3][CAN_MSGBUF_NUM];
UINT_8 		CAN_MSG_dataframe 	[3][CAN_MSGBUF_NUM];
UINT_32 	CAN_MSG_canid 		[3][CAN_MSGBUF_NUM];
UINT_8 		CAN_MSG_datalen 	[3][CAN_MSGBUF_NUM];
UINT_8 		CAN_MSG_data 		[3][CAN_MSGBUF_NUM][8];

UINT_8 		CAN_MSG_in	[3]	=	{0,0,0};
UINT_8 		CAN_MSG_out	[3]	=	{0,0,0};
UINT_8 		CAN_MSG_num	[3]	=	{0,0,0};

/*
** ===================================================================
**	名称:InitCAN
**	参数:	UINT_8 channel		通道
**									0  通道0 接插件{CANL0,CANH0}
**									1  通道1 接插件{CANL1,CANH1}
**									2  通道2 接插件{CANL2,CANH2}
**			UINT_32 baudrate	通道比特率
**									250000	250kbps
**									500000	500kbps
**									1000000	1Mbps
**			UINT_32 mask1		屏蔽MASK1 bit31 1标识过滤扩展帧 0不过滤扩展帧
**			UINT_32 acccode1	ACCODE1 bit31 1为扩展帧 0 标准帧
**			UINT_32 mask2		屏蔽MASK2 同上
**			UINT_32 acccode2	ACCODE2
**  功能:  设置CAN通道波特率并使能CAN通道
**				 当(canid&mask)==(acccode&mask)时接收该帧
**  返回值: 
**			ERROR_OK		设置成功
**			ERROR_TIMESOUT	初始化超时
**			待添加
**	注意:
** ===================================================================
*/

UINT_8 InitCAN(UINT_8 channel,UINT_32 baudrate,UINT_32 mask1,\
					UINT_32 acccode1,UINT_32 mask2,UINT_32 acccode2)
{
	volatile struct FLEXCAN2_tag * CAN_X;
	VUINT_32 t_timer;
	VUINT_32 message_buf_index;
	VUINT_32 times_out=10;				//10 1.42us 正常0.3us
	VUINT_32 times_out_FRZ=10000000;	//10 1.42us 正常 <<0.3us
										//MB接收中断函数
	OnCANRMsgArr[0][12] = OnCANRMsgCAN0MB12;
	OnCANRMsgArr[0][13] = OnCANRMsgCAN0MB13;
	OnCANRMsgArr[0][14] = OnCANRMsgCAN0MB14;
	OnCANRMsgArr[0][15] = OnCANRMsgCAN0MB15;
	
	OnCANRMsgArr[1][12] = OnCANRMsgCAN1MB12;
	OnCANRMsgArr[1][13] = OnCANRMsgCAN1MB13;
	OnCANRMsgArr[1][14] = OnCANRMsgCAN1MB14;
	OnCANRMsgArr[1][15] = OnCANRMsgCAN1MB15;
	
	OnCANRMsgArr[2][12] = OnCANRMsgCAN2MB12;
	OnCANRMsgArr[2][13] = OnCANRMsgCAN2MB13;
	OnCANRMsgArr[2][14] = OnCANRMsgCAN2MB14;
	OnCANRMsgArr[2][15] = OnCANRMsgCAN2MB15;
										//MB发送中断函数
	OnCANTMsgArr[0][0] = OnCANTMsgCAN0MB0;
	OnCANTMsgArr[1][0] = OnCANTMsgCAN1MB0;
	OnCANTMsgArr[2][0] = OnCANTMsgCAN2MB0;
	
										//安装MB0中断函数，发送缓冲
	INTC_InstallINTCInterruptHandler(OnCANTMsgArr[channel][0],CAN_VECTOR_SNUM[channel]+0,INTERRUPT_PRIORITY);
	
	INTC_InstallINTCInterruptHandler(OnCANRMsgArr[channel][12],CAN_VECTOR_SNUM[channel]+12,INTERRUPT_PRIORITY);
	INTC_InstallINTCInterruptHandler(OnCANRMsgArr[channel][13],CAN_VECTOR_SNUM[channel]+13,INTERRUPT_PRIORITY);
	INTC_InstallINTCInterruptHandler(OnCANRMsgArr[channel][14],CAN_VECTOR_SNUM[channel]+14,INTERRUPT_PRIORITY);
	INTC_InstallINTCInterruptHandler(OnCANRMsgArr[channel][15],CAN_VECTOR_SNUM[channel]+15,INTERRUPT_PRIORITY);
	
										//确定操作寄存器基地址
	if(channel==0)
	{	
		CAN_X = &CAN_A;
	}
	else if(channel==2)
	{	
		CAN_X = &CAN_C;
	}
	else if(channel==1)
	{
		CAN_X = &CAN_B;
	}
										//进入Freeze模式
	(*CAN_X).MCR.B.FRZ = 1;				//允许进入Freeze模式
	(*CAN_X).MCR.B.HALT = 1;			//当FRZ==1时,设置该位请求CAN模块进入Freeze模式
	while( (*CAN_X).MCR.B.FRZACK != 1 )
	{
										//等待进入到Freeze模式
		if(times_out_FRZ==0)break;
		times_out_FRZ--;
	}
	if(times_out_FRZ==0)return ERROR_TIMESOUT;
	(*CAN_X).MCR.B.MBFEN= 1;            //对于每一个MB，有单独的MASK，GlobalMask不起作用
	(*CAN_X).MCR.B.WRNEN= 1;            //待研究
	(*CAN_X).MCR.B.SRXDIS= 1;         	//待研究
	(*CAN_X).MCR.B.FEN= 0;              //FIFO不使能
	(*CAN_X).MCR.B.AEN= 1;              //abort功能使能
	(*CAN_X).MCR.B.LPRIO_EN= 0;         //不使用Local Priority
	
	if(baudrate==250000UL)
	{
	
		(*CAN_X).CR.B.PRESDIV = 3;		//Sclock frequency = CPI clock frequency / (PRESDIV + 1)=4Mhz
		(*CAN_X).CR.B.RJW = 0;
		(*CAN_X).CR.B.PSEG1 = 3;
		(*CAN_X).CR.B.PSEG2 = 3;
		(*CAN_X).CR.B.PROPSEG = 6	;	//Bit Rate = Sclock/(SYNC_SEG+PROP_SEG+PSEG1+2+PSEG2+1)
										//Bit Rate = 4Mhz/((1+6++3+2+3+1))=250Khz
		
	}
	else if(baudrate==500000UL)
	{
		(*CAN_X).CR.B.PRESDIV = 1;		//Sclock frequency = CPI clock frequency / (PRESDIV + 1)=8Mhz
		(*CAN_X).CR.B.RJW = 0;
		(*CAN_X).CR.B.PSEG1 = 3;
		(*CAN_X).CR.B.PSEG2 = 3;
		(*CAN_X).CR.B.PROPSEG = 6	;	//Bit Rate = Sclock/(SYNC_SEG+PROP_SEG+PSEG1+2+PSEG2+1)
										//Bit Rate = 8Mhz/((1+6++3+2+3+1))=500Khz		
	}
	else if(baudrate==1000000UL)
	{
		(*CAN_X).CR.B.PRESDIV = 0;		//Sclock frequency = CPI clock frequency / (PRESDIV + 1)=16Mhz
		(*CAN_X).CR.B.RJW = 0;
		(*CAN_X).CR.B.PSEG1 = 3;
		(*CAN_X).CR.B.PSEG2 = 3;
		(*CAN_X).CR.B.PROPSEG = 6	;	//Bit Rate = Sclock/(SYNC_SEG+PROP_SEG+PSEG1+2+PSEG2+1)
										//Bit Rate = 16Mhz/((1+6++3+2+3+1))=1000Khz				
	}
	(*CAN_X).CR.B.LPB = 0;				//Disable Loop Back
	(*CAN_X).CR.B.BOFFMSK = 0;			//Busoff中断禁止使能
	(*CAN_X).CR.B.ERRMSK = 0;			//Error中断禁止使能
	(*CAN_X).CR.B.LBUF = 1;				//编号低的MB优先发送
										
										//初始化所有MB
	message_buf_index = 0;
	while(message_buf_index<16)
	{									//MB 强制到INACTIVE
		(*CAN_X).BUF[message_buf_index].CS.B.CODE = 0b0000;
		message_buf_index++;
	}
										//
	
	(*CAN_X).BUF[12].ID.B.STD_ID = acccode1&0x000007FFL;
	(*CAN_X).BUF[12].CS.B.IDE = 0;
	(*CAN_X).BUF[12].CS.B.RTR = 0;
	t_timer = (*CAN_X).TIMER.R;

	
	(*CAN_X).BUF[13].ID.R = acccode1&0x1FFFFFFFL;
	(*CAN_X).BUF[13].CS.B.IDE = 1;
	(*CAN_X).BUF[13].CS.B.RTR = 0;
	t_timer = (*CAN_X).TIMER.R;
	
	(*CAN_X).BUF[14].ID.B.STD_ID = acccode2&0x000007FFL;
	(*CAN_X).BUF[14].CS.B.IDE = 0;
	(*CAN_X).BUF[14].CS.B.RTR = 0;
	t_timer = (*CAN_X).TIMER.R;

	
	(*CAN_X).BUF[15].ID.R = acccode2&0x1FFFFFFFL;
	(*CAN_X).BUF[15].CS.B.IDE = 1;
	(*CAN_X).BUF[15].CS.B.RTR = 0;
	t_timer = (*CAN_X).TIMER.R;
	
	if(mask1&0x80000000)
	{
		if(acccode1&0x80000000)
		{
			(*CAN_X).BUF[13].CS.B.CODE = 0b0100;			
		}
		else
		{
			(*CAN_X).BUF[12].CS.B.CODE = 0b0100;			
		}
	}
	else
	{
		(*CAN_X).BUF[12].CS.B.CODE = 0b0100;
		(*CAN_X).BUF[13].CS.B.CODE = 0b0100;
	}
	if(mask2&0x80000000)
	{
		if(acccode2&0x80000000)
		{
			(*CAN_X).BUF[15].CS.B.CODE = 0b0100;			
		}
		else
		{
			(*CAN_X).BUF[14].CS.B.CODE = 0b0100;			
		}
	}
	else
	{
		(*CAN_X).BUF[14].CS.B.CODE = 0b0100;
		(*CAN_X).BUF[15].CS.B.CODE = 0b0100;
	}
	
	(*CAN_X).RXIMR[12].R = (mask1&0x7FFL)<<18;
										//设置MASK1
	(*CAN_X).RXIMR[13].R = mask1&0x1FFFFFFF;
	
	(*CAN_X).RXIMR[14].R = (mask2&0x7FFL)<<18;
										//设置MASK2
	(*CAN_X).RXIMR[15].R = mask2&0x1FFFFFFF;		

	
										//Setup MB interrupt
	(*CAN_X).IMRL.R |= 1UL;
										//Setup MB interrupt
	(*CAN_X).IMRL.R |= 1UL<<12;
	(*CAN_X).IMRL.R |= 1UL<<13;
	(*CAN_X).IMRL.R |= 1UL<<14;
	(*CAN_X).IMRL.R |= 1UL<<15;
	
										//退出freeze模式
	(*CAN_X).MCR.B.HALT = 0;
	while((*CAN_X).MCR.B.NOTRDY==1)
	{
										//wait can channel ready
		if(times_out==0)break;
		times_out--;
	}											
										//配置管脚作为CAN
	if(channel==0)
	{	
		SIU.PCR[83].R = 0x0600;         // Configure pad as FlexCAN_B_Tx
  		SIU.PCR[84].R = 0x0500;        	// Configure pad as FlexCAN_B_Rx
	}
	else if(channel==2)
	{	
		SIU.PCR[87].R = 0x0600;         // Configure pad as FlexCAN_C_Tx
  		SIU.PCR[88].R = 0x0500;        	// Configure pad as FlexCAN_C_Rx
	}
	else if(channel==1)
	{
		SIU.PCR[85].R = 0x0600;         // Configure pad as FlexCAN_A_Tx
		SIU.PCR[86].R = 0x0500;         // Configure pad as FlexCAN_A_Rx	
	}	
	if(times_out==0)return ERROR_TIMESOUT;
	return ERROR_OK;
	
}
/*
** ===================================================================
**	名称:SetupCANRMSGCB
**	参数:	UINT_8 channel		通道
**									0  通道0 接插件{CANL0,CANH0}
**									1  通道1 接插件{CANL1,CANH1}
**									2  通道2 接插件{CANL2,CANH2}
**			CANRCBTYPE cb		接收到CAN消息后的回调函数
**  功能:  设置CAN接受到消息后的回调函数
**  返回值: 
**			ERROR_OK		设置成功
**	注意:
** ===================================================================
*/

UINT_8 SetupCANRMSGCB(UINT_8 channel,CANRCBTYPE cb)
{
	OnCANRmsgCB[channel]=cb;
	return ERROR_OK;
}
/*
** ===================================================================
**	名称:SendCANMsgMB
**	参数:	UINT_8 channel		通道
**									0  通道0 接插件{CANL0,CANH0}
**									1  通道1 接插件{CANL1,CANH1}
**									2  通道2 接插件{CANL2,CANH2}
**			UINT_8 MBindex		MB索引值
**									0	Message Buffer 0
**									...	...
**									15	Message Buffer 16
**			UINT_8 ext			是否是扩展帧
**									0	标准帧
**									1	扩展帧
**			UINT_8 dataframe	是否是数据帧
**									0	远程帧
**									1	数据帧
**			UINT_32 canid		CAN ID
**			UINT_8 datalen		数据字节数 0-8
**			UINT_8 *data		存储数据数组的起始地址指针
**										
**  功能:  直接通过MB发送CAN消息
**  返回值: 
**			ERROR_OK			写入MB成功
**			ERROR_MBINDEX_RANGE	MB index超出范围
**			ERROR_MB_BUSY		MB正在发送
**			...
**	注意:	当该函数返回ERROR_MB_BUSY后,不要用循环调用该函数如
**			while(ERROR_OK!=SendCANMsgMB(xxxxxx));
** ===================================================================
*/

UINT_8 SendCANMsgMB(UINT_8 channel,UINT_8 MBindex,UINT_8 ext,\
				UINT_8 dataframe,UINT_32 canid,UINT_8 datalen,UINT_8 *data)
{
	volatile struct FLEXCAN2_tag * CAN_X;
	if(channel==0)
	{
		CAN_X = &CAN_A;
	}
	else if(channel==2)
	{	
		CAN_X = &CAN_C;
	}
	else if(channel==1)
	{
		CAN_X = &CAN_B;
	}
	
	if(MBindex>=16)
		return ERROR_MBINDEX_RANGE;		//MB index超出范围
										//MB 是否已经写入
	(*CAN_X).IFRL.R = 1UL<<MBindex;		//清除中断标志
	if((*CAN_X).BUF[MBindex].CS.B.CODE==0b1100)
		return	ERROR_MB_BUSY;
										//MB 强制到INACTIVE
	(*CAN_X).BUF[MBindex].CS.B.CODE = 0b1000;
										//设置是否是远程帧
	(*CAN_X).BUF[MBindex].CS.B.RTR  =dataframe?0:1;
										//设置数据长度
	(*CAN_X).BUF[MBindex].CS.B.LENGTH = datalen;	
										//设置是否是扩展帧
	(*CAN_X).BUF[MBindex].CS.B.IDE = ext;
	if(ext==1)
	{
			(*CAN_X).BUF[MBindex].ID.R	= canid;
			(*CAN_X).BUF[MBindex].CS.B.SRR=1;
	}
	else
	{
		(*CAN_X).BUF[MBindex].ID.B.STD_ID = canid;
		(*CAN_X).BUF[MBindex].CS.B.SRR=0;
	}
	
	
	while(datalen>0)					//拷贝数据
	{
		(*CAN_X).BUF[MBindex].DATA.B[datalen-1] = data[datalen-1];
		datalen--;
	}
										//强制发送
	(*CAN_X).BUF[MBindex].CS.B.CODE = 0b1100;

	return ERROR_OK;
	
}
/*
** ===================================================================
**	名称:GetCANMsgMB
**	参数:	UINT_8 channel		通道
**									0  通道0 接插件{CANL0,CANH0}
**									1  通道1 接插件{CANL1,CANH1}
**									2  通道2 接插件{CANL2,CANH2}
**			UINT_8 MBindex		MB索引值
**									0	Message Buffer 0
**									...	...
**									15	Message Buffer 16
**			UINT_8 *ext			是否是扩展帧
**									0	标准帧
**									1	扩展帧
**			UINT_8 *dataframe	是否是数据帧
**									0	远程帧
**									1	数据帧
**			UINT_32 *canid		CAN ID
**			UINT_8 *datalen		数据字节数 0-8
**			UINT_8 *data		存储数据数组的起始地址指针
**										
**  功能:  读取MB中CAN消息
**  返回值: 
**			ERROR_OK			写入MB成功
**			ERROR_MBINDEX_RANGE	MB index超出范围
**			ERROR_MBBUF_EMPTY	MB 空
**			ERROR_MB_OVERRUN	MB overrun
**			ERROR_MB_BUSY		MB Busy
**			...
**	注意:	
** ===================================================================
*/

UINT_8 GetCANMsgMB(UINT_8 channel,UINT_8 MBindex,UINT_8 *ext,\
				UINT_8 *dataframe,UINT_32 *canid,UINT_8 *datalen,UINT_8 *data)
{
	VUINT_32 mb_code;
	VUINT_32 tmplen;
	VUINT_32 timer;

	volatile struct FLEXCAN2_tag * CAN_X;
	if(channel==0)
	{
		CAN_X = &CAN_A;
	}
	else if(channel==2)
	{	
		CAN_X = &CAN_C;
	}
	else if(channel==1)
	{
		CAN_X = &CAN_B;
	}
	if(MBindex>=16)
		return ERROR_MBINDEX_RANGE;		//MB index超出范围
	if(!((*CAN_X).IFRL.R&(1UL<<MBindex)))
	{
		return ERROR_MBBUF_EMPTY;
	}									//清除中断标志位
	//(*CAN_X).IFRL.R|=1UL<<MBindex;
										//读取MB的Code
	mb_code = (*CAN_X).BUF[MBindex].CS.B.CODE;
	if(mb_code==0b0010)
	{
										//缓冲区满
		*dataframe 	= (*CAN_X).BUF[MBindex].CS.B.RTR?0:1;
		*datalen 	= (UINT_8)(*CAN_X).BUF[MBindex].CS.B.LENGTH;
		*ext		= (*CAN_X).BUF[MBindex].CS.B.IDE?1:0;
	 	if(*ext==1)
	 	{
	 		*canid = (*CAN_X).BUF[MBindex].ID.R;	
	 	}
	 	else
	 	{
	 		*canid = (*CAN_X).BUF[MBindex].ID.B.STD_ID;	
	 	}
	 	tmplen = *datalen;
	 	while(tmplen>0)
	 	{
	 		data[tmplen-1] = (*CAN_X).BUF[MBindex].DATA.B[tmplen-1];
	 		tmplen--;
	 	}
	 	timer = (*CAN_X).TIMER.R;
	 	(*CAN_X).BUF[MBindex].CS.B.CODE = 0b0100;
	 	return ERROR_OK;

	}else if(mb_code==0b0110)
	{
		
		timer = (*CAN_X).TIMER.R;		//OVERRUN
		(*CAN_X).BUF[MBindex].CS.B.CODE = 0b0100;
		return ERROR_MB_OVERRUN;
	}
	else if((mb_code&0b1001)==0b0001)
	{
		timer = (*CAN_X).TIMER.R;		//正在进行更新操作
		return ERROR_MB_BUSY;
	}
	return ERROR_OK;
}
/*
** ===================================================================
**	名称:OnCANRMsg
**	参数:	
**  功能:  CAN通道接收中断回调
**  返回值: 
**			...
**	注意:	
** ===================================================================
*/


void OnCANRMsg(UINT_8 channel,UINT_8 MBindex)
{
	VUINT_8  sfbufindex;
	volatile struct FLEXCAN2_tag * CAN_X;
	if(channel==0)
	{
		CAN_X = &CAN_A;
	}
	else if(channel==2)
	{	
		CAN_X = &CAN_C;
	}
	else if(channel==1)
	{
		CAN_X = &CAN_B;
	}
	if((*CAN_X).IFRL.R&(1UL<<MBindex))
	{
		sfbufindex = CAN_MSG_in[channel];
		if(CAN_MSG_num[channel]<CAN_MSGBUF_NUM)
		{
			CAN_MSG_in[channel] = (CAN_MSG_in[channel]+1)%CAN_MSGBUF_NUM;
			CAN_MSG_num[channel]++;
		}
		else
		{
										//OVERRUN
		}
		GetCANMsgMB(channel,MBindex,&(CAN_MSG_ext[channel][sfbufindex]),\
						&(CAN_MSG_dataframe[channel][sfbufindex]),\
						&(CAN_MSG_canid[channel][sfbufindex]),\
						&(CAN_MSG_datalen[channel][sfbufindex]),\
						CAN_MSG_data[channel][sfbufindex]);
	}
	(*CAN_X).IFRL.R|=(1UL<<MBindex);
	if(OnCANRmsgCB[channel]!=0)
	OnCANRmsgCB[channel]();

}
/*
** ===================================================================
**	名称:OnCANTMsg
**	参数:	
**  功能:  CAN通道发送中断
**  返回值: 
**			...
**	注意:	
** ===================================================================
*/
void OnCANTMsg(UINT_8 channel,UINT_8 MBindex)
{

	VUINT_8  sfbufindex=0;
	volatile struct FLEXCAN2_tag * CAN_X;

	if(channel==0)
	{
		CAN_X = &CAN_A;
	}
	else if(channel==2)
	{	
		CAN_X = &CAN_C;
	}
	else if(channel==1)
	{
		CAN_X = &CAN_B;
	}
	if(CAN_TMSG_num[channel]==0)
	{
		(*CAN_X).IFRL.R=(1UL<<MBindex);
		return ;
	}
	sfbufindex = CAN_TMSG_out[channel];
	if((*CAN_X).IFRL.R&(1UL))
	{
		if(ERROR_OK==SendCANMsgMB(channel,0,CAN_TMSG_ext[channel][sfbufindex],\
					CAN_TMSG_dataframe[channel][sfbufindex],\
					CAN_TMSG_canid[channel][sfbufindex],\
					CAN_TMSG_datalen[channel][sfbufindex],\
					CAN_TMSG_data[channel][sfbufindex]))
		{
			CAN_TMSG_out[channel] = (CAN_TMSG_out[channel]+1)%CAN_TMSGBUF_NUM;
			CAN_TMSG_num[channel]--;		
		}
	}
		
	(*CAN_X).IFRL.R=(1UL<<MBindex);

}

/*
** ===================================================================
**	名称:OnCANMsgCANXMBX
**	参数:	
**  功能:  CAN通道中断
**  返回值: 
**			...
**	注意:	
** ===================================================================
*/
void OnCANTMsgCAN0MB0(void)
{
	OnCANTMsg(0,0);	
}

void OnCANTMsgCAN1MB0(void)
{
	OnCANTMsg(1,0);	
}

void OnCANTMsgCAN2MB0(void)
{
	OnCANTMsg(2,0);	
}

void OnCANRMsgCAN0MB12(void)
{
	OnCANRMsg(0,12);	
}
void OnCANRMsgCAN0MB13(void)
{
	OnCANRMsg(0,13);	
}
void OnCANRMsgCAN0MB14(void)
{
	OnCANRMsg(0,14);	
}
void OnCANRMsgCAN0MB15(void)
{
	OnCANRMsg(0,15);	
}
void OnCANRMsgCAN1MB12(void)
{
	OnCANRMsg(1,12);	
}
void OnCANRMsgCAN1MB13(void)
{
	OnCANRMsg(1,13);	
}
void OnCANRMsgCAN1MB14(void)
{
	OnCANRMsg(1,14);	
}
void OnCANRMsgCAN1MB15(void)
{
	OnCANRMsg(1,15);	
}
void OnCANRMsgCAN2MB12(void)
{
	OnCANRMsg(2,12);	
}
void OnCANRMsgCAN2MB13(void)
{
	OnCANRMsg(2,13);	
}
void OnCANRMsgCAN2MB14(void)
{
	OnCANRMsg(2,14);	
}
void OnCANRMsgCAN2MB15(void)
{
	OnCANRMsg(2,15);	
}

/*
** ===================================================================
**	名称:GetCANMsg
**	参数:	UINT_8 channel		通道
**									0  通道0 接插件{CANL0,CANH0}
**									1  通道1 接插件{CANL1,CANH1}
**									2  通道2 接插件{CANL2,CANH2}
**			UINT_8 *ext			是否是扩展帧
**									0	标准帧
**									1	扩展帧
**			UINT_8 *dataframe	是否是数据帧
**									0	远程帧
**									1	数据帧
**			UINT_32 *canid		CAN ID
**			UINT_8 *datalen		数据字节数 0-8
**			UINT_8 *data		存储数据数组的起始地址指针
**										
**  功能:  读取CAN缓冲区数据
**  返回值: 
**			ERROR_OK			读取数据成功
**			ERROR_SFBUF_EMPTY	软件缓冲区空		
**			...
**	注意:	
** ===================================================================
*/

UINT_8 GetCANMsg(UINT_8 channel,UINT_8 *ext,UINT_8 *dataframe,\
					UINT_32 *canid,UINT_8 *datalen,UINT_8 *data)
{
	VUINT_8 can_data_len;
	VUINT_32 temp_cur_pri;
	if(CAN_MSG_num[channel]==0)
	{
		return ERROR_SFBUF_EMPTY;
	}
	temp_cur_pri = INTC.CPR.B.PRI;			//进入临界区
	INTC.CPR.B.PRI = 15;	
	
	CAN_MSG_num[channel]--;
	*ext 		= CAN_MSG_ext[channel][CAN_MSG_out[channel]];
	*dataframe  = CAN_MSG_dataframe[channel][CAN_MSG_out[channel]];
	*canid		= CAN_MSG_canid[channel][CAN_MSG_out[channel]];
	*datalen	= CAN_MSG_datalen[channel][CAN_MSG_out[channel]];
	can_data_len = *datalen;
	while(can_data_len>0)
	{
		data[can_data_len-1] = CAN_MSG_data[channel][CAN_MSG_out[channel]][can_data_len-1];
		can_data_len--;
	}
	CAN_MSG_out[channel] = (CAN_MSG_out[channel]+1)%CAN_MSGBUF_NUM;
	
	INTC.CPR.B.PRI = temp_cur_pri;			//退出临界区	
	return ERROR_OK;

	
}
/*
** ===================================================================
**	名称:SendCANMsg
**	参数:	UINT_8 channel		通道
**									0  通道0 接插件{CANL0,CANH0}
**									1  通道1 接插件{CANL1,CANH1}
**									2  通道2 接插件{CANL2,CANH2}
**			UINT_8 ext			是否是扩展帧
**									0	标准帧
**									1	扩展帧
**			UINT_8 dataframe	是否是数据帧
**									0	远程帧
**									1	数据帧
**			UINT_32 canid		CAN ID
**			UINT_8 datalen		数据字节数 0-8
**			UINT_8 *data		存储数据数组的起始地址指针
**										
**  功能:  发送CAN缓冲区添加数据
**  返回值: 
**			ERROR_OK			发送数据成功
**			ERROR_SFTBUF_FULL	软件缓冲区满		
**			...
**	注意:	
** ===================================================================
*/


UINT_8 SendCANMsg(UINT_8 channel,UINT_8 ext,UINT_8 dataframe,\
					UINT_32 canid,UINT_8 datalen,UINT_8 *data)
{
	VUINT_8 can_data_len;
	VUINT_32 temp_cur_pri;
	
	
	if(CAN_TMSG_num[channel]>=CAN_TMSGBUF_NUM)
	{
		return ERROR_SFTBUF_FULL;
	}
	if(CAN_TMSG_num[channel]==0)
	{
											//如果MB空，尝试发送
		if(ERROR_OK==SendCANMsgMB(channel,0,ext,dataframe,canid,datalen,data))
		{
			return ERROR_OK;
		}
	}										//如果MB正在发送，则写入缓冲区
	temp_cur_pri = INTC.CPR.B.PRI;			//进入临界区
	INTC.CPR.B.PRI = 15;								
	CAN_TMSG_num[channel]++;
	CAN_TMSG_ext[channel][CAN_TMSG_in[channel]] = ext;
	CAN_TMSG_dataframe[channel][CAN_TMSG_in[channel]] = dataframe;
	CAN_TMSG_canid[channel][CAN_TMSG_in[channel]] = canid;
	CAN_TMSG_datalen[channel][CAN_TMSG_in[channel]] = datalen;
	can_data_len = datalen;
	while(can_data_len>0)
	{
		CAN_TMSG_data[channel][CAN_TMSG_in[channel]][can_data_len-1] = data[can_data_len-1];
		can_data_len--;
	}
	CAN_TMSG_in[channel] = (CAN_TMSG_in[channel]+1)%CAN_TMSGBUF_NUM;
	INTC.CPR.B.PRI = temp_cur_pri;			//退出临界区
	return ERROR_OK;

	
}


int CanTransmitPossible(void)
{
	if(CAN_TMSG_num[1]>=CAN_TMSGBUF_NUM)
	{
		return 0; // buffer 满了
	}	
	
	else
		return 1;    //可以发送
}