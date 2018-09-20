#ifndef __PACKAGE_H_
#define __PACKAGE_H_




#define FILE_NAME_GPG                 "package.gpg"
#define FILE_NAME_MD5_ENCRYPTION      "package_encrypt.md5"
#define FILE_NAME_MD5_DECRYPTION      "package_decrypt.md5"

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

PAK_FILE_RET PAK_ProtectSignature(char* _fileName, char* _filePath);
PAK_FILE_RET PAK_ProtectVerify(char* _fileName, char* _filePath);
PAK_FILE_RET PAK_CompareFile(char* __fileName1,char* __fileName2);






































#endif