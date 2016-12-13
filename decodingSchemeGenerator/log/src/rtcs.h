#ifndef __RTCS_H__
#define __RTCS_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define RTCS_VERSION  351
#define STR_RTCS_VERSION  "3.51"
  
#undef STR_BUILD_MESSAGE
#define STR_BUILD_MESSAGE ", Using Visual Studio 6.0"

//以前默认使用Static Link Lib的兼容处理
#ifdef VC
	#undef _RTCSLIB
	#define _RTCSLIB

#endif

//给应用模块提供各种简单的组宏

//use RTCS,INTP,ALARM,TCP Static Link LIB
#ifdef _RTCS_INTP_ALARM_TCPLIB
	#define  _RTCS_TCPLIB
	#define  _INTP_TCPLIB
	#define  _ALARM_TCPLIB

	#undef STR_VERSION_MESSAGE
	#define STR_VERSION_MESSAGE ", USE LIB: TCP RTCS version is " STR_RTCS_VERSION

	#define _TCPRTCS //和以前兼容
#endif

//use RTCS,INTP,ALARM,UDP Static Link LIB
#ifdef _RTCS_INTP_ALARM_UDPLIB
	#define  _RTCS_UDPLIB
	#define  _INTP_UDPLIB
	#define  _ALARM_UDPLIB

    #undef STR_VERSION_MESSAGE
    #define STR_VERSION_MESSAGE ", USE LIB: UDP RTCS version is " STR_RTCS_VERSION

#endif

//use RTCS,INTP,ALARM,TCP Dynamic Link LIB
#ifdef _RTCS_INTP_ALARM_TCPDLL
	#define  _RTCS_TCPDLL
	#define  _INTP_TCPDLL
	#define  _ALARM_TCPDLL

    #undef STR_VERSION_MESSAGE
    #define STR_VERSION_MESSAGE ", USE DLL: TCP RTCS version is " STR_RTCS_VERSION

	#define _TCPRTCS //和以前兼容
#endif

//use RTCS,INTP,ALARM,UDP Dynamic Link LIB
#ifdef _RTCS_INTP_ALARM_UDPDLL
	#define  _RTCS_UDPDLL
	#define  _INTP_UDPDLL
	#define  _ALARM_UDPDLL

    #undef STR_VERSION_MESSAGE
    #define STR_VERSION_MESSAGE ", USE DLL: UDP RTCS version is " STR_RTCS_VERSION

#endif


//use RTCS,INTP,TCP Static LIB
#ifdef _RTCS_INTP_TCPLIB
	#define  _RTCS_TCPLIB
	#define  _INTP_TCPLIB

    #undef STR_VERSION_MESSAGE
    #define STR_VERSION_MESSAGE ", USE LIB: TCP RTCS version is " STR_RTCS_VERSION

	#define _TCPRTCS //和以前兼容
#endif
//use RTCS,INTP,UDP Static LIB
#ifdef _RTCS_INTP_UDPLIB
	#define  _RTCS_UDPLIB
	#define  _INTP_UDPLIB

    #undef STR_VERSION_MESSAGE
    #define STR_VERSION_MESSAGE ", USE LIB: UDP RTCS version is " STR_RTCS_VERSION

#endif
//use RTCS,ALARM,TCP Static LIB
#ifdef _RTCS_ALARM_TCPLIB
	#define  _RTCS_TCPLIB
	#define  _ALARM_TCPLIB

	#undef STR_VERSION_MESSAGE
	#define STR_VERSION_MESSAGE ", USE LIB:	TCP RTCS version is " STR_RTCS_VERSION

	#define _TCPRTCS //和以前兼容
#endif
//use RTCS,ALARM,UDP Static LIB
#ifdef _RTCS_ALARM_UDPLIB
	#define  _RTCS_UDPLIB
	#define  _ALARM_UDPLIB

	#undef STR_VERSION_MESSAGE
	#define STR_VERSION_MESSAGE ", USE LIB:	UDP RTCS version is " STR_RTCS_VERSION

#endif

