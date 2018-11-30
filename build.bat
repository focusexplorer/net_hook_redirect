if not exist build  or not exist build64 (
    mkdir build
    mkdir build64
)
set s="1"
if %s% == "0" (
cd build
cmake ../src -A Win32
) else (
cd build64
cmake ../src -A x64 
)
cmake --build . 
cd ..