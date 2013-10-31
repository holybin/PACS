//Protocol.h
//定义服务端和客户端通信协议
//服务端和客户端共有

//约定通信端口号
#ifndef PRE_AGREED_PORT
#define PRE_AGREED_PORT 8888
#endif

//约定每次传输的图片数目：此处为3
#ifndef DCM_NUM
#define DCM_NUM 3
#endif

//约定消息结构体
////要先协商进行的DIMSE消息////
typedef struct ASSOCIATE_PDU
{
	TCHAR PDUName[20];//PDU名称：A-ASSOCIATE-RQ, A-ASSOCIATE-RJ, A-ASSOCIATE-AC
	TCHAR SOPClassUID[100];	//SOP类UID
	TCHAR SOPInstanceUID[100];	//SOP实例UID
	TCHAR transferSyntaxUID[100];	//传输句法
	//TCHAR DIMSEname[20]; //约定DIMSE消息
}ASSOCIATE_PDU;

////业务类型////
//PDU名称：C-FIND-RQ, C-STORE-RQ, C-MOVE-RQ, C-FIND-RSP, C-STORE-RSP, C-MOVE-RSP
typedef struct C_MOVE_PDU
{
	TCHAR PDUname[20];	//PDU
	TCHAR msgID[20];	//消息ID
	TCHAR SOPClassUID[100];	//影响的SOP类UID
	//TCHAR data[1024];	//数据
}C_MOVE_PDU;

////释放链接////
typedef struct RELEASE_PDU
{
	TCHAR PDUName[20];	//PDU名称：A-Release-RQ, A-Release-RP
	TCHAR serviceName[20];	//服务类名称
}RELEASE_PDU;