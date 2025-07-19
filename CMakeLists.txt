cmake_minimum_required(VERSION 3.10)  
project(simple_stl_containers LANGUAGES CXX)  
set(CMAKE_CXX_STANDARD 17)  
set(CMAKE_CXX_STANDARD_REQUIRED ON)  # 强制使用c++11标准

# ----------------------------- 模块管理 -----------------------------    
include_directories(${PROJECT_SOURCE_DIR}/include)  
  
add_subdirectory(test)  # test/ 目录下的 CMakeLists.txt 会被自动调用


# ----------------------------- 编译选项（可选优化） -----------------------------  
if(CMAKE_BUILD_TYPE MATCHES "Debug")  
    add_compile_options(-Wall -Wextra -g)  # 调试模式开启警告和调试信息  
else()  
    add_compile_options(-O2)  # 发布模式优化  
endif()  