#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <algorithm>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char **argv)
{
    std::cout << "Core node created" << std::endl;
    constexpr int BUFFER_SIZE = 2000;
    char buffer[BUFFER_SIZE];
    constexpr int CLIENT_ADDR_LEN = 100;
    char clientAddr[CLIENT_ADDR_LEN];

    // create a new socket
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        std::cerr << "Error: Can't create a socket." << std::endl;
        return 1;
    }
    else
    {
        std::cout << "Core socket created." << std::endl;
    }

    struct sockaddr_in server_addres;
    constexpr int PORT_NUMBER = 8888;
    memset(&server_addres, 0, sizeof(server_addres));

    server_addres.sin_family = AF_INET;
    server_addres.sin_addr.s_addr = INADDR_ANY;
    server_addres.sin_port = PORT_NUMBER;

    int return_val = bind(socket_fd, (struct sockaddr *)&server_addres, sizeof(server_addres));
    if (return_val < 0)
    {
        std::cerr << "Error: Can't bind a socket" << std::endl;
        return 2;
    }

    std::cout << "Waiting for sensors connections" << std::endl;
    listen(socket_fd, 5);

    while (true)
    {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int new_socket_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &client_len);
        if (new_socket_fd < 0)
        {
            std::cerr << "Error: Can't accept connection from sensor" << std::endl;
            return 3;
        }
        else
        {
            std::cout << "Connection accepted" << std::endl;
        }

        inet_ntop(AF_INET, &(client_addr.sin_addr), clientAddr, CLIENT_ADDR_LEN);
        pid_t sensor_pid = fork(); // creating process for sensor
        if (sensor_pid == 0)
        {
            close(socket_fd);
            // stop listening for new connections by the main process.
            // the child will continue to listen.
            // the main process now handles the connected client.

            while (true)
            {
                memset(buffer, 0, BUFFER_SIZE);
                auto ret = recvfrom(new_socket_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &client_len);
                if (ret < 0)
                {
                    std::cerr << "Error: During reading from sensor" << std::endl;
                    return 4;
                }
                std::cout << "Recv data from: " << buffer << std::endl;
            }
        }
        close(new_socket_fd);
    }
    close(socket_fd);
    return 0;
}