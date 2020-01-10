# fea-shaders
Playground for GLSL shaders for FEA  

## Build Prerequisites  
C++17 compiler, e.g. Visual Studio 2019 or GCC 8  
CMake  
Qt 5

## Build & Run
To build the app in Windows or Linux, run `./build.sh` in Bash (on Windows, use Git Bash).  
`bash.sh` takes two optional parameter:  
* Project name: default `ogl`
* Build type: `Debug` or `Release` (default `Debug`)  
  
The resulting executable will be in _build.Linux_ or _build.Windows_.  

IMPORTANT: On Windows, it's better to pass your Qt location to CMake by editing _build.sh_ to contain the right pass as the value of `Qt5_DIR` in line 41.  