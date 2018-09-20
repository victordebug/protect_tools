#include <stdio.h>
#include <string.h>
#include "main.h"
#include "md5.h"
#include "log.h"
#include "package.h"

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

    printf("creat tool log file in >>> %s\n", m_user_data.log_path);
    ret_log = LOG_CreatLogFile(m_user_data.log_path,TYPE_LOG);
    if (ret_log != LOG_OK)
    {
        printf("creat log file faile!\n");
    }

    if (0 == strcmp(m_user_data.tool_function, "enc"))
    {
        LOG_Info("Encrypt the packet >>> \n", TYPE_LOG);
        ret_pak = PAK_ProtectSignature(m_user_data.imag_package_name,m_user_data.imag_package_path);
        if (ret_pak != PAK_OK)
        {
            LOG_Info("Encrypt packet failed!\n", TYPE_LOG);
            goto end;
        }
        LOG_Info("\n***************Encrypt packet succeed***************\n",TYPE_LOG);
    }else if (0 == strcmp(m_user_data.tool_function, "dec"))
    {
        ret_pak = PAK_ProtectVerify(m_user_data.imag_package_name,m_user_data.imag_package_path);
        if (ret_pak != PAK_OK)
        {
            LOG_Info("Decryption packet filed!\n",TYPE_LOG);
            goto end;
        }
        LOG_Info("\n***************Decrypt packet succeed***************\n",TYPE_LOG);
    }else 
    {
        LOG_Info("tools function error !\n", TYPE_LOG);
    }

    LOG_ClosePackageLog(TYPE_LOG);
	return 0;

end:
    LOG_ClosePackageLog(TYPE_LOG);
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
    printf("       -f         enc:encryption  dec:decryption               \n");
    printf("       -n         image package name                           \n");
    printf("       -p         image package path                           \n");
    printf("       -l         log path                                     \n");
    printf("       -g         gpg user                                     \n");
    printf("       -h         help information                             \n");

    printf("example:                                                       \n");
    printf("        ./protect_tool -f enc -n test.tar.gz -g henry          \n");
    printf("***************************************************************\n");
}

int readUserInfo(int argc, char *argv[])
{
    int i =0 ;
    char userInfoBuf[1024];

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
        
        if (strcmp(argv[i], "-n") == 0)
        {
            if (argv[i + 1] != NULL)
            {
                m_user_data.imag_package_name = argv[i + 1];
            }else
            {
                goto end;
            }
        }else if (strcmp(argv[i], "-p") == 0)
        {
            if (argv[i + 1] != NULL)
            {
                m_user_data.imag_package_path = argv[i + 1];
            }else
            {
                goto end;
            }
        }else if (strcmp(argv[i], "-l") == 0)
        {
            if (argv[i + 1] != NULL)
            {
                m_user_data.log_path = argv[i + 1];
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
        }else if (strcmp(argv[i], "-g") == 0)
        {
            if (argv[i + 1] != NULL)
            {
                m_user_data.gpg_user = argv[i + 1];
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


    if (m_user_data.tool_function != NULL)
    {
        if ((strcmp(m_user_data.tool_function, "enc") == 0) && (m_user_data.gpg_user == NULL))
        {
            goto end;
        }
    }else
    {
        goto end;
    }

    if (m_user_data.imag_package_path == NULL)
    {
        goto end;
    }

    if (m_user_data.imag_package_path == NULL)
    {
        goto end;
    }

    if (m_user_data.log_path == NULL)
    {
        m_user_data.log_path = LOG_PATH;
    }

    printf("\n---------------------------------------------------------------\n");
    printf("userInfoBuf:%s\n", userInfoBuf);

    printf("tool_function:%s\n",m_user_data.tool_function);
    printf("imag_package_name:%s\n", m_user_data.imag_package_name);
    printf("imag_package_path:%s\n", m_user_data.imag_package_path);
    printf("log_path:%s\n",m_user_data.log_path);
    printf("gpg_user:%s\n",m_user_data.gpg_user);
    printf("---------------------------------------------------------------\n");


    return 0;

end:
    printf("\n---------------------------------------------------------------\n");
    printf("userInfoBuf:%s\n", userInfoBuf);
    printf("---------------------------------------------------------------\n");

    userHelpInfo();
    return -1;
}




