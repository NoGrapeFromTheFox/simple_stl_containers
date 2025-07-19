#include <tools/threadpool.h>

/**
 * @note workers_是一个线程池对象，emplace_back()会尝试在容器末尾直接构造一个与容器元素类型匹配的对象，这里是std::thread
 * lambda函数通过捕捉this指针获取获得访问 ThreadPool成员的能力，使新创建的进程执行Worker(),进入工作状态
 */
ThreadPool::ThreadPool(int nums_threads){
    for(size_t i = 0; i < nums_threads; i++){
        workers_.emplace_back([this]{ Worker();});
    }
}

ThreadPool::~ThreadPool(){
    task_queue.Cancel();
    for(auto& worker : workers_){
        if(worker.joinable()){  
            worker.join();
        }
    }
}
// 工作线程不断从队列中取出任务并执行任务，直到退出
void ThreadPool::Worker(){
    while (true)
    {
        std::function<void()> task;
        if(!task_queue.Pop(task)){
            break;
        }
        task();
    }
    
}