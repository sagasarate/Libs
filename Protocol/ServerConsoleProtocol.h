/****************************************************************************/
/*                                                                          */
/*      文件名:    ServerConsoleProtocol.h                                  */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

#pragma pack(push)
#pragma pack(4)

enum SERVER_CONSOLE_MSG
{
	SC_MSG_QUERY_SERVER_STATUS=0x1000,				//获取服务器状态
	SC_MSG_QUERY_ALL_SERVER_STATUS,
	SC_MSG_QUERY_SERVER_STATUS_RESULT,

	SC_MSG_SET_SERVER_STATUS,						//设置服务器状态
	SC_MSG_SET_SERVER_STATUS_RESULT,
	
	SC_MSG_LINK_LOG,								//开启接收Log
	SC_MSG_UNLINK_LOG,								//关闭接收Log
	SC_MSG_SERVER_LOG,								//服务器Log

	SC_MSG_EXEC_COMMAND,							//执行命令

	SC_MSG_GET_SERVER_STATUS_FORMAT_INFO,			//获取服务器状态格式信息
	SC_MSG_GET_SERVER_STATUS_FORMAT_INFO_RESULT,
};

enum SC_SST_QUERY_SERVER_STATUS
{
	SC_SST_QSS_STATUS_ID=1,
};


enum SC_SST_SERVER_LOG
{
	SC_SST_SL_LOG_STR=1,
};

enum SC_SST_EXEC_COMMAND
{
	SC_SST_EC_COMMAND_STR=1,
};



enum SC_SST_SERVER_STATUS
{
	SC_SST_SS_CLIENT_COUNT=1,
	SC_SST_SS_CYCLE_TIME,
	SC_SST_SS_CPU_COST,
	SC_SST_SS_TCP_RECV_FLOW,
	SC_SST_SS_TCP_SEND_FLOW,
	SC_SST_SS_UDP_RECV_FLOW,
	SC_SST_SS_UDP_SEND_FLOW,
	SC_SST_SS_TCP_RECV_COUNT,
	SC_SST_SS_TCP_SEND_COUNT,
	SC_SST_SS_UDP_RECV_COUNT,
	SC_SST_SS_UDP_SEND_COUNT,
	SC_SST_SS_PROGRAM_VERSION,
};

enum SC_SST_SERVER_STATUS_FORMAT_INFO_LIST
{
	SC_SST_SSFIL_SERVER_STATUS_FORMAT_INFO=1,
};

enum SC_SST_SERVER_STATUS_FORMAT_INFO
{
	SC_SST_SSFI_STATUS_ID=1,
	SC_SST_SSFI_FORMAT_TYPE,
	SC_SST_SSFI_NAME,
};

#pragma pack(pop)