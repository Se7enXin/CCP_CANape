/*=======================================================================
**     KPV13 开发库定义文件
**     文件名    : CAN.h
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
**					V0.4 2013-01-25 添加初始化超过1.42us超时
=======================================================================*/
#ifndef _KPV_CAN_H
#define _KPV_CAN_H

#define ERROR_OK					((UINT_8)0)
#define ERROR_MBINDEX_RANGE			((UINT_8)1)
#define ERROR_CHANNEL_RANGE			((UINT_8)2)
#define ERROR_MBBUF_EMPTY			((UINT_8)3)
#define ERROR_MB_OVERRUN			((UINT_8)4)
#define ERROR_MB_BUSY				((UINT_8)5)
#define ERROR_SFBUF_EMPTY			((UINT_8)6)
#define ERROR_SFTBUF_FULL			((UINT_8)7)
#define ERROR_TIMESOUT				((UINT_8)8)

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
**			UINT_32 mask1		屏蔽MASK1
**			UINT_32 acccode1	ACCODE1
**			UINT_32 mask2		屏蔽MASK2
**			UINT_32 acccode2	ACCODE2
**  功能:  设置CAN通道波特率并使能CAN通道
**  返回值: 
**			ERROR_OK		设置成功
**			ERROR_TIMESOUT	初始化超时
**			待添加
**	注意:
** ===================================================================
*/
extern UINT_8 InitCAN(UINT_8 channel,UINT_32 baudrate,UINT_32 mask1,\
					UINT_32 acccode1,UINT_32 mask2,UINT_32 acccode2);

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
					UINT_32 canid,UINT_8 datalen,UINT_8 *data);
					
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
					UINT_32 *canid,UINT_8 *datalen,UINT_8 *data);
typedef void (*CANRCBTYPE)(void);

extern  int CanTransmitPossible(void);

#endif