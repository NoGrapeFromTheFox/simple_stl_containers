#define CATCH_CONFIG_MAIN
#include <thread>
#include <chrono>
#include <iostream>
#include <atomic>
#include "tools/threadpool.h"
#include "catch_amalgamated.hpp"

// 全局原子变量，用于跟踪任务执行总数
std::atomic<int> g_task_count(0);

// 测试任务函数：输出当前线程ID并递增任务计数
void test_task(int task_id) {
    // 获取当前线程ID（转为uint64_t方便输出）
    auto thread_id = std::hash<std::thread::id>{}(std::this_thread::get_id());
    std::cout << "线程 " << thread_id << " 正在执行任务 " << task_id << std::endl;
    
    // 模拟任务执行耗时（随机短延时）
    std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 100));
    g_task_count++;
}

TEST_CASE("线程池生产者消费者模型测试", "[threadpool]") {
    // 初始化随机数种子
    srand(time(nullptr));
    
    // 测试参数：3个工作线程，5个任务
    const int THREAD_NUM = 3;
    const int TASK_NUM = 5;
    
    // 重置任务计数
    g_task_count = 0;
    
    // 创建线程池
    ThreadPool pool(THREAD_NUM);
    
    // 模拟生产者线程：添加任务到队列
    std::thread producer([&]() {
        for (int i = 0; i < TASK_NUM; ++i) {
            // 延迟添加任务，模拟实际生产过程
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            pool.Post(std::bind(test_task, i)); // 假设ThreadPool有AddTask方法添加任务
        }
    });
    
    // 等待生产者完成任务提交
    producer.join();
    
    // 等待所有任务执行完成（最多等待2秒，避免无限阻塞）
    auto start = std::chrono::steady_clock::now();
    while (g_task_count < TASK_NUM) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - start).count() > 2) {
            FAIL("任务执行超时");
        }
    }
    
    // 验证所有任务都被执行
    REQUIRE(g_task_count == TASK_NUM);
}