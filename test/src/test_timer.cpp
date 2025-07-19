#define CATCH_CONFIG_MAIN
#include <chrono>
#include <thread>
#include <atomic>
#include <iostream>
#include "tools/timer.h"
#include "catch_amalgamated.hpp"

TEST_CASE("Timer basic functionality with logs", "[timer][log]") {
    auto timer = std::make_unique<Timer>();
    std::atomic<int> count(0);
    std::atomic<bool> flag1(false), flag2(false), flag3(false);

    // 100ms任务
    timer->AddTimeout(100, [&](std::shared_ptr<TimerTask>task) {
        count++;
        flag1 = true;
        std::cout << "[Log] 100ms task executed. AddTime: " << task->AddTime() 
                  << ", Current tick: " << Timer::GetTick() 
                  << ", Count: " << count << std::endl;
    });

    // 200ms任务
    timer->AddTimeout(200, [&](std::shared_ptr<TimerTask>task) {
        count++;
        flag2 = true;
        std::cout << "[Log] 200ms task executed. AddTime: " << task->AddTime() 
                  << ", Current tick: " << Timer::GetTick() 
                  << ", Count: " << count << std::endl;
    });

    // 300ms任务
    timer->AddTimeout(300, [&](std::shared_ptr<TimerTask>task) {
        count++;
        flag3 = true;
        std::cout << "[Log] 300ms task executed. AddTime: " << task->AddTime() 
                  << ", Current tick: " << Timer::GetTick() 
                  << ", Count: " << count << std::endl;
    });

    // 注册后删除的任务
    auto task = timer->AddTimeout(150, [&](std::shared_ptr<TimerTask>task) {
        count++;  // 此回调不应执行
        std::cout << "[Log] 150ms task executed (UNEXPECTED!)" << std::endl;
    });
    std::cout << "[Log] Deleting 150ms task" << std::endl;
    timer->DelTimeout(task); // 立即删除任务

    // 模拟事件循环（带过程日志）
    auto start = Timer::GetTick();
    std::cout << "[Log] Start testing, initial tick: " << start << std::endl;
    while (Timer::GetTick() - start < 400) { // 循环400毫秒
        auto wait_ms = timer->WaitTime(); // 下一个任务还需等待毫秒数
        // std::cout << "[Log] Waiting " << wait_ms << "ms for next task" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(wait_ms)); // 休眠等待
        auto now = Timer::GetTick(); // 获取当前时间戳
        // std::cout << "[Log] Updating timer at tick: " << now << std::endl;
        timer->Update(now); // 检查所有到期的任务，执行它们的回调函数
    }

    // 验证结果
    INFO("Total executed tasks: " << count);
    REQUIRE(count == 3);
    INFO("100ms task status: " << (flag1 ? "executed" : "not executed"));
    REQUIRE(flag1 == true);
    INFO("200ms task status: " << (flag2 ? "executed" : "not executed"));
    REQUIRE(flag2 == true);
    INFO("300ms task status: " << (flag3 ? "executed" : "not executed"));
    REQUIRE(flag3 == true);
}
