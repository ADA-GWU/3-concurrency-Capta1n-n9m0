# Advance Software Paradigms Assignment 2
Student: Abbas Aliyev

## Project Overview
This project demonstrates advanced image processing techniques with a focus on concurrency. 
It utilizes SDL (Simple DirectMedia Layer) for rendering images and handling user interface events, 
OpenCV for robust image processing capabilities, and OpenMP for efficient parallel processing.

The choice of these libraries is strategic:

- SDL is used for its simplicity and efficiency in managing windows and rendering, making it ideal for real-time image display.
- OpenCV is a powerful library for image manipulation, offering a wide range of functions to process and analyze images.
- OpenMP is preferred over standard STL threads for its ease of use and efficient management of parallel processes. 
OpenMP simplifies the task of dividing the work among multiple threads and synchronizing them, making it more suitable for complex tasks like image processing.

## Prerequisites
To compile and run this application, you need to have the following installed:

- CMake (version 3.21 or higher)
- A C++ compiler supporting C++20 standard
- OpenMP
- OpenCV library
- SDL2 library

## Compilation Instructions

1. Clone the repository to your local machine.
2. Navigate to the project directory. 
3. Create a new directory for the build:
```bash
mkdir build
cd build
```

4. Run CMake to configure the project and generate native build files:
```bash
cmake ..
```

5. Compile the project using CMake's build command:
```bash
cmake --build .
```
This command will build the project using the generated build system.

## Running the Application
To run the application, execute the generated binary file from the terminal. 
You can pass command-line arguments as needed for image processing parameters.

Example:
```bash
./ada_a3_image_conv <input_image> <kernel_size> <execution_mode> [color]
```

- input_image: Path to the image file.
- kernel_size: Size of the kernel for image processing.
- execution_mode: 'S' for single-threaded or 'M' for multi-threaded execution.
- color: Background color is 6 character string in hexadecimal. It is optional and will be set to FAFAFA by default.

## High-Level Description
This project is an exploration of concurrent image processing. It demonstrates how different technologies 
like SDL, OpenCV, and OpenMP can be integrated to create an efficient and responsive image processing application. 
The application is capable of handling real-time image manipulation tasks, showcasing the power of parallel computing in processing large datasets like images.

The use of OpenMP is particularly noteworthy. Unlike STL threads, which can be cumbersome to manage and synchronize, 
OpenMP offers a more straightforward approach to parallelism. It allows for easy scaling of the application 
across multiple processors, making the application more efficient and faster in processing complex image manipulation tasks.

## Disclaimer

The author of this project, Abbas Aliyev, provides the code and accompanying documentation on an "as-is" basis and makes no representations regarding the functionality or suitability of this project for any purpose. The author shall not be held responsible for any damage, data loss, or adverse effects resulting from the compilation or execution of this project.

Users are advised to ensure that all prerequisites are properly installed and configured before attempting to compile or run this project. Failure to do so may result in unexpected behavior, system crashes, or other issues for which the author does not assume any responsibility.

This project is intended for educational and demonstration purposes only. Users should exercise caution and understand the risks involved when compiling and running software, especially in environments with critical data or operations.

By compiling or running this project, users acknowledge and accept that they do so at their own risk and discretion, and agree to bear all responsibility for any consequences that may arise.