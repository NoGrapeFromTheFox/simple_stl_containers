#ifndef BLOCKINGQUEUE_O2M_H
#define BLOCKINGQUEUE_O2M_H

#include <mutex>
#include <queue>
#include <condition_variable>

using namespace std;

template<typename T>
class BlockingQueue_O2M
{
private:
    bool nonblock_;
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable not_empty_; // 控制队列状态，为空时线程休眠，否则消费者取出任务并执行

public:

    BlockingQueue_O2M(bool nonblock = false):nonblock_(nonblock) {}

    // 入队：加锁放入队列，并唤醒消费者
    void Push(const T& obj){
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(obj);
        not_empty_.notify_one(); // 具体唤醒哪个线程由操作系统进行调度
    };

    /**
     * @brief 出队：返回false时说明队列异常退出，工作线程会退出
     * @note std::condition_variable::wait两个参数分别是锁对象，和唤醒条件谓词
     * @note unique_lock() 与 lock_guard() 区别
     * lock_guard() 简单，无法中途手动解锁，拷贝和移动语义都被禁用
     * unique_lock() 支持手动解锁unlock()、重新加锁lock(),适用于条件判断wait、锁的所有权转移等
     */
    bool Pop(T& obj){
        std::unique_lock<std::mutex> lock(mutex_);
        not_empty_.wait(lock, [this]{ return !queue_.empty() || nonblock_; });
        if(queue_.empty()){
            return false;
        }
        obj = queue_.front();
        queue_.pop();
        return  true;
    };

    // 解除阻塞当前队列的线程
    void Cancel(){
        std::lock_guard<std::mutex> lock(mutex_);
        nonblock_ = true;
        not_empty_.notify_all(); // 通知所有等待线程，所有线程被唤醒后重新竞争锁，并检查条件
    };

};

#endif // BLOCKINGQUEUE_O2M_H