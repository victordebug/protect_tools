#ifndef __PACKAGE_H_
#define __PACKAGE_H_




#define FILE_NAME_GPG                 "package.gpg"
#define FILE_LOCAL_VERSION_MESAGE     "local_version_file"
#define FILE_LOCAL_GPG                "local_gpg_file"
#define FILE_LOCAL_CRC_FILE           "local_crc_file"

typedef enum package_file_ret
{
	PAK_OK   =   0,
	PAK_FAILE,
	
}PAK_FILE_RET;

typedef enum  file_type_ret
{
	PAK_TYPE_ZIP = 0,
	PAK_TYPE_TAR,
	PAK_TYPE_TAR_GZ,
	PAK_TYPE_TAR_BZ2,

	PAK_TYPE_MAX,
}PAK_FILE_TYPE_RET;


typedef enum file_tar_type_ret
{
	PAK_TAR_TYPE_COMPRESS = 0,
	PAK_TAR_TYPE_DECOMPRESSION,
}PAK_TAR_TYPE;

typedef enum file_gpg_type_ret
{
	PAK_GPG_TYPE_ENCRYPTION = 0,
	PAK_GPG_TYPE_DECRYPTION,
}PAK_GPG_TYPE;

typedef enum  file_write_mode
{
	PAK_WRITE_MODE_STA = 0,
	PAK_WRITE_MODE_END,
	PAK_WRITE_MODE_CUR,

	PAK_WRITE_MODE_MAX,
}PAK_WRITE_MODE;


PAK_FILE_RET PAK_ProtectSignature(char* _fileName);
PAK_FILE_RET PAK_ProtectVerify(char* _fileName);
PAK_FILE_RET PAK_Deal_File(char *_fileName);
PAK_FILE_RET PAK_Crc32_Cmp(char *_fileName);
PAK_FILE_RET PAK_FindFileInDir(char *_filePath, char *_fileName, char *_logBuf);
PAK_FILE_TYPE_RET PAK_JudgeFileType(char* _fileName);
PAK_FILE_RET PAK_Tar_File(char *tar_fileName, char *_filePath, PAK_TAR_TYPE _tar_type, PAK_FILE_TYPE_RET _file_type_ret);
PAK_FILE_RET PAK_Gpg_File(char* _fileName, PAK_GPG_TYPE file_gpg_type);
PAK_FILE_RET PAK_Md5_File(char* _fileName, char* _filePath);
PAK_FILE_RET PAK_DeletOtherFile(char *_filePath, char *_fileName);
PAK_FILE_RET PAK_CompareFile(char* _fileName1,char* _fileName2);
PAK_FILE_RET PAK_WriteToLog(char *_fileName, char *_buf, PAK_WRITE_MODE _mode);
char *PAK_GetBufOfFile(char *_file_name);
PAK_FILE_RET PAK_Get_Package_Version_Config(char *_config_file_name);





































#endif