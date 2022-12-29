#include <iostream>
#include <string>
#include <random>
#include <chrono>
#include <thread>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

class Sensor
{
public:
    explicit Sensor(std::string &arName, std::string &arUnits)
        : _sensorName(arName),
          _unit(arUnits)
    {
        std::cout << "Child node with sensor '" << _sensorName << "' which mesures values in [ " << _unit << " ] created." << std::endl;
    }

    ~Sensor() = default;
    Sensor(const Sensor &) = delete;
    Sensor(Sensor &&) = delete;
    Sensor &operator=(const Sensor &) = delete;
    Sensor &operator=(Sensor &&) = delete;

    unsigned int readMeasuredValue() const
    {
        unsigned int measured_value = rand() % 1000;
        std::cout << "  Measured value: " << measured_value << " [" << _unit << "]" << std::endl;
        return measured_value;
    }

private:
    std::string _sensorName;
    std::string _unit;
};

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cout << "Error: Too few arguments. You need pass child node name and sensor unit." << std::endl;
        std::cout << "Example: ./ChildNode MySensorName mV" << std::endl;
        return 0;
    }

    std::string node_name(argv[1]);
    std::string sensor_unit(argv[2]);

    Sensor node_sensor(node_name, sensor_unit);

    constexpr int PORT_NUMBER = 8888;

    struct sockaddr_in addr, cl_addr;
    int sockfd, ret;
    char buffer[2000];
    struct hostent *server;
    char *serverAddr = "0.0.0.0";

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cout << "Error creating socket! " << std::endl;
        return 1;
    }
    std::cout << "Socket created..." << std::endl;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(serverAddr);
    addr.sin_port = PORT_NUMBER;

    ret = connect(sockfd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0)
    {
        std::cout << "Error connecting to the server! " << std::endl;
        return 2;
    }
    std::cout << "Connected to server..." << std::endl;
    memset(buffer, 0, 2000);

    while (true)
    {
        auto value = std::to_string(node_sensor.readMeasuredValue());
        ret = sendto(sockfd, &value, sizeof(value), 0, (struct sockaddr *) &addr, sizeof(addr));  
        if (ret < 0) {  
            std::cout << "Error during sending data to the server! " << std::endl;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return 0;
}