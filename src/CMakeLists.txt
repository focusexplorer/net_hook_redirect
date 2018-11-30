project(net_hook_redirect)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    # 64 bits
    add_definitions(/DARCH64)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    # 32 bits
    add_definitions(/DARCH32)
endif()
add_library(hook_dll SHARED hook_dll.cpp)
add_executable(hook_redirect hook_redirect.cpp)
add_executable(test_target test_target.cpp)
set_target_properties(test_target PROPERTIES COMPILE_FLAGS "/Fa")