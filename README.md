# Assignment 6
## Info
Name: Gabriel Olivotto    
Email: golivott@uwo.ca  
Student #: 251150929

## Submission
### A6-Water.cpp
This the main file its basically unchanged from the provided sample code I just added my own globe camera controller from assignment 5.

### PlaneMesh.hpp
Added variables for holding the IDs for the buffers, textures, and uniforms. Used the provided planeMeshQuads funciton to populated my buffers.

### Cam Controls.hpp
This contains my logic for generating the V matrix based on the camera's desired position. I flip the controls and up vector when you move over the poles so the camera doesn't flip. Click and drag with left mouse to move. Arrow keys to move forward and back. 

### LoadBMP.hpp
The came from the Resources/Misc Files/Lecture 15 page on OWL. No modifications were required

### shader.hpp
This came from the Resources/Misc Files/Lecture 15 page on OWL. I modified it to also take the TCS and TES shader files and used it to load my shaders.

### WaterShader.vertexshader
Calculated world vertex positons as well as normal, eye, light directions for the phong-like lighting. Texture UV are based on time as described in the instructions.  

### WaterShader.tcs
Outer and inner tess are set to 16. Used gl_InvocationID to assign values for position, normal, light dir, eye dir, and uv.

### WaterShader.tes
Didn't have to make any changes from the provided sample code.

### WaterShader.geoshader
Applied Gerstner wave formula as describe in assignment instructions to the world coordinates and applied change in y coordinate based on uv coordinate on the displacement map and recalculated normals. Then convert to cameraspace coordinates.

### WaterShader.fragmentshader
Almost unchanged from sample just added the calculations for cosTheta and cosAlpha.

### Screenshot1.png
Water shader running using the waves from assignment instructions.

# Windows Build Enviroment Setup
1. Install MSYS2 (https://www.msys2.org/)
2. Open MSYS2

    a. Install pacman: `pacman -Syu`
    
    b. Install MinGW: `pacman -S mingw-w64-x86_64-toolchain`

    c. Install Freegult: `pacman -S mingw-w64-x86_64-freeglut`

    d. Install Glew: `pacman -S mingw-w64-x86_64-glew`

    e. Install GLFW `pacman -S mingw-w64-x86_64-glfw`

    f. Install glm `pacman -S mingw-w64-x86_64-glm`

3. Add `C:/msys64/mingw64/bin` to user PATH variable 
4. Add `C:/msys64/mingw64/include/**` to your includePath in editor

### VS Code Setup
I use VS Code as my editor to get that working properly I had to install the C/C++ extension and update my config files as follows:
#### .vscode/task.json
```json
{
    "tasks": [
        {
            "type": "cppbuild",
            "label": "C/C++: g++.exe build active file",
            "command": "C:\\msys64\\mingw64\\bin\\g++.exe",
            "args": [
                "-fdiagnostics-color=always",
                "-g",
                "${file}",
                "-o",
                "${fileDirname}\\${fileBasenameNoExtension}.exe",
                "-lfreeglut",
                "-lglew32",
                "-lopengl32",
                "-lglfw3",
            ],
            "options": {
                "cwd": "${fileDirname}"
            },
            "problemMatcher": [
                "$gcc"
            ],
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "detail": "Task generated by Debugger."
        }
    ],
    "version": "2.0.0"
}
```

#### .vscode/c_cpp_properties.json
```json
{
    "configurations": [
        {
            "name": "Win32",
            "includePath": [
                "${workspaceFolder}/**",
                "C:/msys64/mingw64/include/**"
            ],
            "defines": [
                "_DEBUG",
                "UNICODE",
                "_UNICODE"
            ],
            "windowsSdkVersion": "10.0.22621.0",
            "compilerPath": "C:/msys64/mingw64/bin/g++.exe",
            "cStandard": "c17",
            "cppStandard": "c++17",
            "intelliSenseMode": "windows-gcc-x86"
        }
    ],
    "version": 4
}
```

#### Building
Open the file to be built, then from the top bar select `Terminal > Run Build Task`