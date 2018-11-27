project(net_hook_redirect)
add_library(hook_dll SHARED hook_dll.cpp)
add_executable(hook_redirect hook_redirect.cpp)
add_executable(test_target test_target.cpp)