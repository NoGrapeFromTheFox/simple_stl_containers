
#define CATCH_CONFIG_MAIN  // 若单独编译测试文件，需定义此宏以生成 main 函数  
#include <memory>          // 若 vector 中使用 allocator、pointer 等类型  
#include "catch_amalgamated.hpp"  
#include <containers/vector.h>  
#include <common/utilities.h> 

// ----------------------------- 自定义测试类型（MoveOnly） -----------------------------  
class MoveOnly {  
public:  
      
    MoveOnly() = default;  

    //（计数 +1）  
    MoveOnly(MoveOnly&& other) noexcept {  
        ++move_count;  
    }  

    // 禁用拷贝构造（强制使用移动语义）  
    MoveOnly(const MoveOnly&) = delete;  

    //（计数 +1）  
    ~MoveOnly() {  
        ++destruct_count;  
    }  

    // 移动赋值运算符（可选，若测试涉及赋值操作）  
    MoveOnly& operator=(MoveOnly&& other) noexcept {  
        if (this != &other) {  
            ++move_count;  
        }  
        return *this;  
    }  

    // 禁用拷贝赋值  
    MoveOnly& operator=(const MoveOnly&) = delete;  

    // 静态成员：记录移动构造次数（包括移动构造和移动赋值）  
    static int move_count;  
    // 静态成员：记录析构次数  
    static int destruct_count;  
};  

// 初始化静态成员（必须在源文件中定义，否则链接错误）  
int MoveOnly::move_count = 0;  
int MoveOnly::destruct_count = 0;  


// ----------------------------- vector reserve 基础测试用例 -----------------------------  
TEST_CASE("vector reserve: 初始化状态", "[vector][reserve]") {  
    simple_stl::vector<int> vec;  
    REQUIRE(vec.capacity() == 0);   // 初始容量为 0  
    REQUIRE(vec.size() == 0);        // 元素数量为 0  
    REQUIRE(vec.start() == nullptr);  // 未分配内存，start_ 为 nullptr  
}  

TEST_CASE("vector reserve: 容量从 0 扩容到 n（无元素）", "[vector][reserve]") {  
    simple_stl::vector<int> vec;  
    vec.reserve(10);  
    REQUIRE(vec.capacity() == 10);   // 容量更新为目标值  
    REQUIRE(vec.size() == 0);        // 元素数量不变（仍为 0）  
    REQUIRE(vec.start() != nullptr);  // 分配新内存，start_ 非空  
}  

TEST_CASE("vector reserve: 目标容量 <= 当前容量（不重新分配）", "[vector][reserve]") {  
    simple_stl::vector<int> vec;  
    vec.reserve(10);  // 第一次扩容到 10  
    vec.reserve(5);   // 目标 5 <= 现有 10，不操作  
    REQUIRE(vec.capacity() == 10);   // 容量不变  
    REQUIRE(vec.start() != nullptr);  // 内存未释放，指针仍有效  
}  

TEST_CASE("vector reserve: 目标容量 > max_size（抛出 length_error）", "[vector][reserve][exception]") {  
    simple_stl::vector<int> vec;  
    size_t max_sz = vec.max_size();  
    // 验证异常类型和信息  
    REQUIRE_THROWS_AS(vec.reserve(max_sz + 1), std::length_error);  
    // 验证异常后状态：容量/大小/指针保持原值  
    REQUIRE(vec.capacity() == 0);  
    REQUIRE(vec.size() == 0);  
    REQUIRE(vec.start() == nullptr);  
}  


TEST_CASE("vector reserve: 扩容后析构函数正确释放内存", "[vector][reserve]") {  
    { // 作用域结束后 vec 析构  
        simple_stl::vector<int> vec;  
        vec.reserve(10); // 分配内存，start_ 非空  
    }  
    // 析构后，内存应被释放（无需断言，通过程序是否崩溃间接验证）  
    // 若析构函数中未正确调用 alloc_.deallocate，此处可能触发野指针错误  
}  


// ----------------------------- 可选：清理静态成员计数（避免测试用例相互影响） -----------------------------  
// 若多个测试用例涉及 MoveOnly，可在每个测试用例开始前重置计数  
// 示例：  
// TEST_CASE("...", "[...]") {  
//     MoveOnly::move_count = 0;  
//     MoveOnly::destruct_count = 0;  
//     // 测试逻辑  
// }  