#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <sys/types.h>  /*提供类型pid_t,size_t的定义*/
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "package.h"
#include "log.h"
#include "main.h"
#include "md5.h"
#include "crc32.h"


unsigned int len_gpg = 0;
static char *local_crc32 = 0;

PAK_FILE_RET PAK_ProtectSignature(char* _fileName)
{
	char logBuf[MAX_STR_LEN];
	char *bufFile = NULL;
	int bufLen = 0;
    long int crc_log = 0;
    PAK_FILE_RET ret_pak;

	//assert
	assert(NULL != _fileName);

	memset(logBuf,'\0',sizeof(logBuf));

	//juge the file is exist
	if (access(_fileName,F_OK) != 0)
	{
		printf("i can not find %s \r\n", _fileName);
		return -1;
	}

	//gpg file to txt
	ret_pak = PAK_Gpg_File(_fileName, PAK_GPG_TYPE_ENCRYPTION);
	if (ret_pak != PAK_OK)
	{
		goto Recycle;
	}

	//get len gpg.file
	bufFile = PAK_GetBufOfFile(FILE_NAME_GPG);
	printf("gpg >>> %s\n", bufFile);

	//write len of gpg.file
	bufLen = strlen(bufFile);
	printf("gpg buf len >>> %d\n", bufLen);
	
	sprintf(logBuf,"%d",bufLen);
    PAK_WriteToLog(m_user_data.log_name,logBuf,PAK_WRITE_MODE_END);
	PAK_WriteToLog(m_user_data.log_name,"#$#",PAK_WRITE_MODE_END);

	
	//write gpg.file to log 
	PAK_WriteToLog(m_user_data.log_name,bufFile,PAK_WRITE_MODE_END);
	PAK_WriteToLog(m_user_data.log_name,"#$#",PAK_WRITE_MODE_END);
	
	//write crc to gpg
	crc_log = get_file_crc32(m_user_data.log_name);
	printf("crc of log file >>> %lx\n", crc_log);

	sprintf(logBuf,"%lx",crc_log);
	PAK_WriteToLog(m_user_data.log_name,logBuf,PAK_WRITE_MODE_END);


	//delete other file
	if (access(_fileName,F_OK) == 0)     remove(_fileName);
	if (access(FILE_NAME_GPG,F_OK) == 0) remove(FILE_NAME_GPG);
	//return 
	return PAK_OK;

Recycle:
	if (access(FILE_NAME_GPG,F_OK) == 0) remove(FILE_NAME_GPG);

	return PAK_FAILE;
}



PAK_FILE_RET PAK_ProtectVerify(char* _fileName)
{
	char logBuf[MAX_STR_LEN];
	char *bufFile = NULL;
    PAK_FILE_RET ret_pak;


	assert(NULL != _fileName);
	memset(logBuf,'\0',sizeof(logBuf));

	//juge the file is exist
	if (access(_fileName,F_OK) != 0)
	{
		printf("error: I can not find %s \r\n", _fileName);
		return PAK_FAILE;
	}

	//deal with the message
	ret_pak = PAK_Deal_File(_fileName);
	if (ret_pak != PAK_OK)
	{
		goto Recycle;
	}

	//compare len of gpg
	bufFile = PAK_GetBufOfFile(FILE_LOCAL_GPG);
	if (len_gpg != strlen(bufFile))
	{
		printf("error: gpg len compare is defferent! \n");
		goto Recycle;
	}

	//crc32 deal with file
	ret_pak = PAK_Crc32_Cmp(_fileName);
	if (ret_pak != PAK_OK)
	{
		goto Recycle;
	}

	//gpg verify 
	ret_pak = PAK_Gpg_File(_fileName, PAK_GPG_TYPE_DECRYPTION);
	if (ret_pak != PAK_OK)
	{
		goto Recycle;
	}

	//delet other file
	if (access(FILE_LOCAL_VERSION_MESAGE,F_OK) == 0)     remove(FILE_LOCAL_VERSION_MESAGE);
	if (access(FILE_LOCAL_GPG,F_OK) == 0)                remove(FILE_LOCAL_GPG);
	if (access(FILE_LOCAL_CRC_FILE,F_OK) == 0)           remove(FILE_LOCAL_CRC_FILE);
	if (access(_fileName,F_OK) == 0)                     remove(_fileName);

	return PAK_OK;

Recycle:
	if (access(FILE_LOCAL_VERSION_MESAGE,F_OK) == 0)     remove(FILE_LOCAL_VERSION_MESAGE);
	if (access(FILE_LOCAL_GPG,F_OK) == 0)                remove(FILE_LOCAL_GPG);
	if (access(FILE_LOCAL_CRC_FILE,F_OK) == 0)           remove(FILE_LOCAL_CRC_FILE);

	return PAK_FAILE;
}


