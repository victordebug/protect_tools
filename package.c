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


PAK_FILE_RET PAK_Gpg_File(char* _fileName, char* _filePath, PAK_GPG_TYPE file_gpg_type)
{
	char gpg_comm_buf[MAX_STR_LEN];
	char result_buf[MAX_STR_LEN];
	int ret;
	FILE *fp;

	memset(gpg_comm_buf,'\0',sizeof(gpg_comm_buf));
	memset(result_buf,'\0',sizeof(result_buf));

	if (file_gpg_type == PAK_GPG_TYPE_ENCRYPTION)
	{
		sprintf(gpg_comm_buf, "%s %s%c%s %s %s %s%c%s" ,"gpg -o",_filePath,'/',_fileName,"-er",m_user_data.gpg_user,_filePath,'/',FILE_NAME_MD5_ENCRYPTION);
	}else if (file_gpg_type == PAK_GPG_TYPE_DECRYPTION)
	{
		sprintf(gpg_comm_buf, "%s %s%c%s %s %s%c%s","gpg -o",_filePath,'/',FILE_NAME_MD5_DECRYPTION,"-d",_filePath,'/',_fileName);
	}

	printf("gpg common buffer >>> %s\n", gpg_comm_buf);

	LOG_Info("gpg >>> ...\n",TYPE_LOG);
	fp = popen(gpg_comm_buf,"r");
	if (NULL == fp)
	{
		LOG_Info("gpg commom execute filed >>> popen file\n",TYPE_LOG);
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


PAK_FILE_RET PAK_ProtectSignature(char* _fileName, char* _filePath)
{
	char logBuf[MAX_STR_LEN];
	char m_FileNameMd5[MAX_STR_LEN];
    int ret;
    int fileLen = 0;
	LOG_FILE_RET ret_log;
    PAK_FILE_RET ret_pak;

	//assert
	assert((NULL != _fileName) && (NULL != _filePath));

	memset(logBuf,'\0',sizeof(logBuf));
	memset(m_FileNameMd5,'\0',sizeof(m_FileNameMd5));

	//juge the file is exist
	ret_pak = PAK_FindFileInDir(_filePath,_fileName,logBuf);
	if (ret_pak != PAK_OK)
	{
		return PAK_FAILE;
	}

	//md5sum calculate
	ret_pak = PAK_Md5_File(_fileName, _filePath);
	if (ret_pak != PAK_OK)
	{
		return PAK_FAILE;
	}

	ret_pak = PAK_Gpg_File(FILE_NAME_GPG, _filePath, PAK_GPG_TYPE_ENCRYPTION);
	if (ret_pak != PAK_OK)
	{
		goto Recycle;
	}

    //delete other file
	sprintf(m_FileNameMd5,"%s%c%s",_filePath,'/', FILE_NAME_MD5_ENCRYPTION);
	printf("md5 file full name >>> %s\n",m_FileNameMd5);
	remove(m_FileNameMd5);

	//tar new file
	ret_pak = PAK_Tar_File(_fileName, _filePath, PAK_TAR_TYPE_COMPRESS, PAK_TYPE_TAR_GZ);
	if (ret_pak != PAK_OK)
	{
		LOG_Info("tar new package failed!\n",TYPE_LOG);
		goto Recycle;
	}

	//delete other file
	PAK_DeletOtherFile(_filePath,_fileName);

	//return 
	return PAK_OK;

Recycle:
	PAK_DeletOtherFile(_filePath,_fileName);

	return PAK_FAILE;
}




PAK_FILE_RET PAK_ProtectVerify(char* _fileName, char* _filePath)
{
	char logBuf[MAX_STR_LEN];
	char m_fileNmae[MAX_STR_LEN];
	char m_md5FileName1[MAX_STR_LEN];
	char m_md5FileName2[MAX_STR_LEN];
    char md5_str[MD5_STR_LEN + 1];	
    int ret;
	LOG_FILE_RET ret_log;
    PAK_FILE_RET ret_pak;
	PAK_FILE_TYPE_RET file_type_ret;


	assert((NULL != _fileName) && (NULL != _filePath));

	memset(logBuf,'\0',sizeof(logBuf));
	memset(m_fileNmae,'\0',sizeof(m_fileNmae));
	memset(m_md5FileName1,'\0',sizeof(m_md5FileName1));
	memset(m_md5FileName2,'\0',sizeof(m_md5FileName2));

	//find fileName in filePath
	ret_pak = PAK_FindFileInDir(_filePath,_fileName,logBuf);
	if (ret_pak != PAK_OK)
	{
		return PAK_FAILE;
	}
	//if fileName is tar.gz, tar -xzvf
	sprintf(m_fileNmae,"%s%c%s",_filePath,'/',_fileName);
	printf("target file full name >>> %s\n", m_fileNmae);

	file_type_ret = PAK_JudgeFileType(m_fileNmae);
	if (PAK_TYPE_MAX != file_type_ret)
	{
		ret_pak = PAK_Tar_File(_fileName, _filePath, PAK_TAR_TYPE_DECOMPRESSION, file_type_ret);
		if (ret_pak != PAK_OK)
		{
			LOG_Info("tar new package failed!\n",TYPE_LOG);
			goto Recycle;
		}	
	}

	//find package.gpg
	memset(logBuf,'\0',sizeof(logBuf));
	ret_pak = PAK_FindFileInDir(_filePath,FILE_NAME_GPG,logBuf);
	if (ret_pak != PAK_OK)
	{
		goto Recycle;
	}
	//gpg package.gpg
	ret_pak = PAK_Gpg_File(FILE_NAME_GPG, _filePath, PAK_GPG_TYPE_DECRYPTION);
	if (ret_pak != PAK_OK)
	{
		goto Recycle;
	}
	//find  other file
	memset(logBuf,'\0',sizeof(logBuf));
	ret_pak = PAK_FindFileInDir(_filePath,_fileName,logBuf);
	if (ret_pak != PAK_OK)
	{
		goto Recycle;
	}
	//md5sum file
	ret_pak = PAK_Md5_File(_fileName, _filePath);
	if (ret_pak != PAK_OK)
	{
		goto Recycle;
	}
	//PAK_CompareFile
	sprintf(m_md5FileName1,"%s%c%s",_filePath,'/',FILE_NAME_MD5_ENCRYPTION);
	sprintf(m_md5FileName2,"%s%c%s",_filePath,'/',FILE_NAME_MD5_DECRYPTION);
	ret_pak = PAK_CompareFile(m_md5FileName1,m_md5FileName2);
	if (ret_pak != PAK_OK)
	{
		goto Recycle;
	}
	//delet other file
	PAK_DeletOtherFile(_filePath,_fileName);

	return PAK_OK;

Recycle:
	PAK_DeletOtherFile(_filePath,_fileName);
	
	return PAK_FAILE;
}
















