#include <stdio.h>
#include <string.h>
#include <libgen.h>

#include "main.h"
#include "md5.h"
#include "log.h"
#include "package.h"
#include "crc32.h"

USER_INFO m_user_data = {NULL};

int readUserInfo(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    int ret;
    LOG_FILE_RET ret_log;
    PAK_FILE_RET ret_pak;


    printf("crc32:%lx\n", get_file_crc32("/home/henry/test/txt"));

/*
    printf("read user input information >>>\n");
    ret = readUserInfo(argc,argv);
    if (ret != 0)
    {
        return -1;
    }

    printf("creat log file in >>>", );
    ret_log = LOG_CreatLogFile(m_user_data.log_path,TYPE_MSG);
    if (ret_log != LOG_OK)
    {
        printf("creat log file faile!\n");
    }




    if (0 == strcmp(m_user_data.tool_function, "enc"))
    {
        LOG_Info("Encrypt the packet >>> \n", TYPE_MAX);
        ret_pak = PAK_ProtectSignature(m_user_data.imag_package_name,m_user_data.imag_package_path);
        if (ret_pak != PAK_OK)
        {
            LOG_Info("Encrypt packet failed!\n", TYPE_MAX);
            goto end;
        }
        LOG_Info("\n***************Encrypt packet succeed***************\n",TYPE_MAX);
    }else if (0 == strcmp(m_user_data.tool_function, "dec"))
    {
        ret_pak = PAK_ProtectVerify(m_user_data.imag_package_name,m_user_data.imag_package_path);
        if (ret_pak != PAK_OK)
        {
            LOG_Info("Decryption packet filed!\n",TYPE_MAX);
            goto end;
        }
        LOG_Info("\n***************Decrypt packet succeed***************\n",TYPE_MAX);
    }else 
    {
        LOG_Info("tools function error !\n", TYPE_MAX);
    }



    LOG_ClosePackageLog(TYPE_MSG);
	return 0;

end:
    LOG_ClosePackageLog(TYPE_MSG);
    return -1;
*/
}


static void userHelpInfo()
{
    printf("\n");
    printf("***************************************************************\n");
    printf("usage: protect_tool [option] [target] ...                      \n");

    printf("note:                                                          \n");
    printf("     Tools for secure packet transmission                      \n");

    printf("option:                                                        \n");
    printf("       -f          enc:encryption  dec:decryption              \n");
    printf("       -pn         image package name                          \n");
    printf("       -mn         image package message name                  \n");
    printf("       -prg        user gpg private keys                       \n");
    printf("       -pug        user gpg public keys                        \n");
    printf("       -h          help information                            \n");

    printf("example:                                                       \n");
    printf("        ./protect_tool -f enc -n test.tar.gz -g henry          \n");
    printf("***************************************************************\n");
}

int readUserInfo(int argc, char *argv[])
{
    int i =0 ;
    char userInfoBuf[1024];
    char *m_str = NULL;

    memset(userInfoBuf,'\0',sizeof(userInfoBuf));
    strcpy(userInfoBuf,argv[0]);
    i++;

    if (argc < 2)
    {
        userHelpInfo();
        return -1;
    }

    do
    {
        strcat(userInfoBuf," ");
        strcat(userInfoBuf,argv[i]);
        
        if (strcmp(argv[i], "-pn") == 0)
        {
            if (argv[i + 1] != NULL)
            {
                m_user_data.imag_package_name = argv[i + 1];
            }else
            {
                goto end;
            }
        }else if (strcmp(argv[i], "-mn") == 0)
        {
            if (argv[i + 1] != NULL)
            {
                m_user_data.message_imag_package_name = argv[i + 1];
            }else
            {
                goto end;
            }
        }else if (strcmp(argv[i], "prg") == 0)
        {
            if (argv[i + 1] != NULL)
            {
                m_user_data.gpg_user_private_key = argv[i + 1];
            }else
            {
                goto end;
            }
        }else if (strcmp(argv[i], "-f") == 0)
        {
            if (argv[i + 1] != NULL)
            {
                m_user_data.tool_function = argv[i + 1];
            }else
            {
                goto end;
            }
        }else if (strcmp(argv[i], "-pug") == 0)
        {
            if (argv[i + 1] != NULL)
            {
                m_user_data.gpg_user_public_key = argv[i + 1];
            }else
            {
                goto end;
            }
        }else if (strcmp(argv[i], "-h") == 0)
        {
            goto end;
        }

        i++;
    }while(i<argc);


    if ((m_user_data.tool_function == NULL) || (m_user_data.gpg_user_private_key == NULL) || (m_user_data.gpg_user_public_key == NULL) || 
        (m_user_data.imag_package_name == NULL) || (m_user_data.message_imag_package_name == NULL))
    { 
        goto end;
    }

    //get imag package path
    m_str = strdup(m_user_data.imag_package_name);
    m_user_data.log_path = dirname(m_str);


    printf("\n---------------------------------------------------------------\n");
    printf("userInfoBuf:%s\n", userInfoBuf);

    printf("tool_function:%s\n",m_user_data.tool_function);
    printf("imag_package_name:%s\n", m_user_data.imag_package_name);
    printf("message_imag_package_name:%s\n", m_user_data.message_imag_package_name);
    printf("log_path:%s\n",m_user_data.log_path);
    printf("gpg_user_public_key:%s\n",m_user_data.gpg_user_public_key);
    printf("gpg_user_private_key:%s\n",m_user_data.gpg_user_private_key);
    printf("---------------------------------------------------------------\n");


    return 0;

end:
    printf("\n---------------------------------------------------------------\n");
    printf("userInfoBuf:%s\n", userInfoBuf);
    printf("---------------------------------------------------------------\n");

    userHelpInfo();
    return -1;
}




