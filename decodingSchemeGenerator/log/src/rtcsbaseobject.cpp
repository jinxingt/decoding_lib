#include "rtcsbaseobject.h"

#if defined(_SOLARIS) || defined(_HPUNIX) || defined(_AIX) || defined(_LINUX)
RTCS_CTuint GetTickCount()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (((RTCS_CTuint)tv.tv_sec) * 1000 + ((RTCS_CTuint)tv.tv_usec) / 1000);
}

void Sleep(RTCS_CTuint ms)
{
#	if defined(_AIX)
	if (ms<=0)
	{
		usleep(0);
	}
	else
	{		
		RTCS_CTuint usec=ms*1000;
		usleep(usec);		
	}
#	else
	struct timespec ts;
	ts.tv_sec = ms / 1000;
	ts.tv_nsec = (ms % 1000) * 1000000L;
	nanosleep(&ts, NULL);
#	endif
}

void OnSIGQUIT(int signal)
{
#	if defined(_SOLARIS)
	thr_exit(0);
#	else
	pthread_exit(0);
#	endif
}
#endif

RTCS_Lock::RTCS_Lock()
{
#ifdef _SOLARIS
	mutex_init(&m_mutex, USYNC_THREAD, NULL);
#elif defined(_HPUNIX) || defined(_AIX) || defined(_LINUX)
	//设置锁的互斥类型，为了支持在一个线程内部多次调用lock操作而不阻塞
	//缺省为PTHREAD_MUTEX_NORMAL ,我们把其设置为PTHREAD_MUTEX_RECURSIVE
	pthread_mutexattr_t attr;
	pthread_mutexattr_init (&attr);
	pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_RECURSIVE);	
	pthread_mutex_init(&m_mutex,&attr);
	pthread_mutexattr_destroy (&attr);
#else
	::InitializeCriticalSection( &m_CriticalSection ); 
#endif
}

RTCS_Lock::~RTCS_Lock()
{
#ifdef _SOLARIS
	mutex_destroy( &m_mutex );
#elif defined(_HPUNIX) || defined(_AIX) || defined(_LINUX)
	pthread_mutex_destroy( &m_mutex );
#else
	::DeleteCriticalSection( &m_CriticalSection ); 
#endif
}

void RTCS_Lock::Lock()
{
#ifdef _SOLARIS
	mutex_lock( &m_mutex );
#elif defined(_HPUNIX) || defined(_AIX) || defined(_LINUX)
	pthread_mutex_lock( &m_mutex );
#else
	::EnterCriticalSection( &m_CriticalSection ); 
#endif
}

void RTCS_Lock::Unlock() 
{
#ifdef _SOLARIS
	mutex_unlock( &m_mutex );
#elif defined(_HPUNIX) || defined(_AIX) || defined(_LINUX)
	pthread_mutex_unlock( &m_mutex );
#else
	::LeaveCriticalSection( &m_CriticalSection ); 
#endif
}


RTCS_SingleLock::RTCS_SingleLock(RTCS_Lock *lock)
	: mlock(lock)
{
}

RTCS_SingleLock::~RTCS_SingleLock()
{
	if(mlock) mlock->Unlock();
}

void RTCS_SingleLock::Lock()
{
	if(mlock) mlock->Lock();
}


RTCS_Mutex::RTCS_Mutex()
#ifdef _SOLARIS
	: m_valid(RTCS_CTboolFALSE)
#elif defined(_HPUNIX) || defined(_AIX) || defined(_LINUX)
	: m_valid(RTCS_CTboolFALSE)
#else
	: m_Handle(0)
#endif
{
}

RTCS_Mutex::~RTCS_Mutex()
{
#ifdef _SOLARIS
	if(m_valid) mutex_destroy(&m_mutex);
#elif defined(_HPUNIX) || defined(_AIX) || defined(_LINUX)
	if(m_valid) pthread_mutex_destroy(&m_mutex);
#else
	if(IsLocked()) Unlock();
#endif
}

