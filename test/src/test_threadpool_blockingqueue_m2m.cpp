#define CATCH_CONFIG_MAIN
#include <thread>
#include <chrono>
#include <iostream>
#include <atomic>
#include <vector>
#include "tools/threadpool.h"  // 假设ThreadPool内部使用BlockingQueue_M2M, 执行前请修改threadpool.h 中的私有队列成员的类模版为BlockingQueue_O2M, 并引入对应的头文件
#include "catch_amalgamated.hpp"

// 全局原子变量：跟踪总任务数和已完成任务数（线程安全）
std::atomic<int> g_total_tasks(0);
std::atomic<int> g_completed_tasks(0);

// 测试任务函数：输出当前线程信息并标记完成
void test_task(int task_id, int producer_id) {
    auto thread_id = std::hash<std::thread::id>{}(std::this_thread::get_id());
    std::cout << "消费者线程 " << thread_id 
              << " 处理生产者 " << producer_id 
              << " 的任务 " << task_id << std::endl;
    
    // 模拟任务耗时（随机0-50ms，避免输出混乱）
    std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 50));
    g_completed_tasks++;
}

TEST_CASE("多生产者多消费者模型测试", "[BlockingQueue_M2M]") {
    srand(time(nullptr));  // 初始化随机数种子
    
    // 测试参数：2个生产者线程，3个消费者线程，每个生产者生产4个任务
    const int PRODUCER_NUM = 2;
    const int CONSUMER_NUM = 3;
    const int TASKS_PER_PRODUCER = 4;
    g_total_tasks = PRODUCER_NUM * TASKS_PER_PRODUCER;
    g_completed_tasks = 0;

    // 创建线程池（内部使用BlockingQueue_M2M，消费者线程数为CONSUMER_NUM）
    ThreadPool pool(CONSUMER_NUM);

    // 启动多个生产者线程
    std::vector<std::thread> producers;
    for (int p = 0; p < PRODUCER_NUM; ++p) {
        producers.emplace_back([&pool, p]() {  // p为生产者ID
            for (int t = 0; t < TASKS_PER_PRODUCER; ++t) {
                // 每个生产者间隔随机时间提交任务（模拟实际生产节奏）
                std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 80));
                // 提交任务：绑定生产者ID和任务ID，便于追踪
                pool.Post(test_task, t, p);
            }
        });
    }

    // 等待所有生产者完成任务提交
    for (auto& prod : producers) {
        prod.join();
    }

    // 等待所有任务执行完成（超时时间5秒）
    auto start = std::chrono::steady_clock::now();
    while (g_completed_tasks < g_total_tasks) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - start
        ).count();
        if (elapsed > 10) {
            FAIL("任务执行超时，可能存在死锁或队列阻塞");
        }
    }

    // 验证所有任务均被执行
    REQUIRE(g_completed_tasks == g_total_tasks);
}