//use RTCS,INTP,TCP Dynamic LIB
#ifdef _RTCS_INTP_TCPDLL
	#define  _RTCS_TCPDLL
	#define  _INTP_TCPDLL
	
    #undef STR_VERSION_MESSAGE
    #define STR_VERSION_MESSAGE ", USE DLL: TCP RTCS version is " STR_RTCS_VERSION

	#define _TCPRTCS //和以前兼容
#endif
//use RTCS,INTP,UDP Dynamic LIB
#ifdef _RTCS_INTP_UDPDLL
	#define  _RTCS_UDPDLL
	#define  _INTP_UDPDLL

#undef STR_VERSION_MESSAGE
#define STR_VERSION_MESSAGE ", USE DLL: UDP RTCS version is " STR_RTCS_VERSION

#endif
//use RTCS,ALARM,TCP Dynamic LIB
#ifdef _RTCS_ALARM_TCPDLL
	#define  _RTCS_TCPDLL
	#define  _ALARM_TCPDLL

	#undef STR_VERSION_MESSAGE
	#define STR_VERSION_MESSAGE ", USE DLL: TCP RTCS version is " STR_RTCS_VERSION

	#define _TCPRTCS //和以前兼容
#endif
//use RTCS,ALARM,UDP Dynamic LIB
#ifdef _RTCS_ALARM_UDPDLL
	#define  _RTCS_UDPDLL
	#define  _ALARM_UDPDLL

	#undef STR_VERSION_MESSAGE
	#define STR_VERSION_MESSAGE ", USE DLL:	UDP RTCS version is " STR_RTCS_VERSION

#endif

#if defined(_RTCS_UDPDLL) || defined(_RTCS_TCPDLL)
    #undef  _RTCSDLL
	#define _RTCSDLL
#elif defined(_RTCS_UDPLIB) || defined(_RTCS_TCPLIB)
    #undef  _RTCSLIB
	#define _RTCSLIB
#else
#endif

//for rtcs,intp,alarm lib
#ifdef _RTCSDLL
//use dynamic lib
	#ifndef	STR_VERSION_MESSAGE
		#undef STR_VERSION_MESSAGE
		#define STR_VERSION_MESSAGE ", USE DLL: RTCS version is " STR_RTCS_VERSION
	#endif
	#if defined _RTCS_TCPDLL && ! defined(RTCS_TCPO_EXPORTS)
		#ifdef _DEBUG
			#pragma comment(lib, "TRTCSd.lib")
		#else
			#pragma comment(lib, "TRTCS.lib")
		#endif
	#endif

	#if defined _RTCS_UDPDLL && !defined(RTCS_UDPO_EXPORTS)
		#ifdef _DEBUG
			#pragma comment(lib, "URTCSd.lib")
		#else
			#pragma comment(lib, "URTCS.lib")
		#endif
	#endif

	#if defined _INTP_TCPDLL && !defined(RTCS_INTP_EXPORTS)
		#ifdef _DEBUG
			#pragma comment(lib, "TINTPd.lib")
		#else
			#pragma comment(lib, "TINTP.lib")
		#endif
	#endif

	#if defined _INTP_UDPDLL && !defined(RTCS_INTP_EXPORTS)
		#ifdef _DEBUG
			#pragma comment(lib, "UINTPd.lib")
		#else
			#pragma comment(lib, "UINTP.lib")
		#endif
	#endif

	#if defined _ALARM_TCPDLL && !defined(POSTALARM_EXPORTS)
		#ifdef _DEBUG
			#pragma comment(lib, "TALARMd.lib")
		#else
			#pragma comment(lib, "TALARM.lib")
		#endif
	#endif

	#if defined _ALARM_UDPDLL && !defined(POSTALARM_EXPORTS)
		#ifdef _DEBUG
			#pragma comment(lib, "UALARMd.lib")
		#else
			#pragma comment(lib, "UALARM.lib")
		#endif
	#endif
