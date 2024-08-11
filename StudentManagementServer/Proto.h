#pragma once

#pragma pack(push)
#pragma pack(1)

#include <Windows.h>

#define MYSQL_LOGIN						0
#define LOGIN_OK								1
#define LOGIN_ERR							    2

#define STU_ADD								    3
#define STU_ADD_OK						    4
#define STU_ADD_ERR						5
#define STU_DEL									6
#define STU_DEL_OK					        7
#define STU_DEL_ERR							8
#define STU_QUERY							9
#define STU_QUERY_RESULT				10
#define STU_QUERY_ERR					11
#define STU_QUERY_NULL					12
#define STU_EDIT									13
#define STU_EDIT_CHECK					14
#define STU_EDIT_NULL						15
#define STU_EDIT_OK							16
#define STU_EDIT_ERR						17
#define CLASS_ADD								18
#define CLASS_ADD_OK						19
#define CLASS_ADD_ERR					20
#define CLASS_DEL								21
#define CLASS_DEL_OK						22
#define CLASS_DEL_ERR						23
#define CLASS_QUERY						24
#define CLASS_QUERY_RESULT			25
#define CLASS_QUERY_ERR				26
#define CLASS_QUERY_NULL				27
#define CLASS_EDIT								28
#define CLASS_EDIT_CHECK				29
#define CLASS_EDIT_NULL					30
#define CLASS_EDIT_OK						31
#define CLASS_EDIT_ERR					32

#define COURSE_ADD                          33
#define COURSE_ADD_OK					  34
#define COURSE_ADD_ERR				  35
#define COURSE_DEL							  36
#define COURSE_DEL_OK					  37
#define COURSE_DEL_ERR					  38
#define COURSE_QUERY					      39
#define COURSE_QUERY_RESULT       40
#define COURSE_QUERY_ERR			  41
#define COURSE_QUERY_NULL			  42
#define COURSE_EDIT							  43
#define COURSE_EDIT_CHECK			  44
#define COURSE_EDIT_NULL				  45
#define COURSE_EDIT_OK					  46
#define COURSE_EDIT_ERR				  47

#define RECORD_ADD                          48
#define RECORD_ADD_OK					  49
#define RECORD_ADD_ERR				  50
#define RECORD_DEL							  51
#define RECORD_DEL_OK					  52
#define RECORD_DEL_ERR					  53
#define RECORD_QUERY					  54
#define RECORD_QUERY_RESULT       55
#define RECORD_QUERY_ERR			  56
#define RECORD_QUERY_NULL			  57
#define RECORD_EDIT							  58
#define RECORD_EDIT_CHECK			  59
#define RECORD_EDIT_NULL				  60
#define RECORD_EDIT_OK					  61
#define RECORD_EDIT_ERR				  62







typedef struct _stPacketHdr {
	short nCmd;
	DWORD nLen;
}stPacketHdr;

//typedef union _stPacket {
typedef struct  _stQueryPacket {
	char szQuery[MAXBYTE];
} stQueryPacket;


// 其他结构体...

//} stPacket;

#pragma pack(pop)