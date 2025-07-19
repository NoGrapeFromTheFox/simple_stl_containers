#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
#include <thread>
#include <functional>
#include "tools/blockingqueue_o2m.h"
#include "tools/blockingqueue_m2m.h"

class ThreadPool {
public:
    // 构造线程池
    explicit ThreadPool(int nums_threads);

    ~ThreadPool();

    /**
     * @brief 发布任务到线程池
     * @note 模板函数的实现要放在头文件中，因为其实例化是在编译期间进行，
     * 其他文件包含头文件时如果只能看到函数声明，会导致链接错误
     * @note bind()相当于，将参数args塞进可调用对象f中
     * @note std::forward<Args>(args)... 其中三个点在括号外面，相当于编译器一个参数一个参数的处理，将它放在括号内编译器会因为看到“一坨糊在一起的东西”而报错
     */
    template<typename F, typename... Args>
    void Post(F &&f, Args &&...args){

        auto task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        task_queue.Push(task);
    }; // 参数传递采用“&&万能引用”，传入左值执行拷贝，传入右值触发移动语义，尽可能减少参数传递时的语义开销

private:
    // 工作线程入口
    void Worker();
    BlockingQueue_M2M<std::function<void()>> task_queue; // 任务队列，存储可调用对象
    std::vector<std::thread> workers_; // 工作线程池，存储工作线程

};

#endif // THREADPOOL_H

/**
 * @brief ①工作线程入口Worker()、②工作线程池 workers_ 、③务队列 task_queue 、④阻塞队列模板类的实现
 * @note 使用Post()模板接口中的std:: bind(),将任务打包成std::function<void()> 
 * 统一标准化的“无参数、无返回值”的格式，使线程池无需关心具体任务细节，只需按照统一方式执行
 */