RTCS_CTbool RTCS_Mutex::Init(RTCS_CTbool bInitialOwner, char* mutexName)
{
#ifdef _SOLARIS
	if(mutex_init(&m_mutex, USYNC_THREAD, NULL))
		m_valid = RTCS_CTboolFALSE;
	else
		m_valid = RTCS_CTboolTRUE;
	return m_valid;
#elif defined(_HPUNIX) || defined(_AIX) || defined(_LINUX)
	if(pthread_mutex_init(&m_mutex, NULL))
		m_valid = RTCS_CTboolFALSE;
	else
		m_valid = RTCS_CTboolTRUE;
	return m_valid;
#else
	if(bInitialOwner == RTCS_CTboolTRUE)
		m_Handle = ::CreateMutex(0,RTCS_CTboolTRUE,mutexName);
	else
		m_Handle = ::CreateMutex(0,RTCS_CTboolFALSE,mutexName);
	
	if(m_Handle)
		return RTCS_CTboolTRUE;
	else
		return RTCS_CTboolFALSE;
#endif
}

RTCS_CTbool RTCS_Mutex::IsValid() const
{
#if defined(_SOLARIS) || defined(_HPUNIX) || defined(_AIX) || defined(_LINUX)
	return m_valid;
#else
	if(m_Handle)
		return RTCS_CTboolTRUE;
	else
		return RTCS_CTboolFALSE;
#endif
}

RTCS_CTbool RTCS_Mutex::Unlock()
{
#ifdef _SOLARIS
	return !mutex_unlock(&m_mutex);
#elif defined(_HPUNIX) || defined(_AIX) || defined(_LINUX)
	return !pthread_mutex_unlock(&m_mutex);
#else
	if(::ReleaseMutex(m_Handle))
		return RTCS_CTboolTRUE;
	else
		return RTCS_CTboolFALSE;
#endif
}

/*	WaitForSingleObject() return value:
	WAIT_ABANDONED The specified object is a mutex object that was not released
	by the thread that owned the mutex object before the owning thread terminated.
	Ownership of the mutex object is granted to the calling thread,
	and the mutex is set to nonsignaled. 
	WAIT_OBJECT_0 The state of the specified object is signaled. 
	WAIT_TIMEOUT The time-out interval elapsed, and the object's state is nonsignaled. 
*/
RTCS_CTuint RTCS_Mutex::Lock(int delay)
{
#ifdef _SOLARIS
	return mutex_lock(&m_mutex);
#elif defined(_HPUNIX) || defined(_AIX) || defined(_LINUX)
	return pthread_mutex_lock(&m_mutex);
#else
	return ::WaitForSingleObject(m_Handle, delay);
#endif
}

RTCS_CTbool RTCS_Mutex::IsLocked()
{
#ifdef _SOLARIS
		if(mutex_trylock(&m_mutex))
		{
			return RTCS_CTboolFALSE;
		}
		else
		{
			mutex_unlock(&m_mutex);
			return RTCS_CTboolTRUE;
		}
#elif defined(_HPUNIX) || defined(_AIX) || defined(_LINUX)
	if(pthread_mutex_trylock(&m_mutex)) {
		return RTCS_CTboolFALSE;
	} else {
		pthread_mutex_unlock(&m_mutex);
		return RTCS_CTboolTRUE;
		}
#else
	return WAIT_TIMEOUT==::WaitForSingleObject(m_Handle, 0);
#endif
}


RTCS_Semaphore::RTCS_Semaphore()
#if defined(_SOLARIS) || defined(_HPUNIX) || defined(_AIX) || defined(_LINUX)
	: m_valid(RTCS_CTboolFALSE)
#else
	: m_Handle(0)
#endif
{
}

