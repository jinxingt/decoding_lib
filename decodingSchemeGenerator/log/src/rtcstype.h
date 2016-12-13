#ifndef __RTCStype_H_
#define __RTCStype_H_

//declare
class RTCS_Lock;
class RTCS_SingleLock;
class RTCS_Mutex;
class RTCS_Semaphore;
class RTCS_Thread;
class RTCS_Node;
class RTCS_NodeList;
class RTCS_CTstring;
class RTCS_TraceLog;
class RTCS_CTime;
class RTCS_Socket;
class RTCS_CTkvset;
class RTCS_CTpdu;

typedef unsigned int	 	RTCS_CTbool;
typedef unsigned char	    RTCS_CTbyte;
typedef unsigned short	    RTCS_CTushort;
typedef int				    RTCS_CTint;
typedef	unsigned int	    RTCS_CTuint;
#if defined(_WIN64) || defined(__x86_64__) || defined (__64BIT__)
	typedef long 				RTCS_CT2int;
	typedef unsigned long 		RTCS_CT2uint;
#else
#	ifdef _MSC_VER
	typedef __int64    			RTCS_CT2int;	
	typedef unsigned __int64    RTCS_CT2uint;	
#	else
  	typedef long long  			RTCS_CT2int;
  	typedef unsigned long long  RTCS_CT2uint;
#	endif
#endif
typedef float			    RTCS_CTfloat;
typedef	double			    RTCS_CTdouble;
typedef unsigned char*      RTCS_CTbyteArray;
typedef unsigned short*     RTCS_CTushortArray;
typedef	int*			    RTCS_CTintArray;
typedef unsigned int*      	RTCS_CTuintArray;
typedef RTCS_CT2int*		RTCS_CT2intArray;
typedef RTCS_CT2uint*		RTCS_CT2uintArray;
typedef RTCS_CTstring*	    RTCS_CTstringArray;
typedef float*			    RTCS_CTfloatArray;
typedef double*			    RTCS_CTdoubleArray;
typedef RTCS_CTkvset*	    RTCS_CTkvsetArray;

//注释：对于某些程序的编译可能需要socket1.1版本
//针对此，可以取消下面的注释行，并重新编译RTCS的连接库
//#undef _WIN32_WINNT
//#define _WIN32_WINNT 0x0300

#ifdef _WIN32
# ifdef _RTCS_DEBUG
#   ifdef _RTCSDLL
#     error not support _RTCS_DEBUG switch in dll
#   endif
#   include <afx.h>
# endif
# ifndef _WIN32_WINNT
#   define _WIN32_WINNT 0x0500
# endif
# include <windows.h>
# pragma comment(lib,"user32.lib")

# ifndef DEBUG_NEW
#   define DEBUG_NEW new
# endif
#else //LINUX
  typedef const char*	LPCTSTR;
  typedef char*		LPCSTR;
  typedef unsigned char BYTE;
  typedef unsigned short WORD;
  typedef unsigned long DWORD;
# define DEBUG_NEW new
#endif //LINUX

#ifdef WIN32
#	ifndef _INC_IO
#		include <io.h>
#	endif
#	ifdef _WINSOCK2API_
#		pragma comment(lib,"ws2_32.lib")
#	else 
#		ifdef _WINSOCKAPI_
#			pragma comment(lib,"wsock32.lib")
#		else
#			include <winsock2.h>
#			pragma comment(lib,"ws2_32.lib")
#		endif
#	endif //WINSOCKET
#endif //WIN32

#include "rtcsgor.h"
#include "rtcserror.h"

#ifndef _INC_STRING
# include <string.h>
#endif
#ifndef _INC_STDIO
# include <stdio.h>
#endif
#ifndef _INC_STDLIB
# include <stdlib.h>
#endif
#ifndef _INC_TIME
# include <time.h>
#endif
#ifndef _INC_LIMITS
# include <limits.h>
#endif
#ifndef _INC_STDDEF
# include <stddef.h>
#endif
#ifndef _INC_STDARG
# include <stdarg.h>
#endif
#ifndef DEF_ASSERT
# define DEF_ASSERT
# include <assert.h>
#endif

#ifdef _NOWARNING
# pragma warning(disable:4018) //signed/unsigned mismatch
# pragma warning(disable:4244) //conversion from x to x, possible loss of data
#endif

//FIXME: should be comment out on VC9.0(1500)
#if (defined(_MSC_VER))
#	define snprintf _snprintf
#	define vsnprintf _vsnprintf
#endif

#endif
