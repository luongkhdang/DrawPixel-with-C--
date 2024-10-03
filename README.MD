
# Project 5: DrawPixel in C++
# Overview
This project is a simple 3D wireframe renderer developed in C++ using the SDL2 library. It visualizes a rotating 3D cube by projecting its vertices onto a 2D screen and drawing the connecting edges. The application demonstrates fundamental concepts in graphics programming, including rendering loops, event handling, and basic 3D transformations. 

## Demo  
[demo](demo.JPG)






# Project Structure
- main.cpp: Contains the main application logic, including the rendering loop, event handling, and 3D transformations.
- screen.h: Defines the Screen class, which manages the SDL2 window, renderer, and drawing operations.

## Run Locally  

Clone the project  

~~~bash  
  git clone https://github.com/luongkhdang/Project-5--DrawPixel-with-CPP--.git
~~~

Go to the project directory  

~~~bash  
  cd my-project
~~~

 
Requirement:
  SDL2 // SDL2 is included (lib and include folder)

To Start:

~~~bash  
g++ -fdiagnostics-color=always -g "main.cpp" -o "main.exe" -I "/include" -L "/lib" -lmingw32 -lSDL2main -lSDL2 -mconsole 
~~~

Insert directory of main.cpp, main.exe, /include, and /lib.
For example:
~~~bash 
C:/msys64/ucrt64/bin/g++.exe -fdiagnostics-color=always -g "C:\PROGRAMMING\BIG PROJECT\DrawPixel with C++\main.cpp" -o "C:\PROGRAMMING\BIG PROJECT\DrawPixel with C++\main.exe" -I "C:/PROGRAMMING/BIG PROJECT/DrawPixel with C++/include" -L "C:/PROGRAMMING/BIG PROJECT/DrawPixel with C++/lib" -lmingw32 -lSDL2main -lSDL2 -mconsole 
~~~





## Features  

* 3D Rendering: Visualizes a rotating 3D cube by projecting 3D points onto a 2D screen.
* Interactive Window: Handles window events, allowing users to close the application gracefully.
* Basic Transformations: Implements rotation around the X, Y, and Z axes.
* Efficient Rendering: Uses SDL2 for rendering points and lines efficiently.




# Prerequisites


Before diving into this project, you should have a solid understanding of the following topics:

* C++ Programming:

  - Basic syntax and structure
  - Object-oriented programming concepts (classes, objects, constructors, destructors)
  - Standard Template Library (STL) usage (e.g., std::vector)
* SDL2 Library:

  - Installation and setup of SDL2 in your development environment
  - Basic SDL2 concepts (windows, renderers, events)
* Basic Mathematics:

  - Understanding of vectors and points in 3D space
  - Basic trigonometry (sine and cosine functions)
  - 3D rotation transformations


# What You'll Learn

Working on this project will help you develop and reinforce the following skills:

* Graphics Programming with SDL2:

  - Setting up an SDL2 window and renderer
  - Drawing pixels and lines to represent 3D objects on a 2D screen
  - Managing the rendering loop and frame updates
* 3D Transformations:

  - Implementing rotation transformations around different axes
  - Understanding the mathematics behind projecting 3D points onto a 2D plane
* Event Handling:

  - Managing user inputs and window events to ensure smooth application behavior
* Optimizing Rendering:

  - Efficiently managing and rendering multiple points and connections


## License  

This project is licensed under the [MIT](https://choosealicense.com/licenses/mit/) License. Feel free to use, modify, and distribute it as you see fit.