#else
//use static lib
	#ifndef STR_VERSION_MESSAGE	
		#undef STR_VERSION_MESSAGE
		#define STR_VERSION_MESSAGE ", USE LIB: RTCS version is " STR_RTCS_VERSION
	#endif
	#ifdef _RTCS_TCPLIB
		#ifdef _DEBUG
			#pragma comment(lib, "RTCSServerd.lib")
		#else
			#pragma comment(lib, "RTCSServer.lib")
		#endif
	#endif

	#ifdef _RTCS_UDPLIB
		#ifdef _DEBUG
			#pragma comment(lib, "URTCSServerd.lib")
		#else
			#pragma comment(lib, "URTCSServer.lib")
		#endif
	#endif

	#ifdef _INTP_TCPLIB
		#ifdef _DEBUG
			#pragma comment(lib, "Intplibd.lib")
		#else
			#pragma comment(lib, "Intplib.lib")
		#endif
	#endif

	#ifdef _INTP_UDPLIB
		#ifdef _DEBUG
			#pragma comment(lib, "UIntplibd.lib")
		#else
			#pragma comment(lib, "UIntplib.lib")
		#endif
	#endif

	#ifdef _ALARM_TCPLIB
		#ifdef _DEBUG
			#pragma comment(lib, "PostAlarmd.lib")
		#else
			#pragma comment(lib, "PostAlarm.lib")
		#endif
	#endif

	#ifdef _ALARM_UDPLIB
		#ifdef _DEBUG
			#pragma comment(lib, "UPostAlarmd.lib")
		#else
			#pragma comment(lib, "UPostAlarm.lib")
		#endif
	#endif

#endif

//宏定义检查是否有冲突
#if defined (_RTCSDLL) && defined (_RTCSLIB)
	#error _RTCSDLL switch conflict with _RTCSLIB!
#endif
#ifdef VC

	#if !defined (_RTCSDLL) && !defined (_RTCSLIB)
		#error file must be compiled with _RTCSDLL or _RTCSLIB!
	#endif
#endif

//定义类导出或导入宏
#if defined(_RTCSDLL)
#define RTCS_API_EXPORT		__declspec(dllexport)
#define RTCS_CLASS_EXPORT	__declspec(dllexport)
#define RTCS_DATA_EXPORT	__declspec(dllexport)
#define RTCS_API_IMPORT		__declspec(dllimport)
#define RTCS_CLASS_IMPORT	__declspec(dllimport)
#define RTCS_DATA_IMPORT	__declspec(dllimport)
#else
#define RTCS_API_EXPORT
#define RTCS_CLASS_EXPORT
#define RTCS_DATA_EXPORT
#define RTCS_API_IMPORT
#define RTCS_CLASS_IMPORT
#define RTCS_DATA_IMPORT
#endif

#if defined(RTCS_TCPO_EXPORTS)
#undef RTCS_BASEOBJECT_EXPORTS
#define RTCS_BASEOBJECT_EXPORTS
#undef RTCS_TCP_EXPORTS
#define RTCS_TCP_EXPORTS
#endif

#if defined(RTCS_UDPO_EXPORTS)
#undef RTCS_BASEOBJECT_EXPORTS
#define RTCS_BASEOBJECT_EXPORTS
#undef RTCS_UDP_EXPORTS
#define RTCS_UDP_EXPORTS
#endif

#if defined(RTCS_BASEOBJECT_EXPORTS)
#define RTCS_BASEOBJECT_EXT_API			RTCS_API_EXPORT
#define RTCS_BASEOBJECT_EXT_CLASS		RTCS_CLASS_EXPORT
#define RTCS_BASEOBJECT_EXT_DATA		RTCS_DATA_EXPORT
#else
#define RTCS_BASEOBJECT_EXT_API			RTCS_API_IMPORT
#define RTCS_BASEOBJECT_EXT_CLASS		RTCS_CLASS_IMPORT
#define RTCS_BASEOBJECT_EXT_DATA		RTCS_DATA_IMPORT
#endif

#if defined(RTCS_TCP_EXPORTS)
#define RTCS_TCP_EXT_API				RTCS_API_EXPORT
#define RTCS_TCP_EXT_CLASS				RTCS_CLASS_EXPORT
#define RTCS_TCP_EXT_DATA				RTCS_DATA_EXPORT
#else
#define RTCS_TCP_EXT_API				RTCS_API_IMPORT
#define RTCS_TCP_EXT_CLASS				RTCS_CLASS_IMPORT
#define RTCS_TCP_EXT_DATA				RTCS_DATA_IMPORT
#endif

