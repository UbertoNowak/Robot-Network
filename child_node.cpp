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

    std::string getUnit() const{
        return _unit;
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

    constexpr int PORT_NUMBER = 8888;
    constexpr unsigned BUFFER_SIZE = 2000;
    char buffer[BUFFER_SIZE];
    struct hostent *server;

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        std::cout << "Error creating socket! " << std::endl;
        return 1;
    }
    std::cout << "Socket created..." << std::endl;

    const std::string CORE_NODE_ADRESS("0.0.0.0");
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(CORE_NODE_ADRESS.c_str());
    server_addr.sin_port = PORT_NUMBER;

    int rv = connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (rv < 0)
    {
        std::cout << "Error connecting to the server! " << std::endl;
        return 2;
    }
    std::cout << "Connected to server..." << std::endl;
    memset(buffer, 0, BUFFER_SIZE);

    std::string node_name(argv[1]);
    std::string sensor_unit(argv[2]);
    Sensor node_sensor(node_name, sensor_unit);

    while (true)
    {
        auto value = std::to_string(node_sensor.readMeasuredValue());
        value += " [ " + node_sensor.getUnit() + " ]";
        rv = sendto(socket_fd, &value, sizeof(value), 0, (struct sockaddr *) &server_addr, sizeof(server_addr));  
        if (rv < 0) {  
            std::cout << "Error during sending data to the server! " << std::endl;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return 0;
}