#ifndef __RTCSerror_H_
#define __RTcSerror_H_

// RTCS_CTbool true and false

#define	RTCS_CTboolTRUE				1
#define RTCS_CTboolFALSE			0

// end RTCS_CTbool true and false

#define MakeECTFError(errorid)								(0x10000 + errorid)

#define MakeRTCSError(errorid)								(0x20000 + errorid)

// RTCS errror macro

#define RTCS_Error_ECTF_OK								0x0
#define RTCS_Error_ECTF_BadKey							MakeECTFError( 0x001 )
#define RTCS_Error_ECTF_BadKVSet						MakeECTFError( 0x002 )
#define RTCS_Error_ECTF_BadValueType					MakeECTFError( 0x003 )
#define RTCS_Error_ECTF_EndOfKeys						MakeECTFError( 0x004 )
#define RTCS_Error_ECTF_InvalidFile						MakeECTFError( 0x005 )
#define RTCS_Error_ECTF_KeyNotFound						MakeECTFError( 0x006 )
#define RTCS_Error_ECTF_NullArray						MakeECTFError( 0x007 )
#define RTCS_Error_ECTF_System							MakeECTFError( 0x008 )
#define RTCS_Error_ECTF_ValueIsArray					MakeECTFError( 0x009 )
#define RTCS_Error_ECTF_ValueNotArray					MakeECTFError( 0x00a )

#define RTCS_Error_ECTF_Warning							MakeECTFError( 0x00b )
#define RTCS_Error_ECTF_Started							MakeECTFError( 0x00c )	
#define RTCS_Error_ECTF_Fail							MakeECTFError( 0x00d )
#define RTCS_Error_ECTF_Fatal							MakeECTFError( 0x00e )
#define RTCS_Error_ECTF_Destroy							MakeECTFError( 0x00f )
#define RTCS_Error_ECTF_Badflow							MakeECTFError( 0x010 )
#define RTCS_Error_ECTF_Badflowhead						MakeECTFError( 0x011 )
#define RTCS_Error_ECTF_Badflowend						MakeECTFError( 0x012 )
#define RTCS_Error_ECTF_InvalidFlow						MakeECTFError( 0x013 )
#define RTCS_Error_ECTF_IncompleteFlow					MakeECTFError( 0x014 )

/* RTCS error macro */
#define RTCS_Error_OK									RTCS_Error_ECTF_OK
#define RTCS_Error_FAIL									RTCS_Error_ECTF_Fail

#define RTCS_Error_NoMemory								MakeRTCSError( 0x001 )
#define RTCS_Error_TimeOut								MakeRTCSError( 0x002 )
#define RTCS_Error_InvalidStation						MakeRTCSError( 0x003 )
#define RTCS_Error_InvalidCOH							MakeRTCSError( 0x004 )
#define RTCS_Error_InvalidRequest						MakeRTCSError( 0x005 )
#define RTCS_Error_NoBus								MakeRTCSError( 0x006 )
#define RTCS_Error_ServerNoReady						MakeRTCSError( 0x007 )
#define RTCS_Error_InvalidServer						MakeRTCSError( 0x008 )
#define	RTCS_Error_InvalidOperator						MakeRTCSError( 0x009 )
#define RTCS_Error_QueueEmpty							MakeRTCSError( 0x010 )
#define RTCS_Error_InvalidConnect                       MakeRTCSError( 0x011 )
#define RTCS_Error_InvalidGOR							MakeRTCSError( 0x012 )
#define RTCS_Error_InvalidClient						MakeRTCSError( 0x013 )
#define RTCS_Error_QueueFull							MakeRTCSError( 0x014 )
#define RTCS_Error_NULL									MakeRTCSError( 0x015 )

#endif
