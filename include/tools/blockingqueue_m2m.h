/**
 * @brief 多生产者-多消费者模式 维护两个队列，消费者队列为空时与生产者队列交换（如果生产者队列也为空，则阻塞线程）
 */
#ifndef BLOCKINGQUEUE_M2M_H
#define BLOCKINGQUEUE_M2M_H

#include <queue>
#include <mutex>
#include <string>
#include <iostream>
#include <thread>
#include <cstdint>
#include <sstream>
#include <condition_variable>

template <typename T>
class BlockingQueue_M2M
{
private:
    // 辅助函数，获取当前线程id方便打印日志
    std::string getThreadId() {
        auto tid = std::this_thread::get_id();
        // 先通过hash将thread::id转为uint64_t，再输出到stringstream
        std::hash<std::thread::id> hasher;
        uint64_t tid_hash = hasher(tid);  // 哈希为整数
        std::stringstream ss; // 字符串容器
        ss << tid_hash;  // 将tid_hash存入字符串容器ss
        return ss.str();
    }

    // 消费者队列为空，交换生产者和消费者队列
    size_t SwapQueue_(){
        std::unique_lock<std::mutex> lock(producer_mutex_);

        // 打印等待前的状态
        std::string tid = getThreadId();  
        std::cout << "[线程 " << tid << "] SwapQueue_: 等待生产者队列非空（当前大小: " 
                  << producer_queue_.size() << "）" << std::endl;
        
        // nonblock_ 允许外部停止队列的阻塞等待，用于在不需要等待新数据时作为紧急出口，避免一直卡死在等待中
        not_empty_.wait(lock, [this]{ return !producer_queue_.empty() || nonblock_; });
        
        // 打印交换前的队列状态
        std::cout << "[线程 " << tid << "] SwapQueue_: 准备交换（生产者队列大小: " 
                  << producer_queue_.size() << ", 消费者队列大小: " 
                  << consumer_queue_.size() << "）" << std::endl;

        std::swap(producer_queue_, consumer_queue_);
        
        // 打印交换后的状态
        std::cout << "[线程 " << tid << "] SwapQueue_: 交换完成（新消费者队列大小: " 
                  << consumer_queue_.size() << "）" << std::endl;
        return consumer_queue_.size();
    }
    
    bool nonblock_;
    std::queue<T> producer_queue_;
    std::queue<T> consumer_queue_;
    std::mutex producer_mutex_;
    std::mutex consumer_mutex_;
    std::condition_variable not_empty_;
public:

    // 构造函数 显式初始化 nonblock_ 为 false（默认不退出，阻塞等待任务）
    BlockingQueue_M2M(bool nonblock = false) : nonblock_(nonblock) {}
    
    void Push(const T& obj) {
        std::string tid = getThreadId();
        {
            std::lock_guard<std::mutex> lock(producer_mutex_);
            producer_queue_.push(obj);
            // 打印入队信息（包含当前生产者队列大小）
            std::cout << "[线程 " << tid << "] Push: 任务入队，生产者队列大小变为 " 
                      << producer_queue_.size() << std::endl;
        }
        not_empty_.notify_one();
    }

    bool Pop(T& obj) {
        std::unique_lock<std::mutex> lock(consumer_mutex_);

        std::string tid = getThreadId();  // 当前消费者线程ID
        std::cout << "[线程 " << tid << "] Pop: 开始，消费者队列初始大小: " 
                  << consumer_queue_.size() << std::endl;

        while (consumer_queue_.empty()) {
            std::cout << "[线程 " << tid << "] Pop: 消费者队列为空，准备交换队列" << std::endl;
            size_t swapped_size = SwapQueue_();  // 交换队列

            if (swapped_size == 0 && nonblock_) {
                std::cout << "[线程 " << tid << "] Pop: 队列空且已取消，返回false" << std::endl;
                return false;
            } else if (swapped_size == 0) {
                std::cout << "[线程 " << tid << "] Pop: 交换后仍为空，继续等待" << std::endl;
            }
        }

        // 取任务并打印信息
        obj = consumer_queue_.front();
        consumer_queue_.pop();
        std::cout << "[线程 " << tid << "] Pop: 取出任务，消费者队列剩余大小: " 
                  << consumer_queue_.size() << std::endl;

        return true;
    }

    // 解除阻塞当前队列的线程
    void Cancel(){
        {
            std::lock_guard<std::mutex> lock(producer_mutex_);
            nonblock_ = true;
        }
        not_empty_.notify_all(); // 通知所有等待线程，所有线程被唤醒后重新竞争锁，并检查条件
    };

};



#endif // BLOCKINGQUEUE_M2M_H
/**
 * @brief 生产者与消费者对应各自队列，消费者队列为空时从生产者队列交换 SwapQueue_()、Pop()、Push()、Cancel()
 * @note SwapQueue_() 使用unqiue_lock()因为会使用谓词判断解锁条件（生产者队列不为空或者应急退出）
 * @note Pop()、Push() 使用lock_guard()中途不需要解锁，出队时如果消费者队列为空要进入循环阻塞，
 * 如果消费者队列为空且交换后也为空则应该优雅的退出Pop()函数，间接导致工作线程也退出Worker()
 */
/*
    // 入队：加锁放入队列，并唤醒消费者
    void Push(const T& obj){
        {
            std::lock_guard<std::mutex> lock(producer_mutex_);
            producer_queue_.push(obj);
        }
        not_empty_.notify_one(); // 具体唤醒哪个线程由操作系统进行调度
    };

    bool Pop(T& obj){
        std::unique_lock<std::mutex> lock(consumer_mutex_);
        while(consumer_queue_.empty()){
            if(consumer_queue_.empty() && SwapQueue_() == 0){
                return false; // 当消费者队列为空且生产者队列也没有任务要处理时，优雅退出
            }
        }
        obj = consumer_queue_.front();
        consumer_queue_.pop();
        return  true;
    };
*/