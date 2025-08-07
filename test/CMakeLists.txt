cmake_minimum_required(VERSION 3.10)  

# 定义测试目标
set( ref_catch test_main.cpp catch_amalgamated.cpp)  
# 定义函数：创建测试目标并添加公共依赖
function(add_test_target target_name src_file)
    add_executable(${target_name}
        ${ref_catch}
        ${src_file}
    )
    target_include_directories(${target_name} PRIVATE
        ${PROJECT_SOURCE_DIR}/include # 项目头文件路径（如 include/simple_stl/containers/vector.h） 
        ${CMAKE_CURRENT_SOURCE_DIR} # Catch2 头文件目录（test/Catch2/include/）
    )
    set_target_properties(${target_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin"  #可执行文件目录
    )
endfunction()

# 调用函数创建测试目标，只需传入目标名和源文件
add_test_target(test_vector_allfunc src/test_vector_allfunc.cpp)
add_test_target(test_list_allfunc src/test_list_allfunc.cpp)

add_test_target(test_shared_ptr_thread_safety src/test_shared_ptr_thread_safety.cpp)
target_link_libraries(test_shared_ptr_thread_safety PRIVATE pthread)

add_test_target(test_timer src/test_timer.cpp)
set(TOOLS_SRC 
    ${PROJECT_SOURCE_DIR}/src/tools/timer.cpp 
    ${PROJECT_SOURCE_DIR}/src/tools/timertask.cpp
)
target_sources(test_timer PRIVATE ${TOOLS_SRC}) # 添加源文件依赖 如timer.cpp timertask.cpp

add_test_target(test_threadpool_blockingqueue_o2m src/test_threadpool_blockingqueue_o2m.cpp)
target_link_libraries(test_threadpool_blockingqueue_o2m PRIVATE pthread)
target_sources(test_threadpool_blockingqueue_o2m PRIVATE ${PROJECT_SOURCE_DIR}/src/tools/threadpool.cpp)

add_test_target(test_threadpool_blockingqueue_m2m src/test_threadpool_blockingqueue_m2m.cpp)
target_link_libraries(test_threadpool_blockingqueue_m2m PRIVATE pthread)
target_sources(test_threadpool_blockingqueue_m2m PRIVATE ${PROJECT_SOURCE_DIR}/src/tools/threadpool.cpp)

add_test_target(test_string_allfunc src/test_string_allfunc.cpp)
target_sources(test_string_allfunc PRIVATE ${PROJECT_SOURCE_DIR}/src/containers/string.cpp)

add_test_target(test_singletonpattern_thread_safety src/test_singletonpattern_thread_safety.cpp)
target_link_libraries(test_singletonpattern_thread_safety PRIVATE pthread)
target_sources(test_singletonpattern_thread_safety PRIVATE ${PROJECT_SOURCE_DIR}/src/tools/singletonpattern.cpp)

add_test_target(test_lrucache src/test_lrucache.cpp)
target_sources(test_lrucache PRIVATE ${PROJECT_SOURCE_DIR}/src/tools/lrucache.cpp)