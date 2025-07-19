#define CATCH_CONFIG_MAIN
#include <memory>
#include <thread>
#include <vector>
#include <atomic>
#include <iostream>
#include "catch_amalgamated.hpp"

// 测试shared_ptr的线程安全性（主要验证引用计数的线程安全）
TEST_CASE("shared_ptr thread safety: reference count concurrent modification", "[shared_ptr][thread_safety]") {
    // 创建一个共享对象，初始引用计数为1
    auto shared_obj = std::make_shared<int>(0);
    // 原子变量用于记录所有线程完成后的总操作次数（确保线程执行正确）
    std::atomic<int> total_operations(0);
    const int thread_count = 10;  // 线程数量
    const int operations_per_thread = 100000;  // 每个线程的操作次数（增大次数提高测试压力）

    // 存储所有线程的容器
    std::vector<std::thread> threads;
    threads.reserve(thread_count);

    // 启动10个线程，并发修改引用计数
    for (int i = 0; i < thread_count; ++i) {
        threads.emplace_back([&]() {
            for (int j = 0; j < operations_per_thread; ++j) {
                // 复制shared_ptr（引用计数+1）
                std::shared_ptr<int> temp = shared_obj;
                // 临时变量销毁时引用计数自动-1，无需手动操作
                total_operations.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }

    // 等待所有线程完成
    for (auto& t : threads) {
        t.join();
    }

    // 验证总操作次数是否正确（确保所有线程都执行了预期操作）
    REQUIRE(total_operations == thread_count * operations_per_thread);
    // 验证最终引用计数是否为1（所有临时shared_ptr销毁后，回到初始状态）
    REQUIRE(shared_obj.use_count() == 1);
}

// 自定义删除器：用于追踪内存释放并输出日志
struct LoggingDeleter {
    void operator()(int* ptr) const {
        if (ptr != nullptr) {
            std::cout << "[日志] 内存地址 " << static_cast<void*>(ptr) << " 已释放" << std::endl;
            delete ptr; // 实际释放内存
        }
    }
};

/**
 * 
 * @note 这个“无法通过的测试用例”是为了测试：多个shared_ptr管理同一个裸指针，会造成悬垂指针问题，
 * 会因为多次释放同一块内存（操作系统的内存管理模块发现程序试图再次释放“已释放”的内存，而发送中断信号）而责令程序崩溃退出
 */
TEST_CASE("多shared_ptr管理同一裸指针及释放日志", "[shared_ptr/double_free_log]") {
    // 1. 创建裸指针并打印初始地址（方便对照日志）
    int* raw_ptr = new int(100);
    std::cout << "[日志] 初始分配内存地址: " << static_cast<void*>(raw_ptr) << std::endl;
    REQUIRE(raw_ptr != nullptr);

    // 2. 多个shared_ptr使用自定义删除器接管同一裸指针（错误用法）
    std::shared_ptr<int> sp1(raw_ptr, LoggingDeleter());
    std::shared_ptr<int> sp2(raw_ptr, LoggingDeleter());
    std::shared_ptr<int> sp3(raw_ptr, LoggingDeleter());

    SECTION("初始状态验证") {
        REQUIRE(sp1.get() == raw_ptr);
        REQUIRE(sp2.get() == raw_ptr);
        REQUIRE(sp3.get() == raw_ptr);
        REQUIRE(*sp1 == 100);
    }

    SECTION("逐步释放并打印日志") {
        // 第一次释放：触发删除器，输出释放日志
        std::cout << "[操作] 释放sp1..." << std::endl;
        sp1.reset();
        REQUIRE(sp1 == nullptr);

        std::cout << "[操作] 释放sp2（此时内存已无效）..." << std::endl;
        sp2.reset(); // 第二次释放：触发删除器，但内存已释放，会导致错误

        std::cout << "[操作] 释放sp3（再次释放无效内存）..." << std::endl;
        sp3.reset(); // 第三次释放：必然触发严重内存错误
    }
}