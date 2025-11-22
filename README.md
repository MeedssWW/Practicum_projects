# Practicum C++ Projects
A collection of C++ projects developed during the **Yandex Practicum "C++ Developer"** program.
Each project is independent, modular, and demonstrates practical skills in modern C++ development, including working with data structures, JSON processing, serialization, routing, and rendering.

---

## 📁 Repository Structure

### Projects Overview Table

| Project | Description | Key Technologies |
|--------|------------|------------------|
| **cpp-spreadsheet** | A simplified spreadsheet system with support for storing and processing cell data. | C++17/20, STL, CMake, ANTLR  |
| **cpp-transport-catalogue** | A transport catalog with JSON input, route calculation, SVG map rendering, and protobuf serialization. | C++17/20, STL, Protocol Buffers, JSON parser, SVG, routing algorithms |

---

## 📌 Project Descriptions

### 1. cpp-spreadsheet
A minimal spreadsheet application implemented in modern C++.
The project focuses on data modeling and internal spreadsheet architecture such as:

- representation of rows, columns, and cells
- storing different value types (numbers, strings, etc.)
- controlled access to cell data
- clean modular structure using CMake

#### Technologies Used
- C++17 / C++20
- STL (containers, algorithms, smart pointers)
- CMake
- ANTLR 

---

### 2. cpp-transport-catalogue
A full-featured transport catalog capable of reading transport data from JSON and answering statistical queries.
It includes:

- building an internal database of stops, routes, and distances
- parsing JSON requests for input and output
- computing optimal routes using a graph-based router
- rendering the transport network as an SVG map
- serializing and deserializing the catalog using **Protocol Buffers**

#### Input Dataset
The catalog reads:
- stop coordinates
- bus routes
- distances between stops
- rendering parameters
- routing settings (bus speed, wait time)

#### Technologies Used
- C++17 / C++20
- STL
- CMake
- Google Protocol Buffers
- Custom JSON parser
- Graph algorithms and routing module
- SVG rendering module

---

## 🛠Build Instructions

Each project is built independently using CMake.

```bash
cd <project-folder>
mkdir build
cd build
cmake..
make