PAK_FILE_RET PAK_WriteToLog(char *_fileName, char *_buf, PAK_WRITE_MODE _mode)
{
	FILE *fp;

	fp = fopen(_fileName,"a+");
	if (fp  == NULL)
	{
		printf("fopen %s filed!\n", _fileName);
		return PAK_FAILE;
	}

	if (_mode == PAK_WRITE_MODE_STA)
	{
		fseek(fp,0,SEEK_SET);
	}else if (_mode == PAK_WRITE_MODE_END)
	{
		fseek(fp,0,SEEK_END);
	}else if (_mode == PAK_WRITE_MODE_CUR)
	{
		fseek(fp,0,SEEK_CUR);
	}
	fseek(fp,0,SEEK_END);
	fwrite(_buf,strlen(_buf),1,fp);
	fclose(fp);

	return PAK_OK;
}

char *PAK_GetBufOfFile(char *_file_name)
{
	FILE *fp;
	int flen = 0;
	int ret = 0;
	char *readBuf = NULL;

	if (access(_file_name,F_OK) != 0)
	{
		printf("i can not find %s \r\n", _file_name);
		return NULL;
	}

	fp = fopen(_file_name,"r");
	if (fp < 0)
	{
		printf("open %s file\n",_file_name);
		return NULL;
	}

	fseek(fp,0,SEEK_END);
	flen = ftell(fp);

	readBuf = (char *)malloc(flen + 1);
	if (readBuf == NULL)
	{
		printf("malloc faile \n");
		fclose(fp);
		return NULL;
	}

	fseek(fp,0,SEEK_SET);
	ret = fread(readBuf,flen,1,fp);
	if (ret < 0)
	{
		printf("read %s filed\n", _file_name);
	}

	fclose(fp);
	
	readBuf[flen] = '\0';

	return readBuf;
}

PAK_FILE_RET PAK_Get_Package_Version_Config(char *_config_file_name)
{
	char *fileBuf = NULL;
	char *ptr = NULL;

	//add #$#
	PAK_WriteToLog(m_user_data.log_name,"#$#",PAK_WRITE_MODE_STA);

	//get buf of config file
	fileBuf = PAK_GetBufOfFile(_config_file_name);
	printf("file_buf:%s\r\n",fileBuf);

	//splid buf and write in message file
	ptr = strtok(fileBuf,"\n");
	while(ptr != NULL)
	{
		printf("ptr = %s\n", ptr);
		PAK_WriteToLog(m_user_data.log_name,ptr,PAK_WRITE_MODE_END);
		PAK_WriteToLog(m_user_data.log_name,";",PAK_WRITE_MODE_END);
		ptr = strtok(NULL,"\n");
	}

	//add #$#
	PAK_WriteToLog(m_user_data.log_name,"#$#",PAK_WRITE_MODE_END);

	return PAK_OK;
}



PAK_FILE_RET PAK_Deal_File(char *_fileName)
{
	char *fileBuf = NULL;
    char *ptr = NULL;
    int index = 0;

    //get buf of config file
    fileBuf = PAK_GetBufOfFile(_fileName);
    //printf("file_buf:%s\r\n",fileBuf);

    //splid buf and write in message file
    ptr = strtok(fileBuf,"#$#");
    while(ptr != NULL)
    {
    	index ++;
        if (index == 1)//version message
        {
            printf("version message >>> %s\n",ptr);
            PAK_WriteToLog(FILE_LOCAL_VERSION_MESAGE,ptr,PAK_WRITE_MODE_STA);
        }else if (index == 2)//len of gpg
        {
            len_gpg = atoi(ptr);
            printf("len of gpg >>> %d\n",len_gpg);
        }else if (index == 3)//gpg
        {
            PAK_WriteToLog(FILE_LOCAL_GPG,ptr,PAK_WRITE_MODE_STA);
            printf("gpg >>> %s\n",ptr);
        }else if (index == 4)//crc
        {
            local_crc32 = ptr;
            printf("crc >>> %s\n",local_crc32);
        }

        ptr = strtok(NULL,"#$#");
    }

    return PAK_OK;
}


