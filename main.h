#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdio.h>
#include <string.h>

#define READ_DATA_SIZE	1024
#define MD5_SIZE		16
#define MD5_STR_LEN		(MD5_SIZE * 2)

#define MAX_STR_LEN     1024

#define LOG_PATH             "/tmp"

/*file*/
#define FILE_CREAT_SUCCESS  1
#define FILE_CREAT_FAILE   -1
#define FILE_OPEN_FAILE    -2
#define FILE_WRITE_FAILE   -3
#define FILE_CLOSE_FAILE   -4
#define FILE_READ_FAILE    -5
#define MD5_COMPUT_FAILE   -10

typedef struct User_Info
{
	char *tool_function;
	char *imag_package_name;
	char *imag_package_path;
	char *log_path;
	char *gpg_user;
}USER_INFO;


extern USER_INFO m_user_data;

















#endif
