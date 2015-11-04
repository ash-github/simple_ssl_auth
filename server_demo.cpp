#include "hyc_ssl_contex.h"

int main(int argc, char* argv[])
{
    try {

        //! 创建SSL环境
        HycSSLContex contex(true);

        std::string ca_verify_file_path = "certificate/ca-cert.pem";
        std::string local_certificate_file_path = "certificate/server-cert.pem";
        std::string local_private_file_path = "certificate/server-cert.key";


        if(contex.SetContex(ca_verify_file_path,
                            local_certificate_file_path,
                            local_private_file_path) < 0 )
        {
            std::cerr << "set contex fail" << std::endl;
        }

        // 以下是正常的socket监听流程
        sockaddr_in service;
        service.sin_family = AF_INET;
        service.sin_addr.s_addr = INADDR_ANY;
        service.sin_port = htons( 8899 );

        SOCKET serverFd = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
        if ( bind( serverFd, (struct sockaddr *) &service, sizeof(service) ) < 0 )
        {
            close(serverFd);
            return -1;
        }

        int opt = 1;
        setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(opt));
        if ( listen(serverFd, 10 ) < 0 )
        {
            return -1;
        }
        SOCKET newClientFd;
        do
        {
            newClientFd = accept(serverFd, NULL, NULL);
            sleep(2);
        }while(newClientFd < 0);


        //! 生成ssl socket
        HycSSLSocket *sslSocket = contex.CreateSSLSocket(newClientFd);
        char buffer[1024] = {0};

        //! 开始ssl通信
        while(1)
        {
            // read
            if(sslSocket->Read(buffer, 1023) <= 0)
            {
                break;
            }
            std::cout << "receive:" << buffer << std::endl;
            // write
            if(sslSocket->Write("I AM SERVER", sizeof("I AM SERVER")) <= 0)
            {
                break;
            }

            sleep(1);
            memset(buffer, 0x0, 1024);
        }

        return 0;
    }
    catch (std::exception& e)
    {
        std::cerr << "[main]Exception: " << e.what() << "\n";
    }

}
