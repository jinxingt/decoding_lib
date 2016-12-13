/*
	RTCS GOR macro define  

	SubSystem: RTCS ---- Real Time Communication System

	Version: v1.0 

	Decription: 
		RTCS Version v1.0 only support inter Thread communication, and only support Windows platform.
		
		All RTCS Client Object use GOR to indicate sender and receiver.

		GOR is RTCS_CTuint interger, include:
		|--------------------------------------------------------------|
		|	RTCS Server Object Handle	| RTCS Client Object Handle    |
		|-------------------------------|------------------------------|
		|	(High 16 bit)				|	(Low 16 bit)			   |
		|-------------------------------|------------------------------|


		COH is Client Object Handle

*/

#ifndef __RTCSgor_H_
#define __RTCSgor_H_

#define ALL_GOR			0
#define INVALID_GOR		0xFFFFFFFF

#define ALL_STATION		0
#define INVALID_STATION  0xFFFF0000

#define ALL_COH			0
#define INVALID_COH		0xFFFF


// use this macro to make gor 
#define MAKEGOR(serverid, clientid)				( (serverid << 16) | clientid )

// use macro to get server object handle or client object handle from gor
#define GETSTATION(gor)							( (gor >> 16) & 0xFFFF )
#define GETCLIENT(gor)							( gor & 0xFFFF )

// define client Object Handle here
// range from 1 ---- 0xF000,	other reserved.
// Object Handle must use:  COH_xxx
// example: COH_ACD

#endif