RTCS_Semaphore::~RTCS_Semaphore()
{
#ifdef _SOLARIS
	if(m_valid) sema_destroy(&m_sema);
#elif defined(_HPUNIX) || defined(_LINUX)
	if(m_valid) sem_destroy(&m_sema);
#elif defined(_AIX)
	if(m_valid) semctl(m_semid, 0, IPC_RMID, 0);
#else
	if(m_Handle) CloseHandle(m_Handle);
#endif
}

RTCS_CTbool RTCS_Semaphore::Init(int initsps, int maxsps, char* spName)
{
#ifdef _SOLARIS
	if(m_valid) sema_destroy(&m_sema);
#elif defined(_HPUNIX) || defined(_LINUX)
	if(m_valid) sem_destroy(&m_sema);
#elif defined(_AIX)
	if(m_valid) semctl(m_semid, 0, IPC_RMID, 0);
#else
	if(m_Handle) CloseHandle(m_Handle);
#endif

#ifdef _SOLARIS
	m_valid = !sema_init(&m_sema, initsps, USYNC_THREAD, NULL);
	return m_valid;
#elif defined(_HPUNIX) || defined(_LINUX)
	m_valid = !sem_init(&m_sema,0, initsps);
	return m_valid;
#elif defined(_AIX)
	m_semid = semget(IPC_PRIVATE, 1, IPC_CREAT|0660);
	if (m_semid < 0)
	{
		m_valid = RTCS_CTboolFALSE;
		return m_valid;
	}
	union semun {
		int val;
		struct semid_ds *buf;
		unsigned short *array;
	};
	union semun semopts;
	semopts.val = initsps;
	if (semctl(m_semid, 0, SETVAL, semopts) < 0)
	{
		m_valid = RTCS_CTboolFALSE;
		return m_valid;
	}

	m_valid = RTCS_CTboolTRUE;
	return m_valid;

#else
	m_Handle = ::CreateSemaphore(NULL,initsps,maxsps,spName);
	if(m_Handle)
		return RTCS_CTboolTRUE;
	else
		return RTCS_CTboolFALSE;
#endif
}

RTCS_CTbool RTCS_Semaphore::Post(int sps)
{
#ifdef _SOLARIS
	return !sema_post(&m_sema);
#elif defined(_HPUNIX) || defined(_LINUX)
	return !sem_post(&m_sema);
#elif defined(_AIX)
	struct sembuf sb;
	sb.sem_num = 0;
	sb.sem_op = sps;
	sb.sem_flg = 0;
	return !semop(m_semid, &sb, 1);
#else
	if(::ReleaseSemaphore(m_Handle,sps,NULL))
		return RTCS_CTboolTRUE;
	else
		return RTCS_CTboolFALSE;
#endif
}

RTCS_CTuint RTCS_Semaphore::Wait(int delay)
{ 
#ifdef _SOLARIS
	int rc = 0;
	if(delay == INFINITE)
	{
		while((rc=sema_wait(&m_sema)) == EINTR) ;
	}
	else
	{
		for(int i=0; i<=(delay/10); i++)
		{
			if((rc=sema_trywait(&m_sema)) == 0) //this function may cost 10 ms
				return 0;
			Sleep(10);
		}
	}
	return rc;
#elif defined(_HPUNIX) || defined(_LINUX)
	int rc = 0;
	if(delay == INFINITE)
	{
		while((rc=sem_wait(&m_sema)) == EINTR) ;
	}
	else
	{
		for(int i=0; i<=(delay/10); i++)
		{
			if((rc=sem_trywait(&m_sema)) == 0) //this function may cost 10 ms
				return 0;
			Sleep(10);
		}
	}
	return rc;
#elif defined(_AIX)
	int rc = 0;
	struct sembuf sb;
	sb.sem_num = 0;
	sb.sem_op = -1;
	sb.sem_flg = 0;

	if(delay == INFINITE)
	{
		while((rc=semop(m_semid, &sb, 1)) == EINTR) ;
	}
	else
	{
		sb.sem_flg = IPC_NOWAIT;
		for(int i=0; i<=(delay/10); i++)
		{
			if((rc=semop(m_semid, &sb, 1)) == 0) //this function may cost 10 ms
				return 0;
			Sleep(10);
		}
	}
	return rc;
#else	
	return ::WaitForSingleObject(m_Handle,delay);
#endif
}

