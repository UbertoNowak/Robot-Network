#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <algorithm>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
// #include <sys/time.h>

#define CLADDR_LEN 100

int main()
{
    std::cout << "Core node created" << std::endl;
    struct sockaddr_in addr, cl_addr;
    socklen_t len;
    int return_val, new_socket_fd;
    char buffer[2000];
    pid_t child_pid;
    char clientAddr[CLADDR_LEN];

    constexpr int PORT_NUMBER = 8888;

    // create a new socket
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        std::cerr << "Error: Can't create a socket" << std::endl;
        return 1;
    }
    else
    {
        std::cout << "Core socket created." << std::endl;
    }

    // std::fill(&addr, &addr + sizeof(addr), 0);
    //  memset(&addr, 0, sizeof(addr));
    bzero((char *)&addr, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = PORT_NUMBER;

    return_val = bind(socket_fd, (struct sockaddr *)&addr, sizeof(addr));
    if (return_val < 0)
    {
        std::cerr << "Error: Can't bind a socket" << std::endl;
        return 2;
    }

    std::cout << "Waiting for sensors connections" << std::endl;
    listen(socket_fd, 5);

    while (true)
    {
        // bzero(buffer, 2000);
        // std::fill(buffer, buffer + 2000, 0);
        len = sizeof(cl_addr);
        new_socket_fd = accept(socket_fd, (struct sockaddr *)&cl_addr, &len);
        if (new_socket_fd < 0)
        {
            std::cerr << "Error: Can't accept connection from sensor" << std::endl;
            return 3;
        }
        else
        {
            std::cout << "Connection accepted" << std::endl;
        }

        // bzero(buffer, 2000);

        // return_val = recvfrom(new_socket_fd, buffer, 2000, 0, (struct sockaddr *)&cl_addr, &len);
        // if (return_val < 0)
        // {
        //     std::cerr << "Error: During reading from sensor" << std::endl;
        //     return 4;
        // }

        inet_ntop(AF_INET, &(cl_addr.sin_addr), clientAddr, CLADDR_LEN);
        pid_t childpid = fork();
        if (childpid == 0)
        { // creating a child process

            close(socket_fd);
            // stop listening for new connections by the main process.
            // the child will continue to listen.
            // the main process now handles the connected client.

            for (;;)
            {
                memset(buffer, 0, 2000);
                auto ret = recvfrom(new_socket_fd, buffer, 2000, 0, (struct sockaddr *)&cl_addr, &len);
                if (ret < 0)
                {
                    std::cerr << "Error: During reading from sensor" << std::endl;
                    return 4;
                }
                std::cout << "Recv data: " << buffer << std::endl;

                //printf("Received data from %s: %s\n", clientAddr, buffer);
            }

            // std::string recv_msg(buffer);

            //std::cout << "  Message received: " << buffer << std::endl;
        }
        close(new_socket_fd);
    }
        return 0;
    }