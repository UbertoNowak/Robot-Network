# Robot Network project

## Build

To build project run:

    * cmake -B build

    * cmake --build build

## Run application

Go to build folder.
First run Core Node application which is a TCP server on andress 0.0.0.0 port 8888.
    * ./CoreNode

To run child node in network, you need to execute ChildNode application with 2 paramters :

* first -> node name

* second -> unit of the sensor e.g. mV, mA etc.

For example:

    * ./ChildNode node-1 mV


You can run many chold nodes (sensors) in separete consoles.
