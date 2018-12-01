project(net_hook_redirect)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    # 64 bits
    add_definitions(/DARCH64)
    set(ARCH64 TRUE)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    # 32 bits
    add_definitions(/DARCH32)
    set(ARCH32 TRUE)
endif()
add_library(hook_dll SHARED hook_dll.cpp C_log/log.cpp)
target_include_directories(hook_dll PRIVATE C_log C_log/include)
add_executable(hook_redirect hook_redirect.cpp RemThreadInjector.cpp )
add_executable(test_target test_target.cpp)
set_target_properties(test_target PROPERTIES COMPILE_FLAGS "/Fa")

if(ARCH32)
    #the following have no effect
   # install(TARGETS test_target hook.dll DESTINATION ../test_run)
endif(ARCH32)
if(ARCH64)
    #the following have no effect
   # install(TARGETS hook_redirect DESTINATION ../test_run)
endif(ARCH64)

