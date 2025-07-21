#define CATCH_CONFIG_MAIN
#include "catch_amalgamated.hpp"
#include "tools/singletonpattern.h"

#include <thread>
#include <vector>
#include <atomic>

/**
 * @brief 多个线程调用公开GetInstance接口时，全局只创建一个instance实例
 */
TEST_CASE("SingletonPattern thread safety", "[singleton]") {
    std::vector<SingletonPattern*> instances; // 存储所有线程获取到的实例指针
    std::atomic<bool> start_flag(false);
    const int thread_count = 500;  // 增加线程数
    std::mutex vec_mutex;  // 保护 vector 并发写入

    // 多线程并发调用 GetInstance,获取实例并保存
    auto task = [&]() {
        while (!start_flag.load(std::memory_order_relaxed)) {}
        SingletonPattern* instance = SingletonPattern::GetInstance();
        std::lock_guard<std::mutex> lock(vec_mutex);  // 加锁写入
        instances.push_back(instance);
    };

    // 创建、并触发所有线程开始执行
    std::vector<std::thread> threads;
    for (int i = 0; i < thread_count; ++i) {
        threads.emplace_back(task);
    }

    start_flag.store(true, std::memory_order_relaxed);
    for (auto& t : threads) {
        t.join();
    }

    REQUIRE_FALSE(instances.empty());
    SingletonPattern* first_instance = instances[0];
    for (auto instance : instances) {
        CHECK(instance == first_instance);
    }

    // 额外验证：最后一次获取仍唯一
    SingletonPattern* final_instance = SingletonPattern::GetInstance();
    CHECK(final_instance == first_instance);
}