PAK_FILE_RET PAK_Crc32_Cmp(char *_fileName)
{
   FILE *fp,*ffp;
   int i = 0;
   int flen = 0;
   char c[2];
   char crc[9];

   memset(crc,'\0',sizeof(crc));

   /* 打开用于读取的文件 */
   fp = fopen(_fileName , "r");
   if(fp == NULL)
   {
      perror("打开文件时发生错误");
      return PAK_FAILE;
   }


   fseek(fp,0,SEEK_END);
   flen = ftell(fp);
   printf("flen>>>%d\r\n",flen);

   ffp = fopen(FILE_LOCAL_CRC_FILE,"w+");
   fseek(fp,0,SEEK_SET);
   while((fgets(c,sizeof(c),fp) != NULL) && (i < flen-8))
   {
        i++;
        fputs(c,ffp);

        //printf("%s",c);
   }
   fclose(ffp);
   fclose(fp);
   
   sprintf(crc,"%lx",get_file_crc32(FILE_LOCAL_CRC_FILE));
   printf("crc >>> %s\n", crc);

   if (strcmp(crc,local_crc32) == 0)
   {
   		printf("crc value verify succeed!\n");
   		return PAK_OK;
   }

   printf("crc value verify filed!\n");
   return PAK_FAILE;
}


PAK_FILE_RET PAK_FindFileInDir(char *_filePath, char *_fileName, char *_logBuf)
{
	DIR *pDir;
    struct dirent *stPstr;

    pDir = opendir(_filePath);
    if (pDir == NULL)
    {
    	LOG_Info("imagPath not exist!\n",TYPE_LOG);

    	return PAK_FAILE;
    }

    while(NULL != (stPstr = readdir(pDir)))
    {
    	if (NULL != strstr(stPstr->d_name, _fileName))
    	{
    		strncpy(_logBuf,stPstr->d_name,strlen(stPstr->d_name));
			closedir(pDir);

			sprintf(_logBuf, "%s exist in %s\n",_fileName,_filePath);
		    LOG_Info(_logBuf,TYPE_LOG);
    		return PAK_OK;
    	}
    }

    closedir(pDir);
    sprintf(_logBuf, "%s not exist in %s\n",_fileName,_filePath);
	LOG_Info(_logBuf,TYPE_LOG);	

    return PAK_FAILE;
}


/*	PAK_TYPE_ZIP = 0,
	PAK_TYPE_TAR,
	PAK_TYPE_TAR_GZ,
	PAK_TYPE_TAR_BZ2,*/
PAK_FILE_TYPE_RET PAK_JudgeFileType(char* _fileName)
{
	assert(NULL != _fileName);

	PAK_FILE_TYPE_RET file_type_ret;

	 if (strstr(_fileName,"zip") != NULL)
	 {
	 	file_type_ret = PAK_TYPE_ZIP;
	 }else if (strstr(_fileName,"tar") != NULL)
	 {
	 	if (strstr(_fileName,"tar.bz2") != NULL)
	 	{
	 		file_type_ret = PAK_TYPE_TAR_BZ2;
	 	}else if (strstr(_fileName,"tar.gz") != NULL)
	 	{
	 		file_type_ret = PAK_TYPE_TAR_GZ;
	 	}else
	 	{
	 		file_type_ret = PAK_TYPE_TAR;
	 	}
	 }else
	 {
	 	file_type_ret = PAK_TYPE_MAX;
	 }

	 return file_type_ret;
}


