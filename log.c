#include "log.h"
#include "main.h"
#include "package.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <sys/types.h>  /*提供类型pid_t,size_t的定义*/
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>


FILE *LogFd, *Md5Fd;


int LOG_GetCurTime(char *_time)
{
	time_t t;
	struct tm *tmp_time;

	time(&t);
	tmp_time = localtime(&t);
	strftime(_time, MAX_SIZE_STR, "%04Y%02m%02d%H%M%S", tmp_time);
	printf("current time >>> %s\n",_time);

	return LOG_OK;
}


int LOG_Info(char *_logInfoBuf , TYPE_FILE _type_file)
{
	int ret;

	printf("%s",_logInfoBuf);

	if (_type_file == TYPE_MD5)
	{
		fseek(Md5Fd, 0, SEEK_END);
		ret = fwrite(_logInfoBuf,strlen(_logInfoBuf),1,Md5Fd);
		if (ret < 0)
		{
			printf("write md5 log faile!\n");
			return LOG_FAILE;
		}
	}else if (_type_file == TYPE_LOG)
	{
		fseek(LogFd, 0, SEEK_END);
		ret = fwrite(_logInfoBuf,strlen(_logInfoBuf),1,LogFd);
		if (ret < 0)
		{
			printf("write log faile!\n");
			return LOG_FAILE;
		}		
	} 


	return LOG_OK;
}

int LOG_ClosePackageLog(TYPE_FILE _type_file)
{
	int ret;

	if (_type_file == TYPE_MD5)
	{
		ret = fclose(Md5Fd);
		if (ret < 0)
		{
			printf("md5 file close faile!\r\n");
			return LOG_FAILE;
		}
		printf("packet.md5 file closed\n");
	}else if (_type_file == TYPE_LOG)
	{
		ret = fclose(LogFd);
		if (ret < 0)
		{
			printf("log close faile!\r\n");
			return LOG_FAILE;
		}	
		printf("packet.log file closed\n");	
	}


	return LOG_OK;	
}


int LOG_CreatPackageLog(char *_logPathName, TYPE_FILE _type_file)
{
	assert(_logPathName != NULL);
	
	size_t ret;

	if (_type_file == TYPE_MD5)
	{
		Md5Fd = fopen(_logPathName, "a");
		if (Md5Fd < 0)
		{
			printf("log creat faile\n");
			return LOG_FAILE;
		}

		fseek(Md5Fd,0,SEEK_SET);
	}else if (_type_file == TYPE_LOG)
	{
		LogFd = fopen(_logPathName, "a");
		if (LogFd < 0)
		{
			printf("log creat faile\n");
			return LOG_FAILE;
		}

		ret = fwrite("\n",sizeof("\n"),1,LogFd);
		if (ret < 0)
		{
			printf("write to log faile!\n");
			return LOG_FAILE;
		} 

		fseek(LogFd,0,SEEK_SET);
	}

	return LOG_OK;
}



int LOG_CreatLogFile(char *_logDirPath, TYPE_FILE _type_file)
{
	int ret;
	char logPathName[MAX_STR_LEN];
	char packageName[MAX_STR_LEN];
	char curTime[MAX_SIZE_STR];

	memset(logPathName,'\0',sizeof(logPathName));
	memset(packageName,'\0',sizeof(packageName));
	memset(curTime,'\0',sizeof(curTime));

	strncpy(logPathName,_logDirPath,strlen(_logDirPath));

	if (_type_file == TYPE_MD5)
	{
		sprintf(packageName,"%c%s",'/',FILE_NAME_MD5_ENCRYPTION);
	}else if (_type_file == TYPE_LOG)
	{
		LOG_GetCurTime(curTime);
		sprintf(packageName,"%s%s%s","/package_",curTime,".log");
	}

	strcat(logPathName,packageName);
	printf("log file name >>> %s\n", logPathName);

	ret = LOG_CreatPackageLog(logPathName, _type_file);
	if (LOG_OK != ret)
	{
		printf("creat log failed!\n");
		return LOG_FAILE;
	}

	return LOG_OK;
}

