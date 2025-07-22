
#define CATCH_CONFIG_MAIN  // 若单独编译测试文件，需定义此宏以生成 main 函数  
#include <memory>          // 若 vector 中使用 allocator、pointer 等类型  
#include "catch_amalgamated.hpp"  
#include <containers/vector.h>  
#include <common/utilities.h> 
#include <common/shared_ptr.h>
#include <iostream>

TEST_CASE("minimal segfault test with detailed logs") {
    
    std::cout << "\n===== 开始极简段错误测试 =====" << std::endl;

    std::cout << "[步骤1] 创建空vector对象" << std::endl;
    simple_stl::vector<int> vec;

    std::cout << "[步骤2] 初始状态: size=" << vec.size() 
              << ", capacity=" << vec.capacity() << std::endl;

    std::cout << "[步骤3] 调用emplace_back(10)" << std::endl;
    vec.emplace_back(10);

    std::cout << "[步骤4] 插入后状态: size=" << vec.size() 
              << ", capacity=" << vec.capacity() << std::endl;

    std::cout << "[步骤5] 尝试访问vec[0]" << std::endl;
    int val = vec[0];
    std::cout << "[步骤5] 访问结果: vec[0]=" << val << std::endl;

    std::cout << "[步骤6] 调用emplace_back(20)（可能触发扩容）" << std::endl;
    vec.emplace_back(20);

    std::cout << "[步骤7] 第二次插入后: size=" << vec.size() 
              << ", capacity=" << vec.capacity() << std::endl;

    std::cout << "[步骤8] 尝试访问vec[1]" << std::endl;
    val = vec[1];
    std::cout << "[步骤8] 访问结果: vec[1]=" << val << std::endl;
    std::cout << "\n===== 测试正常结束 =====" << std::endl;
}
TEST_CASE("vector with simple_stl::shared_ptr (complex type test)") {

    std::cout << "\n===== 开始 shared_ptr 复杂类型测试 =====" << std::endl;

    using SharedPtrInt = simple_stl::shared_ptr<int>;

    std::cout << "[步骤2] 创建 vector<SharedPtrInt> 对象" << std::endl;
    simple_stl::vector<SharedPtrInt> vec;

    std::cout << "[步骤3] 初始状态: size=" << vec.size() 
              << ", capacity=" << vec.capacity() << std::endl;

    std::cout << "[步骤4] 调用 emplace_back(SharedPtrInt(new int(10)))" << std::endl;
    vec.emplace_back(SharedPtrInt(new int(10)));
    std::cout << "[步骤4] 插入后: size=" << vec.size() 
              << ", 引用计数=" << vec[0].use_count() << std::endl; // 应输出1

    std::cout << "[步骤5] 访问 vec[0]: 指向的值=" << *vec[0] << std::endl;
    CHECK(*vec[0] == 10);

    std::cout << "[步骤6] 调用 reserve(5)" << std::endl;
    vec.reserve(5);
    std::cout << "[步骤6] reserve后: capacity=" << vec.capacity() 
              << ", 元素值=" << *vec[0] << std::endl; // 扩容后值应不变
    CHECK(vec.capacity() == 5);

    std::cout << "[步骤7] 调用 emplace_back(SharedPtrInt(new int(20)))" << std::endl;
    vec.emplace_back(SharedPtrInt(new int(20)));
    std::cout << "[步骤7] 插入后: size=" << vec.size() 
              << ", vec[1]值=" << *vec[1] << std::endl;
    CHECK(*vec[1] == 20);

    std::cout << "[步骤8] 调用 emplace_back(SharedPtrInt(new int(30)))" << std::endl;
    vec.emplace_back(SharedPtrInt(new int(30)));
    std::cout << "[步骤8] 插入后: size=" << vec.size() 
              << ", capacity=" << vec.capacity() << std::endl;
    CHECK(vec.size() == 3);

    
    std::cout << "[步骤9] 连续插入3个元素，触发扩容" << std::endl;
    vec.emplace_back(SharedPtrInt(new int(40)));
    vec.emplace_back(SharedPtrInt(new int(50)));
    vec.emplace_back(SharedPtrInt(new int(60))); 
    std::cout << "[步骤9] 扩容后: capacity=" << vec.capacity() 
              << ", vec[5]值=" << *vec[5] << std::endl;
    CHECK(vec.capacity() >= 10);
    CHECK(*vec[5] == 60);

    std::cout << "[步骤10] 调用 push_back(SharedPtrInt(new int(70)))" << std::endl;
    SharedPtrInt ptr(new int(70));
    vec.push_back(std::move(ptr)); // 移动插入
    std::cout << "[步骤10] push_back后: vec[6]值=" << *vec[6] << std::endl;
    CHECK(*vec[6] == 70);

    std::cout << "[步骤11] 调用 pop_back()" << std::endl;
    vec.pop_back(); // 删除最后一个元素（值为70的shared_ptr）
    std::cout << "[步骤11] pop_back后: size=" << vec.size() << std::endl; // 应从7变为6
    CHECK(vec.size() == 6);

    std::cout << "[步骤12] 访问 vec[5]: 值=" << *vec[5] << std::endl; // 应输出60
    CHECK(*vec[5] == 60);

    std::cout << "\n===== shared_ptr 测试结束 =====" << std::endl;
}