PAK_FILE_RET PAK_Tar_File(char *tar_fileName, char *_filePath, PAK_TAR_TYPE _tar_type, PAK_FILE_TYPE_RET _file_type_ret)
{
	assert((tar_fileName != NULL) && (_filePath != NULL));

	char tar_comm_buf[MAX_STR_LEN];
	char result_buf[MAX_STR_LEN];
	int ret;
    struct dirent *stPstr;
    FILE *fp;
 	DIR *pDir;

	memset(tar_comm_buf,'\0',sizeof(tar_comm_buf));
	memset(result_buf,'\0',sizeof(result_buf));
	strcpy(tar_comm_buf,"tar ");

	if (_tar_type == PAK_TAR_TYPE_COMPRESS)
	{
		if (_file_type_ret == PAK_TYPE_TAR_GZ) //tar.gz
		{
			strcat(tar_comm_buf,"-zcvf ");
			strcat(tar_comm_buf, _filePath);
			strcat(tar_comm_buf, "/");
			strcat(tar_comm_buf,tar_fileName);
			strcat(tar_comm_buf," ");

			pDir = opendir(_filePath);
		    if (pDir == NULL)
		    {
		    	LOG_Info("imagPath not exist!\n",TYPE_LOG);

		    	return PAK_FAILE;
		    }

			while(NULL != (stPstr = readdir(pDir)))
			{
				if ((0 == strcmp(stPstr->d_name, ".")) || (0 == strcmp(stPstr->d_name, "..")))
				{
				
				}else 
				{
					strcat(tar_comm_buf,stPstr->d_name);
					strcat(tar_comm_buf," ");
				}
			}

			closedir(pDir);
		}

	}else if (_tar_type == PAK_TAR_TYPE_DECOMPRESSION)
	{
		if (_file_type_ret == PAK_TYPE_TAR_GZ)  //tar.gz
		{
			strcat(tar_comm_buf, "-zxvf ");
			strcat(tar_comm_buf, _filePath);
			strcat(tar_comm_buf, "/");
			strcat(tar_comm_buf, tar_fileName);
		}
	}

	printf("tar common buffer >>> %s\n", tar_comm_buf);

	chdir(_filePath);
	LOG_Info("tar >>> ...\n",TYPE_LOG);
	fp = popen(tar_comm_buf, "r");
	if (NULL == fp)
	{
		LOG_Info("tar commom execute filed >>> popen file\n",TYPE_LOG);
	}

	while(fgets(result_buf,sizeof(result_buf),fp) != NULL)
	{
		LOG_Info(result_buf,TYPE_LOG);
	}

	ret = pclose(fp);
	if (ret < 0)
	{
		LOG_Info("close fp file\n",TYPE_LOG);
		return PAK_FAILE;
	}

	return PAK_OK;
}


PAK_FILE_RET PAK_Gpg_File(char* _fileName, PAK_GPG_TYPE file_gpg_type)
{
	char gpg_comm_buf[MAX_STR_LEN];
	char result_buf[100];
	char curTime[MAX_STR_LEN];
	char packageName[MAX_STR_LEN];
	FILE *fp;
	int flag = 0;

	memset(result_buf,'\0',sizeof(result_buf));
	//memset(curTime,'\0',sizeof(curTime));
	//memset(packageName,'\0',sizeof(packageName));

	if (file_gpg_type == PAK_GPG_TYPE_ENCRYPTION)
	{
		/*sprintf(gpg_comm_buf, "%s %s %s %s %s %s %s %s" ,"gpg -u ",m_user_data.gpg_user_send," -r ",m_user_data.gpg_user_rev,
			" -a -s -o ",FILE_NAME_GPG," -e ",_fileName);*/

		sprintf(gpg_comm_buf, "%s %s %s %s %s %s" ,"gpg -a -r ",m_user_data.gpg_keys," -o ",FILE_NAME_GPG," -e ",_fileName);
	}else if (file_gpg_type == PAK_GPG_TYPE_DECRYPTION)
	{
		LOG_GetCurTime(curTime);
		sprintf(packageName,"%s%s%s%s",m_user_data.log_path,"/ImgPackageMsg_",curTime,".tar.gz");
		sprintf(gpg_comm_buf, "%s %s %s %s","gpg -o",packageName,"-d",FILE_LOCAL_GPG);
	}

	printf("gpg common buffer >>> %s\n", gpg_comm_buf);

	printf("gpg >>> ...\n");
	fp = popen(gpg_comm_buf,"r");
	if (NULL == fp)
	{
		printf("gpg commom execute filed >>> popen file\n");
	}

	while(fgets(result_buf,sizeof(result_buf),fp) != NULL)
	{
		printf("%s",result_buf);
		if (strstr(result_buf, "failed")) flag = 1;
	}

	pclose(fp);

    if (flag == 1) return PAK_FAILE;
	return PAK_OK;
}


