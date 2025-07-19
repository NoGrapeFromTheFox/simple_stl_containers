#define CATCH_CONFIG_MAIN
#include "catch_amalgamated.hpp"
#include "containers/string.h" // 替换为实际头文件路径

// 初始化静态成员变量（假设 str_min_capacity 定义为 16，需与类中声明一致）
const simple_stl::string::size_type simple_stl::string::str_min_capacity = 16;

TEST_CASE("string 类构造函数及成员函数测试", "[string]") {

    // 测试默认构造函数
    SECTION("默认构造函数") {
        simple_stl::string s;
        REQUIRE(s.size() == 0); // 初始长度为0
        REQUIRE(s.capacity() == simple_stl::string::str_min_capacity); // 容量为最小容量
        REQUIRE(std::strcmp(s.c_str(), "") == 0); // 空字符串
        REQUIRE(s.empty() == true); // 为空
    }

    // 测试 C风格字符串构造函数
    SECTION("C风格字符串构造函数") {
        const char* test_str = "hello world";
        simple_stl::string s(test_str);
        
        REQUIRE(s.size() == std::strlen(test_str)); // 长度正确
        REQUIRE(s.capacity() >= s.size()); // 容量不小于长度
        REQUIRE(std::strcmp(s.c_str(), test_str) == 0); // 内容一致
        REQUIRE(s.empty() == false); // 非空
    }

    // 测试 C风格字符串构造函数（空字符串）
    SECTION("C风格字符串构造函数（空串）") {
        simple_stl::string s("");
        REQUIRE(s.size() == 0);
        REQUIRE(s.capacity() == simple_stl::string::str_min_capacity); // 至少为最小容量
        REQUIRE(std::strcmp(s.c_str(), "") == 0);
    }

    // 测试 C风格字符串构造函数（空指针异常）
    SECTION("C风格字符串构造函数（空指针）") {
        REQUIRE_THROWS_AS(simple_stl::string(nullptr), std::invalid_argument); // 抛出异常
    }

    // 测试二进制安全构造函数
    SECTION("二进制安全构造函数") {
        const char test_data[] = {'a', 'b', '\0', 'c'}; // 包含'\0'的二进制数据
        simple_stl::string::size_type len = sizeof(test_data);
        
        simple_stl::string s(test_data, len);
        REQUIRE(s.size() == len); // 长度正确（包含'\0'）
        REQUIRE(s.capacity() >= len); // 容量正确
        REQUIRE(std::memcmp(s.data(), test_data, len) == 0); // 二进制内容一致
        REQUIRE(s.c_str()[len] == '\0'); // 末尾补'\0'
    }

    // 测试二进制安全构造函数（空指针异常）
    SECTION("二进制安全构造函数（空指针）") {
        REQUIRE_THROWS_AS(simple_stl::string(nullptr, 5), std::invalid_argument); // 抛出异常
    }

    // 测试拷贝构造函数
    SECTION("拷贝构造函数") {
        simple_stl::string original("copy test");
        simple_stl::string copy(original); // 拷贝构造
        
        // 内容和长度一致
        REQUIRE(copy.size() == original.size());
        REQUIRE(std::strcmp(copy.c_str(), original.c_str()) == 0);
        // 验证深拷贝（修改原对象不影响拷贝对象）
        original.append("足够长的字符串，触发扩容验证realloc_data函数");
        REQUIRE(std::strcmp(copy.c_str(), "copy test") == 0); // 拷贝对象内容不变
    }
}

TEST_CASE("string 类赋值运算符测试", "[string/assignment]") {
    // 测试拷贝赋值运算符（深拷贝验证）
    SECTION("拷贝赋值运算符") {
        // 普通赋值场景
        simple_stl::string original("copy assign test");
        simple_stl::string target("initial value");
        
        target = original; // 执行拷贝赋值
        
        // 验证赋值后内容和长度一致
        REQUIRE(target.size() == original.size());
        REQUIRE(std::strcmp(target.c_str(), original.c_str()) == 0);
        
        // 验证深拷贝（原对象内存变化不影响目标对象）
        original.reserve(original.capacity() + 10); // 触发原对象内存重分配
        REQUIRE(std::strcmp(target.c_str(), "copy assign test") == 0); // 目标对象内容不变
        
        // 自赋值场景（自身赋值后状态正确）
        simple_stl::string self("self copy");
        const char* self_data = self.data();
        self = self; // 自拷贝赋值
        REQUIRE(self.data() == self_data); // 地址不变（避免不必要的内存操作）
        REQUIRE(std::strcmp(self.c_str(), "self copy") == 0); // 内容正确
    }

    // 测试移动赋值运算符（资源转移验证）
    SECTION("移动赋值运算符") {
        // 普通移动场景
        simple_stl::string original("move assign test");
        simple_stl::string target("old data");
        const char* target_old_data = target.data(); // 记录目标对象原地址
        
        target = std::move(original); // 执行移动赋值
        
        // 验证目标对象接管资源
        REQUIRE(target.size() == 16); // "move assign test"长度为15
        REQUIRE(std::strcmp(target.c_str(), "move assign test") == 0);
        REQUIRE(target.data() != target_old_data); // 地址已变更（接管原对象内存）
        
        // 验证原对象处于空状态（避免二次释放）
        REQUIRE(original.data() == nullptr);
        REQUIRE(original.size() == 0);
        REQUIRE(original.capacity() == 0);
    }

    // 测试移动赋值运算符的自移动场景
    SECTION("移动赋值运算符（自移动）") {
        simple_stl::string s("self move assign");
        const char* s_data = s.data(); // 记录原地址
        size_t s_size = s.size(); // 记录原长度
        
        s = std::move(s); // 执行自移动赋值
        
        // 自移动后应保持有效状态（地址和内容不变）
        REQUIRE(s.data() == s_data); // 地址未变（避免错误释放内存）
        REQUIRE(s.size() == s_size);
        REQUIRE(std::strcmp(s.c_str(), "self move assign") == 0);
    }
}

