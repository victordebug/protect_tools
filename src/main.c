#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <fcntl.h>

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


    printf("read user input information >>>\n");
    ret = readUserInfo(argc,argv);
    if (ret != 0)
    {
        return -1;
    }


    if (0 == strcmp(m_user_data.tool_function, "enc"))
    {
        printf("creat log file >>>\n");
        ret_log = LOG_CreatLogFile(m_user_data.log_path,TYPE_MSG);
        if (ret_log != LOG_OK)
        {
            printf("creat log file faile!\n");
            return -1;
        }

        printf("write package version massage to log >>>\n");
        ret_pak = PAK_Get_Package_Version_Config(m_user_data.message_imag_package_name);
        if (ret_pak != PAK_OK)
        {
            printf("get package version filed!\n");
            goto end;
        }

        LOG_Info("Encrypt the packet >>> \n", TYPE_MAX);
        ret_pak = PAK_ProtectSignature(m_user_data.imag_package_name);
        if (ret_pak != PAK_OK)
        {
            LOG_Info("Encrypt packet failed!\n", TYPE_MAX);
            goto end;
        }
        LOG_Info("\n***************Encrypt packet succeed***************\n",TYPE_MAX);
    }else if (0 == strcmp(m_user_data.tool_function, "dec"))
    {
        ret_pak = PAK_ProtectVerify(m_user_data.imag_package_name);
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

	return 0;

end:
    if (access(m_user_data.log_name,F_OK) == 0)     remove(m_user_data.log_name);
    return -1;

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
    printf("       -pn         image package path name                     \n");
    printf("       -mn         image package message path name             \n");
    printf("       -k          gpg keys                                    \n");
    printf("       -h          help information                            \n");

    printf("example:                                                       \n");
    printf("        ./protect_tool -f enc -pn test.tar.gz  ...             \n");
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
        }else if (strcmp(argv[i], "-k") == 0)
        {
            if (argv[i + 1] != NULL)
            {
                m_user_data.gpg_keys = argv[i + 1];
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
        }else if (strcmp(argv[i], "-h") == 0)
        {
            goto end;
        }

        i++;
    }while(i<argc);


    if (strcmp(m_user_data.tool_function , "enc") == 0)
    {
        if ((m_user_data.gpg_keys == NULL) || (m_user_data.imag_package_name == NULL) || (m_user_data.message_imag_package_name == NULL))
        { 
            goto end;
        }
    }else if (strcmp(m_user_data.tool_function , "dec") == 0)
    {
        if (m_user_data.imag_package_name == NULL)
        { 
            goto end;
        }
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
    printf("gpg_keys:%s\n",m_user_data.gpg_keys);
    printf("---------------------------------------------------------------\n");


    return 0;

end:
    printf("\n---------------------------------------------------------------\n");
    printf("userInfoBuf:%s\n", userInfoBuf);

    printf("tool_function:%s\n",m_user_data.tool_function);
    printf("imag_package_name:%s\n", m_user_data.imag_package_name);
    printf("message_imag_package_name:%s\n", m_user_data.message_imag_package_name);
    printf("log_path:%s\n",m_user_data.log_path);
    printf("gpg_keys:%s\n",m_user_data.gpg_keys);
    printf("---------------------------------------------------------------\n");

    userHelpInfo();
    return -1;
}




