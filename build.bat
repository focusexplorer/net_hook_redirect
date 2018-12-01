if not exist build  or not exist build64 (
    mkdir build
    mkdir build64
)
::set s="0"
::if %s% == "0" (
::cd build
::cmake ../src -A Win32
::) else (
::cd build64
::cmake ../src -A x64 
::)
cd build
cmake ../src -A Win32
cd ..
cmake --build build  

cd build64
cmake ../src -A x64 
cd ..
cmake --build build64 
::cmake --build build64 --target hook_redirect


::64exe must use 32dll to hook 32exe
::cp build/Debug/hook_dll* build64/Debug