#if defined(RTCS_UDP_EXPORTS)
#define RTCS_UDP_EXT_API				RTCS_API_EXPORT
#define RTCS_UDP_EXT_CLASS				RTCS_CLASS_EXPORT
#define RTCS_UDP_EXT_DATA				RTCS_DATA_EXPORT
#else
#define RTCS_UDP_EXT_API				RTCS_API_IMPORT
#define RTCS_UDP_EXT_CLASS				RTCS_CLASS_IMPORT
#define RTCS_UDP_EXT_DATA				RTCS_DATA_IMPORT
#endif

#if defined(RTCS_INTP_EXPORTS)
#define RTCS_INTP_EXT_API				RTCS_API_EXPORT
#define RTCS_INTP_EXT_CLASS				RTCS_CLASS_EXPORT
#define RTCS_INTP_EXT_DATA				RTCS_DATA_EXPORT
#else
#define RTCS_INTP_EXT_API				RTCS_API_IMPORT
#define RTCS_INTP_EXT_CLASS				RTCS_CLASS_IMPORT
#define RTCS_INTP_EXT_DATA				RTCS_DATA_IMPORT
#endif


#include "rtcstype.h"

//用于控制DLL与EXE中对堆的管理
#ifdef _RTCSDLL
#define DECLARE_RTCSHEAP() \
	public: \
	void * operator new (size_t size) \
	{	return (::operator new(size));  } \
	void * operator new(size_t size, LPCSTR lpszFileName, int nLine) \
	{	return (::operator new(size));	} \
	void operator delete(void* p) \
	{	::operator delete(p);	} \
	void operator delete(void* p, LPCSTR lpszFileName, int nLine) \
	{	::operator delete(p);	} \

#else
#define DECLARE_RTCSHEAP() \

#endif

//实现DLL向后兼容定义接口宏
#define CLASS_UIID_DEF \
	static short GetClassUIID()\
	{\
		return 0;\
	}
#define OBJECT_UIID_DEF \
	virtual short GetObjectUIID()\
	{\
		return this->GetClassUIID();\
	}

// 所有回调的基本接口(虚函数)
class RTCS_BASEOBJECT_EXT_CLASS RTCS_ICallBack
{
public:
	CLASS_UIID_DEF
	OBJECT_UIID_DEF
public: //去除gcc编译告警
  virtual ~RTCS_ICallBack() {}
};

#undef CLASS_UIID_DEF
#define CLASS_UIID_DEF(BASE) \
	public: \
		static short GetClassUIID()\
		{\
			return BASE::GetClassUIID() + 1;\
		}

// 新增加的接口扩展方法
class RTCS_BASEOBJECT_EXT_CLASS RTCS_IBaseCallBack : public RTCS_ICallBack
{
public:
	CLASS_UIID_DEF(RTCS_ICallBack)
	OBJECT_UIID_DEF
	virtual void OnCallBack(int event) = 0; // 新的回调函数
};

//定义类成员变量接口
class RTCS_CPrivateData
{
public:
	RTCS_CPrivateData()
	{
//		memset(this,0,sizeof(RTCS_CPrivateData));
	}
	~RTCS_CPrivateData()
	{
//		memset(this,0,sizeof(RTCS_CPrivateData));
	}
};

//定义导出类接口
class RTCS_BASEOBJECT_EXT_CLASS RTCS_COject
{
public:
	RTCS_COject() : m_pHandler(0), m_pPrivateData(0) {}
	virtual ~RTCS_COject(){}
	RTCS_ICallBack * SetCallBackHandler(RTCS_ICallBack *handler)
	{
		return (m_pHandler = handler);
	}
	//调用回调函数方法
	//if( m_pHandler != NULL && m_pHandler->GetObjectUIID()>=ICallBack01::GetClassUIID())
	//{
	//    ((ICallBack01 *) mpHandler)->DoCallBack01(2);
	//}
private:
	RTCS_ICallBack     * m_pHandler;
	RTCS_CPrivateData  * m_pPrivateData;
};


//编译器输出
#ifdef WIN32
	#pragma message("  You are in Windows now" STR_BUILD_MESSAGE STR_VERSION_MESSAGE )
#endif

#endif
