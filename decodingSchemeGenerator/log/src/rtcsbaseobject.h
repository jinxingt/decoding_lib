#ifndef __RTCSbaseObject_H_
#define __RTCSbaseObject_H_

#include <errno.h>
#include <assert.h>
#include "rtcs.h"
#include "rtcstype.h"
#include "rtcserror.h"
#include "rtcscstring.h"
#include <queue>
using std::queue;

#if defined(_SOLARIS) || defined(_HPUNIX) || defined(_AIX) || defined(_LINUX)

#define WAIT_OBJECT_0  0
#define INFINITE      -1

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>

#if defined(_AIX) || defined(_LINUX)
#include <sys/ioctl.h>
#endif

#ifdef _SOLARIS
#include <thread.h>
#include <signal.h>
#elif defined(_AIX)
#include <pthread.h>
#include <sys/sem.h>
#else
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#endif

#else
#include <windows.h>
#include <process.h>
#endif

#define RTCS_QUEUESIZE              4096
#define RTCS_VIPREMAINSIZE			3096
#define RTCS_TIMERINTERVAL1			50          // 50 ms
#define RTCS_TIMERINTERVAL2			200          // 200 ms
#define RTCS_TIMERSLICE				200000000   // 200 ms

#if defined(_SOLARIS) || defined(_HPUNIX) || defined(_AIX) || defined(_LINUX)
RTCS_CTuint GetTickCount();
void Sleep(RTCS_CTuint ms);
void OnSIGQUIT(int signal);
#endif

/* CriticalSection Lock:                                             */
/* The object was locked until Unlock() was called.                  */
class RTCS_BASEOBJECT_EXT_CLASS RTCS_Lock
{
public:
	RTCS_Lock();
	~RTCS_Lock();

	void Lock();
	void Unlock();

private:
#ifdef _SOLARIS
	mutex_t          m_mutex;
#elif defined(_HPUNIX) || defined(_AIX) || defined(_LINUX)
	pthread_mutex_t  m_mutex;
#else
	CRITICAL_SECTION m_CriticalSection;
#endif
};

class RTCS_BASEOBJECT_EXT_CLASS RTCS_SingleLock
{
public:
	RTCS_SingleLock(RTCS_Lock *lock);
	~RTCS_SingleLock();
	void       Lock();

private:
	RTCS_Lock* mlock;
};

/* Semaphone Lock:                                                   */
/* The object was locked until Unlock() was called or timeout.       */ 
class RTCS_BASEOBJECT_EXT_CLASS RTCS_Mutex
{
public:
	RTCS_Mutex();
	~RTCS_Mutex();

	RTCS_CTbool		Init(RTCS_CTbool bInitialOwner = RTCS_CTboolFALSE, char* mutexName = NULL);
	RTCS_CTbool		IsValid() const;
	RTCS_CTbool		Unlock();
	RTCS_CTuint		Lock(int delay = INFINITE);
	RTCS_CTbool		IsLocked();

private:
#ifdef _SOLARIS
	RTCS_CTbool		m_valid;
	mutex_t			m_mutex;
#elif defined(_HPUNIX) || defined(_AIX) || defined(_LINUX)
	RTCS_CTbool		m_valid;
	pthread_mutex_t	m_mutex;
#else
	HANDLE			m_Handle;
#endif
};

class RTCS_BASEOBJECT_EXT_CLASS RTCS_Semaphore
{
public:
	RTCS_Semaphore();
	~RTCS_Semaphore();

	RTCS_CTbool		Init(int initsps, int maxsps, char *spName = NULL);
	RTCS_CTbool		Post(int sps = 1);
	RTCS_CTuint		Wait(int delay = INFINITE);
	RTCS_CTbool		IsValid() const;

private:
#ifdef _SOLARIS
	RTCS_CTbool		m_valid;
	sema_t			m_sema;
#elif defined(_HPUNIX) || defined(_LINUX)
	RTCS_CTbool		m_valid;
	sem_t			m_sema;
#elif defined(_AIX)
	RTCS_CTbool		m_valid;
	int				m_semid;	
#else
	HANDLE			m_Handle;
#endif
};

#if defined(_SOLARIS)  || defined(_HPUNIX) || defined(_AIX) || defined(_LINUX)
	typedef void * (* RTCS_THREADFUNC)(void *);
#define THREAD_RESULT void *
	typedef RTCS_CTuint HANDLE;
#else
#	define THREAD_RESULT unsigned long __stdcall
	typedef unsigned long (__stdcall * RTCS_THREADFUNC)(void *);
#endif

class RTCS_BASEOBJECT_EXT_CLASS RTCS_Thread
{
public:
	RTCS_Thread(RTCS_THREADFUNC start_address, void* arglist = NULL, RTCS_CTbool initflag = RTCS_CTboolFALSE);
	~RTCS_Thread();

	static RTCS_CTuint GetCurrentThreadId();
	RTCS_CTuint		GetId() const;
	HANDLE			GetHandle() const;
	RTCS_CTuint		Suspend();
	RTCS_CTuint		Resume();
	RTCS_CTbool		Kill(RTCS_CTuint exitCode);
	RTCS_CTbool		Exit(RTCS_CTuint exitCode);
	RTCS_CTbool		IsValid() const;
	RTCS_CTbool		SetPriority(RTCS_CTint pri);
	RTCS_CTint		GetPriority() const;
	RTCS_CTbool		Running() const;
	void			Stop();

	void            NotifyStop();  //通知退出