PAK_FILE_RET PAK_Md5_File(char* _fileName, char* _filePath)
{
	LOG_FILE_RET ret_log;
	char md5_str[MD5_STR_LEN + 1];
	char m_FileName[MAX_STR_LEN];
	int ret;

	memset(md5_str,'\0',sizeof(md5_str));
	memset(m_FileName,'\0',sizeof(m_FileName));

	//creat md5.txt
	ret_log = LOG_CreatLogFile(_filePath,TYPE_MD5);
	if (ret_log != LOG_OK)
	{
		printf("creat log file faile!\n");
		return PAK_FAILE;
	}	

	//md5sum calculate
	sprintf(m_FileName,"%s%c%s",_filePath,'/',_fileName);
	printf("md5 file name >>> %s\n", m_FileName);

    ret = Compute_file_md5(m_FileName,md5_str);
    if (ret == 0)
    {
    	LOG_Info(md5_str,TYPE_MD5);
    	LOG_Info("\n",TYPE_LOG);
    }
	LOG_ClosePackageLog(TYPE_MD5);	

	return PAK_OK;
}

PAK_FILE_RET PAK_DeletOtherFile(char *_filePath, char *_fileName)
{
	DIR *pDir;
    struct dirent *stPstr;
    char nameBuf[MAX_STR_LEN];

    pDir = opendir(_filePath);
    if (pDir == NULL)
    {
    	LOG_Info("imagPath not exist!\n",TYPE_LOG);

    	return PAK_FAILE;
    }

    while(NULL != (stPstr = readdir(pDir)))
    {
    	if ((NULL != strstr(stPstr->d_name, _fileName)) || (0 == strcmp(stPstr->d_name, ".")) || (0 == strcmp(stPstr->d_name, "..")))
    	{
    		
    	}else
    	{
    		memset(nameBuf,'\0',sizeof(nameBuf));
    		sprintf(nameBuf, "%s%c%s",_filePath,'/',(stPstr->d_name));
    		printf("remove %s >>> ...\n", nameBuf);
    		remove(nameBuf);
    	}
    }

    closedir(pDir);
    return PAK_OK;	
}

PAK_FILE_RET PAK_CompareFile(char* _fileName1,char* _fileName2)
{
    int ret;
    int fd1,fd2;
    int data_len;
    char file_buf1[MAX_STR_LEN];
    char file_buf2[MAX_STR_LEN];

    memset(file_buf1,'\0',sizeof(file_buf1));
    memset(file_buf2,'\0',sizeof(file_buf2));

    fd1 = open(_fileName1,O_RDONLY);
    if (fd1 < 0)
    {
        return FILE_OPEN_FAILE;
    }
    
    data_len = read(fd1,file_buf1,MD5_STR_LEN+1);
    if (data_len < 0)
    {
        return FILE_READ_FAILE;
    }
    
    close(fd1);

    printf("buf1:%s\r\n",file_buf1);

    
    fd2 = open(_fileName2,O_RDONLY);
    if (fd2 < 0)
    {
        return FILE_OPEN_FAILE;
    }
    
    data_len = read(fd2,file_buf2,MD5_STR_LEN+1);
    if (data_len < 0)
    {
        return FILE_READ_FAILE;
    }
    
    close(fd2);

    printf("buf2:%s\r\n",file_buf2);

    ret = strcmp(file_buf1,file_buf2);
    if (ret != 0)
    {
        return PAK_FAILE;
    }

    return PAK_OK;
}