/* 数据结构使用multimap、函数对象作为闭包解决异步回调、 */
/*
什么情况下类需要继承 std::enable_shared_from_this？
仅当需要在类的成员函数内部将 this 转换为 shared_ptr 时，见timertask::run(),需要确保this在回调函数的生命周期内存活
*/
#include "tools/timer.h"

// 用static声明的成员函数可以直接通过 类名::函数名 调用，而无需实例化对象
uint64_t Timer::GetTick() {
    auto sc = chrono::time_point_cast<Milliseconds>(chrono::steady_clock::now()); // 获取当前时间点，转化为毫秒
    auto temp = chrono::duration_cast<Milliseconds>(sc.time_since_epoch()); // 获取时间距离"纪元"间隔，转化为毫秒
    return temp.count(); // 返回毫秒级时间戳，数值部分
}

// 添加定时器 
std::shared_ptr<TimerTask> Timer::AddTimeout(uint64_t offset, TimerTask::Callback func) {
    auto now = GetTick();
    auto exectime = now+offset;
    auto task = std::make_shared<TimerTask>(now, exectime, std::move(func));
    // 性能优化
    if(m_timeouts.empty() || exectime >= m_timeouts.crbegin()->first){
        // multimap.crbegin() 返回常量反向迭代器
        // xx.emplace_hint() 第一个参数的插入位置，将插入操作优化为O(1)，返回值指向新插入元素本身的迭代器
        auto ele = m_timeouts.emplace_hint(m_timeouts.end(), exectime, task);
        return ele->second;
    }
    m_timeouts.emplace(exectime, task);
    return task;
}

// 删除定时器
void Timer::DelTimeout(std::shared_ptr<TimerTask> task) {
    // 查找多个将要超时的任务, 返回值为迭代器对儿，range.second迭代器指向最后一个目标的下一个位置
    auto range = m_timeouts.equal_range(task->ExecTime()); 
    for (auto it = range.first; it != range.second;) {
        if (it->second == task) {
           it = m_timeouts.erase(it);
        } else {
            ++it;
        }
    }
}

// 更新定时器：批量处理到期任务
void Timer::Update(uint64_t now) {
    auto it = m_timeouts.begin();
    while (it != m_timeouts.end() && it->first <= now) {
        it->second->run();
        // delete it->second; 如果没有使用智能指针，原始指针可以手动删除
        it = m_timeouts.erase(it); // 返回值为被删除元素的下一个迭代器
    }
}

// 获取最近的超时时间
int Timer::WaitTime() {
    auto it = m_timeouts.begin();
    if (it == m_timeouts.end()) {
        return -1;
    }
    int diss = it->first - GetTick();
    return diss > 0 ? diss : 0;
}