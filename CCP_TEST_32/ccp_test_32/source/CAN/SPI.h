/*=======================================================================
**     KPB03 开发库定义文件
**     文件名    : SPI.h
**     工程名    : T5644ATest
**     系统平台  : BMS32
**     编译平台  : CodeWarrior IDE version 2.8
**     创建日期  : 2012-9-28 17:02:25
**     简介      : CAN模块操作
**     版权      : (c) Copyright KeyPower, 2012
**     开发者    : 张鹏 
**     联系方式  : peng.zhang@key-power.com.cn
**     历史版本  :
**                 	V0.1 2012-9-28  实现基本功能 
=======================================================================*/
#ifndef _BMS_SPI_H
#define _BMS_SPI_H
extern void InitDSPIB();
extern void DSPIB_Send(UINT_16*senddata,UINT_16*recvdata,UINT_16 datanum);
extern void DSPIB_Send_Ch(UINT_16*senddata,UINT_16*recvdata,UINT_16 datanum,UINT_8 cs);
extern void InitUJA1075A();
extern void GetUJA1075A();
extern void SleepUJA1075A(UINT_8 wake_cfg_can,UINT_8 wake_cfg_cc,UINT_8 wake_cfg_key);
extern UINT_8 SetAO(UINT_8 channel,UINT_16 vol);
#endif