RTCS_CTbool RTCS_Semaphore::IsValid() const
{
#if defined(_SOLARIS) || defined(_HPUNIX) || defined(_AIX) || defined(_LINUX)
	return m_valid;
#else
	if(m_Handle)
		return RTCS_CTboolTRUE;
	else
		return RTCS_CTboolFALSE;
#endif
}

//static 
RTCS_CTuint RTCS_Thread::GetCurrentThreadId()
{
#ifdef _SOLARIS
	return (RTCS_CTuint)thr_self();
#elif defined(_HPUNIX) || defined(_AIX) || defined(_LINUX)
	return (RTCS_CTuint)pthread_self();
#else
	return (RTCS_CTuint)::GetCurrentThreadId();
#endif
}

#if defined(_HPUNIX) || defined(_AIX) || defined(_LINUX)
RTCS_CTuint RTCS_Thread::GetNextPosixThreadId()
{
	
	static RTCS_CTuint PosixThreadIdIndex = 0;  

	if (PosixThreadIdIndex>=0xFFFFFFFF)
	{
		PosixThreadIdIndex=0;
		return PosixThreadIdIndex;
	}
	else
	{
		PosixThreadIdIndex++;
		return PosixThreadIdIndex;
	}
}
#endif

RTCS_Thread::RTCS_Thread(RTCS_THREADFUNC start_address, void* arglist, RTCS_CTbool initflag)
{
	m_StopFlag = RTCS_CTboolFALSE;
#ifdef _SOLARIS
	sigset(SIGQUIT, OnSIGQUIT);
	if(initflag)
		thr_create(0,0,start_address,arglist,0,&m_ThreadId);
	else
		thr_create(0,0,start_address,arglist,THR_SUSPENDED,&m_ThreadId);

#elif defined(_HPUNIX) || defined(_AIX) || defined(_LINUX)
	int rc=0;
#	if defined(_LINUX)
	signal(SIGQUIT, OnSIGQUIT);
#	else
	sigset(SIGQUIT, OnSIGQUIT);
#	endif
	if (initflag)
	{
		m_PosixThreadId=GetNextPosixThreadId();
		m_IsCreated=RTCS_CTboolFALSE;
		m_IsSuspended=RTCS_CTboolTRUE;
		m_ThreadFunc=start_address;
		m_ArgList=arglist;

		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setstacksize(&attr, 2*1024*1024);  //设置栈空间为2M，windows默认1M
		rc = pthread_create(&m_ThreadId, &attr, start_address, arglist);
		pthread_attr_destroy(&attr);

		if (rc == 0)
		{
			m_IsCreated=RTCS_CTboolTRUE;
			m_IsSuspended=RTCS_CTboolFALSE;
		}
		pthread_detach(m_ThreadId);
	}
	else
	{
		m_PosixThreadId=GetNextPosixThreadId();
		m_IsCreated=RTCS_CTboolFALSE;
		m_IsSuspended=RTCS_CTboolTRUE;
		m_ThreadFunc=start_address;
		m_ArgList=arglist;

	}

#else
	if (initflag)
		m_Handle = ::CreateThread(0,0,start_address,arglist,0,&m_ThreadId);

	else
		m_Handle = ::CreateThread(0,0,start_address,arglist,CREATE_SUSPENDED,&m_ThreadId);

#endif
	m_Running = RTCS_CTboolTRUE;
}

RTCS_Thread::~RTCS_Thread()
{
	Stop();
#if defined(_SOLARIS) || defined(_HPUNIX) || defined(_AIX) || defined(_LINUX)
#else
	CloseHandle(m_Handle);
#endif
}

RTCS_CTuint RTCS_Thread::GetId() const
{ 
	return m_ThreadId;
}

