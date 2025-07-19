#ifndef TIMER_TASK_H
#define TIMER_TASK_H

#include <cstdint>    // uint64_t
#include <functional> // std::function
#include <memory>


class timer;

/**
 * @brief 定时任务类，封装需要延迟执行的回调函数
 * @note 通过友元类Timer管理执行权限
 * @note 什么情况下类需要继承 std::enable_shared_from_this？ 
 * @note 仅当需要在类的成员函数内部将 this 转换为 shared_ptr 时，见timertask::run(),需要确保this在回调函数的生命周期内存活
*
 */
class TimerTask : public std::enable_shared_from_this<TimerTask>{
    friend class Timer; // 允许Timer访问私有成员

public:
    // Callback 的返回值类型为void, 传入参数是shared_ptr<Timertask>类型
    // 
    using Callback = std::function<void(std::shared_ptr<TimerTask> task)>;

    
    TimerTask(uint64_t addtime, uint64_t exectime, Callback func);

    // 获取任务创建时间
    uint64_t AddTime() const;

    // 获取预定执行时间
    uint64_t ExecTime() const;

private:

    void run(); // 执行回调函数(仅供Timer类调用)

    uint64_t m_addTime;   
    uint64_t m_execTime;  
    Callback m_func;      
};

#endif // TIMER_TASK_H