#include "tools/timertask.h"

/** @brief 封装任务创建时间、任务预计执行时间、回调函数，并且仅向Timer提供run接口,
 * 传入this指针调用自身的，类型为CallBack 类类型的，私有成员m_func
    @note (std::move(obj)) 将obj转为右值引用，实际上是“请求”编译器优先使用obj的“移动语义”，
    具体如何转移资源由obj的移动语义决定，类比于obj交出指针所有权后，将指针置空
*/
TimerTask::TimerTask(uint64_t addtime, uint64_t exectime, Callback func)
    : m_addTime(addtime), m_execTime(exectime), m_func(std::move(func)) {}

uint64_t TimerTask::AddTime() const {
    return m_addTime;
}

uint64_t TimerTask::ExecTime() const {
    return m_execTime;
}

void TimerTask::run() {
    // m_func 是一个函数对象成员，在判断其有效后，带着自身智能指针调用该回调函数
    if (m_func) {
        // 将this转换为shared_ptr传递给回调，保证对象在回调期间存活
        m_func(std::shared_ptr<TimerTask>(shared_from_this())); 
    }
}
/**
 * @note std::function<>类类型模板（类比传入函数返回值类型和各参数类型，实例化一个函数对象），
 * Callback 是function<>实例出来的函数对象的简化签名，是一个类类型
 * 成员函数run()，通过类的成员this指针，回调本身的成员变量（Callback 类型的 m_func）
 */