HANDLE RTCS_Thread::GetHandle() const
{	
#if defined(_SOLARIS) || defined(_HPUNIX) || defined(_AIX) || defined(_LINUX)
	return m_ThreadId;
#else
	return m_Handle;
#endif
}

RTCS_CTuint RTCS_Thread::Suspend()
{
#ifdef _SOLARIS
	return thr_suspend(m_ThreadId);
#elif defined(_HPUNIX) || defined(_AIX)
	int rc=0;
	if (m_IsCreated==RTCS_CTboolTRUE && m_IsSuspended==RTCS_CTboolFALSE)
	{
		rc=pthread_suspend(m_ThreadId);
		if (rc==0) m_IsSuspended=RTCS_CTboolTRUE;
		return rc;
	}
	else if (m_IsCreated==RTCS_CTboolTRUE && m_IsSuspended==RTCS_CTboolTRUE)
	{
		return 0;
	}
	else
	{
		return -1;
	}
#elif defined(_LINUX)
	return 0;
#else
	return ::SuspendThread(m_Handle);
#endif
}

RTCS_CTuint RTCS_Thread::Resume()
{
#ifdef _SOLARIS
	return thr_continue(m_ThreadId);
#elif defined(_HPUNIX) || defined(_AIX) || defined(_LINUX)
	int rc=0;
	if (m_IsCreated==RTCS_CTboolFALSE)
	{
		if (m_PosixThreadId==0)
			m_PosixThreadId=GetNextPosixThreadId();
		
		RTCS_THREADFUNC start_address=m_ThreadFunc;
		void *arglist=m_ArgList;

		rc=pthread_create(&m_ThreadId,NULL,start_address,arglist);
		if (rc==0) 
		{
			m_IsCreated=RTCS_CTboolTRUE;
			m_IsSuspended=RTCS_CTboolFALSE;
		}
		pthread_detach(m_ThreadId);
		return rc;
	}
	else if (m_IsCreated==RTCS_CTboolTRUE && m_IsSuspended==RTCS_CTboolTRUE)
	{	
#	ifndef _LINUX
		rc = pthread_continue(m_ThreadId);
#	endif
		if (rc==0) m_IsSuspended=RTCS_CTboolFALSE;
		return rc;
	}
	else
	{
		return 0;
	}
#else
	return ::ResumeThread(m_Handle);
#endif
}

/*
 TerminateThread() is a dangerous function that should only be used in the most extreme cases. 
 You should call TerminateThread only if you know exactly what the target thread is doing, 
 and you control all of the code that the target thread could possibly be running at the time
 of the termination. For example, TerminateThread can result in the following problems: 
     * If the target thread owns a critical section, the critical section will not be released. 
     * If the target thread is executing certain kernel32 calls when it is terminated,
	   the kernel32 state for the thread's process could be inconsistent. 
     * If the target thread is manipulating the global state of a shared DLL,
	   the state of the DLL could be destroyed, affecting other users of the DLL. 
*/
RTCS_CTbool RTCS_Thread::Kill(RTCS_CTuint exitCode)
{
#ifdef _SOLARIS
	return !thr_kill(m_ThreadId, SIGQUIT);
#elif defined(_HPUNIX) || defined(_AIX) || defined(_LINUX)
	return !pthread_kill(m_ThreadId, SIGQUIT);
#else
	return ::TerminateThread(m_Handle,exitCode);
#endif
}

