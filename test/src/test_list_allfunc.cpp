#define CATCH_CONFIG_MAIN  // 若单独编译测试文件，需定义此宏以生成 main 函数  
#include "catch_amalgamated.hpp" 
#include "containers/list.h"
#include <vector>

TEST_CASE("默认构造函数测试", "[List][constructor]") {
    simple_stl::List<int> list;  
    REQUIRE(list.begin() == list.end()); // 空链表
}

// 从其他容器迭代器区间构造
TEST_CASE("区间构造函数测试", "[List][constructor]") {
    std::vector<int> vec = {1, 2, 3, 4};
    simple_stl::List<int> list(vec.begin(), vec.end());  

    auto it = list.begin();
    REQUIRE(*it++ == 1);
    REQUIRE(*it++ == 2);
    REQUIRE(*it++ == 3);
    REQUIRE(*it++ == 4);
    REQUIRE(it == list.end());  
}


TEST_CASE("计数构造函数测试", "[List][constructor]") {
    int n = 5;
    int val = 10;
    simple_stl::List<int> list(n, val);  

    int count = 0;
    for (auto it = list.begin(); it != list.end(); ++it) {
        REQUIRE(*it == val);
        count++;
    }
    REQUIRE(count == n); 
}


TEST_CASE("拷贝构造函数测试", "[List][constructor]") {
    simple_stl::List<int> original;
    original.push_back(10);
    original.push_back(20);
    original.push_back(30);

    simple_stl::List<int> copy(original);  

    // 验证拷贝后元素一致
    auto orig_it = original.begin();
    auto copy_it = copy.begin();
    while (orig_it != original.end()) {
        REQUIRE(*copy_it == *orig_it);  
        ++orig_it;
        ++copy_it;
    }
    REQUIRE(copy_it == copy.end());  
}