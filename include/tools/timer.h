#ifndef TIMER_H
#define TIMER_H

#include <sys/epoll.h>
#include <functional>
#include <chrono>
#include <cstdint>
#include <map>
#include <memory>

#include "tools/timertask.h"

using namespace std;
/**
 * @brief Timer的主要逻辑是通过 multimap 批量管理定时任务 “什么时候执行哪些任务”
 * TimerTask封装任务信息，通过函数回调机制实现自动任务的唤醒 实现定时器的异步通信 “任务信息、具体执行什么”
 */
class Timer
{
    using Milliseconds = std::chrono::milliseconds; // 时间间隔类型为毫秒
private:
    /* data */
public:
    // 用static声明的成员函数可以直接通过 类名::函数名 调用，而无需实例化对象
    static uint64_t GetTick ();
    
    // 添加定时器 
    std::shared_ptr<TimerTask> AddTimeout(uint64_t offset, TimerTask::Callback func);
    
    // 删除定时器
    void DelTimeout(std::shared_ptr<TimerTask> task);
    
    // 更新定时器
    void Update(uint64_t now);
    
    // 获取最近的超时时间
    int WaitTime();
    
private:
    std::multimap<uint64_t, std::shared_ptr<TimerTask>> m_timeouts;
};

#endif // TIMER_H