TEST_CASE("string 类获取函数（getter）测试", "[string/getters]") {
    // 测试空字符串的获取函数
    SECTION("空字符串的获取函数") {
        simple_stl::string s; // 默认构造的空串
        
        REQUIRE(s.size() == 0); // 长度为0
        REQUIRE(s.capacity() == simple_stl::string::str_min_capacity); // 容量为最小容量
        REQUIRE(s.empty() == true); // 为空
        REQUIRE(std::strcmp(s.c_str(), "") == 0); // c_str() 返回空串
        REQUIRE(std::strcmp(s.data(), "") == 0); // data() 返回空串（与c_str()一致）
    }

    // 测试普通字符串的获取函数
    SECTION("普通字符串的获取函数") {
        const char* test_str = "hello getter";
        simple_stl::string s(test_str);
        size_t expected_size = std::strlen(test_str);
        
        REQUIRE(s.size() == expected_size); // 长度正确
        REQUIRE(s.capacity() >= expected_size); // 容量不小于长度
        REQUIRE(s.empty() == false); // 非空
        REQUIRE(std::strcmp(s.c_str(), test_str) == 0); // c_str() 内容正确
        REQUIRE(std::memcmp(s.data(), test_str, expected_size) == 0); // data() 内容正确
        REQUIRE(s.data()[expected_size] == '\0'); // data() 末尾有终止符
    }

    // 测试包含二进制数据（含'\0'）的获取函数
    SECTION("二进制数据的获取函数") {
        const char bin_data[] = {'a', '\0', 'b', 'c'}; // 中间包含'\0'
        size_t bin_len = sizeof(bin_data);
        simple_stl::string s(bin_data, bin_len);
        
        // size() 和 capacity() 不受中间'\0'影响
        REQUIRE(s.size() == bin_len);
        REQUIRE(s.capacity() >= bin_len);
        REQUIRE(s.empty() == false);
        
        // data() 返回完整二进制数据（包括中间'\0'）
        REQUIRE(std::memcmp(s.data(), bin_data, bin_len) == 0);
        
        // c_str() 遇到第一个'\0'终止（与data()的区别）
        REQUIRE(std::strcmp(s.c_str(), "a") == 0);
    }

    // 测试容量变化后的获取函数
    SECTION("容量变化后的获取函数") {
        simple_stl::string s("resize test");
        size_t old_cap = s.capacity();
        
        s.reserve(old_cap + 20); // 扩容
        
        // 扩容后size不变，capacity更新
        REQUIRE(s.size() == std::strlen("resize test")); // 长度不变
        REQUIRE(s.capacity() == old_cap + 20); // 容量正确
        REQUIRE(s.empty() == false); // 仍非空
        REQUIRE(std::strcmp(s.c_str(), "resize test") == 0); // 内容不变
    }

    // 测试拷贝/移动后获取函数的一致性
    SECTION("拷贝/移动后获取函数的一致性") {
        // 拷贝后获取函数应与原对象一致
        simple_stl::string original("copy check");
        simple_stl::string copy(original);
        REQUIRE(copy.size() == original.size());
        REQUIRE(copy.capacity() == original.capacity());
        REQUIRE(copy.empty() == original.empty());
        REQUIRE(std::strcmp(copy.c_str(), original.c_str()) == 0);
        
        // 移动后新对象的获取函数应与原对象一致（原对象变为空）
        simple_stl::string moved(std::move(original));
        REQUIRE(moved.size() == std::strlen("copy check"));
        REQUIRE(moved.capacity() >= moved.size());
        REQUIRE(moved.empty() == false);
        REQUIRE(std::strcmp(moved.c_str(), "copy check") == 0);
    }
}