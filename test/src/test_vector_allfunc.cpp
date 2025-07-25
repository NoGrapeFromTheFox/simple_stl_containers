
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

// 测试拷贝构造函数
TEST_CASE("Copy Constructor", "[vector]") {
    // 1. 测试空 vector 拷贝
    SECTION("Copy empty vector") {
        simple_stl::vector<int> src;
        simple_stl::vector<int> dest(src); // 调用拷贝构造

        REQUIRE(dest.size() == 0);
        REQUIRE(dest.capacity() == 0);
    }

    // 2. 测试非空 vector 拷贝（深拷贝验证）
    SECTION("Copy non-empty vector (deep copy)") {
        simple_stl::vector<int> src;
        src.push_back(10);
        src.push_back(20);
        src.push_back(30);
        size_t src_cap = src.capacity();

        simple_stl::vector<int> dest(src); // 拷贝构造

        // 验证内容相同
        REQUIRE(dest.size() == 3);
        REQUIRE(dest[0] == 10);
        REQUIRE(dest[1] == 20);
        REQUIRE(dest[2] == 30);

        // 验证深拷贝（修改 src 不影响 dest）
        src[0] = 100;
        REQUIRE(dest[0] == 10); // dest 未被修改

        // 验证容量独立（不共享内存）
        REQUIRE(dest.capacity() == src_cap); // 拷贝构造通常保留原容量
    }
}

// 测试拷贝赋值运算符
TEST_CASE("Copy Assignment Operator", "[vector]") {
    // 1. 测试自我赋值（无异常）
    SECTION("Self assignment") {
        simple_stl::vector<int> vec;
        vec.push_back(5);
        vec = vec; // 自我赋值
        REQUIRE(vec.size() == 1);
        REQUIRE(vec[0] == 5);
    }

    // 2. 测试非空 vector 赋值（深拷贝验证）
    SECTION("Assign non-empty vector (deep copy)") {
        simple_stl::vector<int> src, dest;
        src.push_back(1);
        src.push_back(2);
        dest.push_back(100); // dest 原有旧元素

        dest = src; // 拷贝赋值

        // 验证内容相同
        REQUIRE(dest.size() == 2);
        REQUIRE(dest[0] == 1);
        REQUIRE(dest[1] == 2);

        // 验证深拷贝（修改 src 不影响 dest）
        src[0] = 10;
        REQUIRE(dest[0] == 1);

        // 验证旧元素已被销毁（若元素是类类型，可通过析构计数验证）
    }

    // 3. 测试赋值空 vector
    SECTION("Assign empty vector") {
        simple_stl::vector<int> src, dest;
        dest.push_back(10);
        dest = src; // 赋值空 vector
        REQUIRE(dest.size() == 0);
    }
}

// 测试移动构造函数
TEST_CASE("Move Constructor", "[vector]") {
    // 1. 移动非空 vector
    SECTION("Move non-empty vector") {
        simple_stl::vector<int> src;
        src.push_back(10);
        src.push_back(20);
        size_t src_cap = src.capacity();

        simple_stl::vector<int> dest(std::move(src)); // 移动构造

        // 验证 dest 接管了 src 的内容
        REQUIRE(dest.size() == 2);
        REQUIRE(dest[0] == 10);
        REQUIRE(dest[1] == 20);
        REQUIRE(dest.capacity() == src_cap);

        // 验证 src 处于有效但未指定状态（通常为空）
        REQUIRE(src.size() == 0); // 移动后 src 应清空
        // 不验证 src 的 capacity（可能保留或清零，标准未规定）
    }

    // 2. 移动空 vector
    SECTION("Move empty vector") {
        simple_stl::vector<int> src;
        simple_stl::vector<int> dest(std::move(src));
        REQUIRE(dest.size() == 0);
        REQUIRE(src.size() == 0);
    }
}

// 测试移动赋值运算符
TEST_CASE("Move Assignment Operator", "[vector]") {
    // 1. 自我移动（需保证安全，通常无操作）
    SECTION("Self move") {
        simple_stl::vector<int> vec;
        vec.push_back(5);
        vec = std::move(vec); // 自我移动
        REQUIRE(vec.size() == 1); // 仍有效
        REQUIRE(vec[0] == 5);
    }

    // 2. 移动非空 vector 到已有元素的 vector
    SECTION("Move non-empty to existing vector") {
        simple_stl::vector<int> src, dest;
        src.push_back(30);
        src.push_back(40);
        size_t src_cap = src.capacity();
        dest.push_back(100); // dest 原有元素

        dest = std::move(src); // 移动赋值

        // 验证 dest 接管内容
        REQUIRE(dest.size() == 2);
        REQUIRE(dest[0] == 30);
        REQUIRE(dest[1] == 40);
        REQUIRE(dest.capacity() == src_cap);

        // 验证 src 为空
        REQUIRE(src.size() == 0);
    }

    // 3. 移动空 vector 到非空 vector
    SECTION("Move empty to non-empty vector") {
        simple_stl::vector<int> src, dest;
        dest.push_back(10);
        dest = std::move(src); // 移动空 vector
        REQUIRE(dest.size() == 0);
        REQUIRE(src.size() == 0);
    }
}