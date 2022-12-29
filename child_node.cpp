#include <iostream>
#include <string>
#include <random>
#include <chrono>
#include <thread>

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
    Sensor(const Sensor&) = delete;
    Sensor(Sensor&&) = delete;
    Sensor& operator=(const Sensor&) = delete;
    Sensor& operator=(Sensor&&) = delete;

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
    while (true)
    {
        node_sensor.readMeasuredValue();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return 0;
}