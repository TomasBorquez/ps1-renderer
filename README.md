# About
This is a simple 3D renderer with PS1 like graphics, it has shader hot reloading, model loaders, extended `.glsl` for including code, and more.

**Example**:
![Screenshot_5](https://github.com/user-attachments/assets/94902c14-9e27-4987-bb7f-3b640424c1b8)

**Stack**:
- C99
- SDL3
- Glew 
- Assimp 
- cimgui

# How to run
- This project uses `mate.h` build system I created, for more info check it [here](https://github.com/TomasBorquez/mate.h)
- To run use your compiler of choice and run the build script with: `gcc ./mate.c -o ./mate.exe && ./mate.exe`, for now it only works on MinGW
