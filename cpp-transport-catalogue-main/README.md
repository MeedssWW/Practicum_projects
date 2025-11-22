# Transport Catalog

##Overview
Transport Catalog is a C++ application designed to store and process public transport data.
It reads information about stops and bus routes, creates an internal database, and provides the ability to respond to various statistical queries.

The project also supports:
- rendering transport maps using SVG,
- routing between stops based on travel time,
- serialization and deserialization of the transport catalog using protocol buffers.

This project demonstrates a sophisticated architecture, efficient data structures, JSON processing, graph algorithms, and modern design principles in C++.

## Dataset Description
The input data is in JSON format and includes:
- **Stops**: names, geographic coordinates
- **Routes (buses)**: lists of stops
- **Distances** between stops
- **Routing Settings**: bus speeds, waiting times
- **Rendering Settings**: colors, sizes, and offsets for map rendering

This dataset forms a complete transportation network, which the application analyzes to:
- calculate route lengths
- create map visuals
- construct a routing graph for optimal pathfinding
- serialize internal state

## Project Goals
- Create an efficient in-memory database of stops and routes
- Parse and process JSON input data to populate the directory
- Implement serialization and deserialization using ProtoBuf
- Render the transportation map in SVG format
- Routing graph construction and optimal route calculation.
- Demonstration of modular architecture and strict separation of concerns.

## Technologies and Libraries
- **C++17/C++20**
- **STL**
- **CMake**
- **Protocol buffers** for serialization
- **Custom JSON parser**
- **SVG rendering module**
- **Graph and routing module** for shortest path calculation
- **Git/GitHub**

##Build Instructions
```bash
mkdir build
cd build
cmake..
make