RTCS_CTbool RTCS_Thread::Exit(RTCS_CTuint exitCode)
{
#ifdef _SOLARIS
	if (thr_self() == m_ThreadId)
	{
		thr_exit(0);
#elif defined(_HPUNIX) || defined(_AIX) || defined(_LINUX)
	if(pthread_self() == m_ThreadId)
	{
		pthread_exit(0);
#else
	if (GetCurrentThreadId() == m_ThreadId)	
	{
		ExitThread(exitCode);

#endif
		return RTCS_CTboolTRUE;
	}
	return RTCS_CTboolFALSE;
}

RTCS_CTbool RTCS_Thread::IsValid() const
{
#ifdef _SOLARIS
	return m_ThreadId != 0;
#elif defined(_HPUNIX) || defined(_AIX) || defined(_LINUX)
	return m_ThreadId != 0;
#else
	if(m_Handle)
		return RTCS_CTboolTRUE;
	else
		return RTCS_CTboolFALSE;
#endif
}

RTCS_CTbool RTCS_Thread::SetPriority(RTCS_CTint pri)
{
#ifdef _SOLARIS
	return !thr_setprio(m_ThreadId, pri);
#elif defined(_HPUNIX) || defined(_AIX) || defined(_LINUX)
	int rc=0; 
	sched_param param; 	
	param.sched_priority = pri;
	int policy = SCHED_OTHER;
	rc = pthread_setschedparam (m_ThreadId, policy, &param);
	if (rc==0)
		return RTCS_CTboolTRUE;
	else
		return RTCS_CTboolFALSE;
#else
	if(::SetThreadPriority(m_Handle,pri))
		return RTCS_CTboolTRUE;
	else
		return RTCS_CTboolFALSE;
#endif
}

RTCS_CTint RTCS_Thread::GetPriority() const
{
#ifdef _SOLARIS
	int pri;
	if(!thr_getprio(m_ThreadId, &pri))
		return pri;
	else
		return -1;
#elif defined(_HPUNIX) || defined(_AIX) || defined(_LINUX)
	int rc=0; 
	sched_param param; 	
	int policy; 
	int pri=-1; 
	rc = pthread_getschedparam (m_ThreadId, &policy, &param); 
	if (rc==0)
	{
		pri = param.sched_priority;
	}	
	return pri;
#else
	return ::GetThreadPriority(m_Handle);
#endif
}

RTCS_CTbool RTCS_Thread::Running() const
{
	return (m_Running && !m_StopFlag);
}

void RTCS_Thread::NotifyStop()
{
	if (Running())
	{
		Resume();
	}
	m_StopFlag = RTCS_CTboolTRUE;
}

void RTCS_Thread::Stop()
{
	if (Running())
	{
		Resume();
	}
#ifdef _SOLARIS
	if( m_Running != RTCS_CTboolFALSE)
	{
		m_Running = RTCS_CTboolFALSE;
		//Resume();
		if(thr_self() == m_ThreadId) return;
		Sleep(60);
		if (!m_StopFlag) //if(m_Flag != 1)
		{
			Kill(0);
		}
	}
#elif defined(_HPUNIX) || defined(_AIX) || defined(_LINUX)
	if( m_Running != RTCS_CTboolFALSE)
	{
		m_StopFlag = RTCS_CTboolTRUE;
		m_Running = RTCS_CTboolFALSE;
		//Resume();
		if(pthread_self() == m_ThreadId) return;

		int cnt = 40;
		do 
		{
			cnt--;
			Sleep(50);
		} while (cnt && pthread_kill(m_ThreadId, 0) != ESRCH);

		if (cnt == 0)
			Kill(0);
	}
#else
	if( m_Running != RTCS_CTboolFALSE)
	{
		m_Running = RTCS_CTboolFALSE;
		//Resume();
		if(GetCurrentThreadId() == m_ThreadId) 
			return; // 不要自己等自己
		/*
		RTCS_CTuint status;
		int i;
		for(i=0; i<200; i++) 
		{
			if( ::GetExitCodeThread(m_Handle, &status) && status==STILL_ACTIVE ) Sleep(10);
			else break;
		}
		if(i >= 200) 
			Kill(0); //2秒还不停下来就把它KILL掉*/
		if (::WaitForSingleObject(m_Handle, 5000) != WAIT_OBJECT_0)
			Kill(0); //5秒还不停下来就把它KILL掉
		return;
	}
#endif
}