# Spreadsheet Application in C++

## Overview
This project is a simplified spreadsheet application implemented in modern C++.

The goal is to develop a table-like data structure that can store cell values ​​of various types and support basic spreadsheet functionality.

The project demonstrates the architecture of spreadsheet systems such as Excel or Google Sheets, including data storage, cell access, formula parsing, and table management.

## Dataset Description

"Data" in this project is the content entered by the user into spreadsheet cells.

Each cell can contain:
- Numeric values;
- Text;
- Formulas;

The focus is on building a robust spreadsheet data representation model.

## Project Goals
- Implement a table structure with rows, columns, and cells.
- Support various cell value types.
- Provide interfaces for reading and writing cell contents.
– Develop an internal architecture similar to that of a spreadsheet.
– Demonstrate modular project structure using CMake.
– Explore expression parsing.

## Technologies and Libraries
- **C++17/C++20**
- **STL**
- **CMake**
- **ANTLR**
- **Git/GitHub**

## Build Instructions
```bash
mkdir build
cd build
cmake ..
make
