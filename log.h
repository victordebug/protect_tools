#ifndef _LOG_H_
#define _LOG_H_

#define MAX_SIZE_STR 100

#define FILE_NAME_MD5_ENCRYPTION   ""

typedef enum type_File
{
	TYPE_MD5 = 0,
	TYPE_LOG,
	TYPE_MSG,

	TYPE_MAX,
}TYPE_FILE;

typedef enum log_file_ret
{
	LOG_OK   =   0,
	LOG_FAILE,
}LOG_FILE_RET;

int LOG_CreatLogFile(char *_logDirPath, TYPE_FILE _type_file);
int LOG_ClosePackageLog(TYPE_FILE _type_file);
int LOG_Info(char *_logInfoBuf, TYPE_FILE _type_file);
int LOG_GetCurTime(char *_time);


































#endif