	//RTCS_CTint      m_Flag;
	RTCS_CTbool     m_StopFlag;   //线程停止标识
#if defined(_HPUNIX) || defined(_AIX) || defined(_LINUX)
private:
	RTCS_CTuint GetNextPosixThreadId();
#endif 

private:
#ifdef _SOLARIS
	thread_t		m_ThreadId;
#elif defined(_HPUNIX) || defined(_AIX) || defined(_LINUX)
	pthread_t		m_ThreadId;
	RTCS_CTuint		m_PosixThreadId;
	RTCS_CTbool		m_IsCreated;
	RTCS_CTbool		m_IsSuspended;
	RTCS_THREADFUNC	m_ThreadFunc;
	void *			m_ArgList;
#else
	HANDLE			m_Handle;
	DWORD			m_ThreadId;
#endif
	RTCS_CTbool		m_Running;
};


template <class T>
class RTCS_BASEOBJECT_EXT_CLASS RTCS_Queue
{
public:
	RTCS_Queue(int nBlockSize = RTCS_QUEUESIZE, RTCS_CTint delay = RTCS_TIMERINTERVAL2);
	virtual ~RTCS_Queue();
	RTCS_CTuint		Push(T &elem);
	RTCS_CTuint		Pop(T& elem);
	RTCS_CTuint		TryPush(T &elem);
	RTCS_CTuint		TryPop(T& elem);
	RTCS_CTint		GetCount();
	RTCS_CTbool		IsEmpty();

private:
	RTCS_Lock		m_CrLock;
	RTCS_Semaphore	m_Food;
	RTCS_Semaphore	m_Buf;
	RTCS_CTint		m_MaxCount;//max buffer size
	RTCS_CTint		m_WaitTime;//ms
	queue<T>		m_queue;
};

// RTCS_Queue implementations
template <class T>
inline RTCS_Queue<T>::RTCS_Queue(int nBlockSize, RTCS_CTint delay)
{
	m_WaitTime = delay;
	m_MaxCount = nBlockSize;
	m_Food.Init(0,nBlockSize);
	m_Buf.Init(nBlockSize,nBlockSize);
}

template <class T>
inline RTCS_Queue<T>::~RTCS_Queue()
{
	m_CrLock.Lock();
	while (!m_queue.empty())
	{
		m_queue.pop();
	}
	m_CrLock.Unlock();
}

template <class T>
inline RTCS_CTint RTCS_Queue<T>::GetCount()
{
	m_CrLock.Lock();
	RTCS_CTint l_count = m_queue.size();
	m_CrLock.Unlock();
	return l_count;
}
template <class T>
inline RTCS_CTbool RTCS_Queue<T>::IsEmpty()
{
	m_CrLock.Lock();
	RTCS_CTbool emptysign = m_queue.empty();
	m_CrLock.Unlock();
	return emptysign;
}
template <class T>
inline RTCS_CTuint RTCS_Queue<T>::Push(T& elem)
{
	//申请缓冲区,wait方法默认传入无限等待时间
	if(m_Buf.Wait() != WAIT_OBJECT_0) {
		return RTCS_Error_ECTF_System;
	}

	//入队
	m_CrLock.Lock();
	m_queue.push(elem);
	m_CrLock.Unlock();
	
	//唤醒消费者
	m_Food.Post();
	
	return RTCS_Error_OK;
}


template <class T>
inline RTCS_CTuint RTCS_Queue<T>::TryPush(T& elem) 
{
	//WAIT_OBJECT_0表明m_Buf有信号，不等于表明等待超时。
	if(m_Buf.Wait(m_WaitTime) != WAIT_OBJECT_0) {
		return RTCS_Error_TimeOut;
	}

	m_CrLock.Lock();
	m_queue.push(elem);
	m_CrLock.Unlock();

	m_Food.Post();

	return RTCS_Error_OK;
}

template <class T>
inline RTCS_CTuint RTCS_Queue<T>::Pop(T& elem)
{
	RTCS_CTuint emptysign = 0;
	//获取一个元素
	if(m_Food.Wait() != WAIT_OBJECT_0)
		return RTCS_Error_ECTF_System;

	//出队列
	m_CrLock.Lock();
	if(m_queue.empty())
		emptysign = 1;
	else{
		elem = m_queue.front();
		m_queue.pop();
		emptysign = 0;
	}
	m_CrLock.Unlock();

	//表明缓冲区多了一个
	m_Buf.Post();	
	return emptysign;
}
template <class T>
inline RTCS_CTuint RTCS_Queue<T>::TryPop(T& elem)
{
	RTCS_CTuint emptysign = 0;
	//WAIT_OBJECT_0表明m_Food有信号，不等于表明等待超时。
	if(m_Buf.Wait(m_WaitTime) != WAIT_OBJECT_0) {
		return RTCS_Error_TimeOut;
	}

	//出队列
	m_CrLock.Lock();
	if(m_queue.empty())
		emptysign = 1;
	else{
		elem = m_queue.front();
		m_queue.pop();
		emptysign = 0;
	}
	m_CrLock.Unlock();
	//表明缓冲区多了一个
	m_Buf.Post();	
	return emptysign;
}
// end of RTCS_Queue implementations


#if defined(_SOLARIS) || defined(_HPUNIX) || defined(_AIX) || defined(_LINUX)
extern int GetPrivateProfileString(const char* section, const char* key, const char* sdefault, char *dest, int size, const char* filename);
extern int GetPrivateProfileInt(const char* section, const char* key, int ndefault, const char* filename);
extern bool WritePrivateProfileString(const char* section, const char* key, const char* value, const char* filename);
extern bool WritePrivateProfileInt(const char* section, const char* key, int value, const char* filename);

#define  GetCurrentProcessId getpid
#else
#endif
#endif//__RTCSbaseObject_H_
