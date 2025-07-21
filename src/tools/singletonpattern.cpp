#include "tools/singletonpattern.h"

// 静态成员定义
std::atomic<SingletonPattern*> SingletonPattern::instance_{nullptr};
std::atomic<bool> SingletonPattern::destructor_registerd_(false);
std::mutex SingletonPattern::mutex_{};

void SingletonPattern::Destructor(){
    SingletonPattern* tmp = instance_.load(std::memory_order_relaxed);
    if(tmp != nullptr){
        delete tmp;
        instance_.store(nullptr, std::memory_order_relaxed);
    }
}

/**
 * @note 保证线程安全的获取实例方式：两次检查+中间加锁，
 * 第一次非空检查：避免已经存在的实例进行无意义加锁
 * 第二次非空检查：在加锁保护下，防止多线程并发创建实例
 * @note 
 * ① acquire主要防止后面的 if load 插队到 第一个load前，
 * ② release主要防止前面的if load 滞后到store以后
 * @note 测试过程中发现多个线程同时注册destructor并重复释放问题 
 * --> 原子布尔值+compare_exchange_strong()
 * atomic_variable.compare_exchange_strong（expected, new_value）
 * 如果原子变量与“预期值expected”相等，修改它为新值new_value，否则修改预期值为原子变量当前值
 * 所以在代码中，只有第一次相等，原子变量值变成true, 后面比较都不相等原子变量true预期值false，
 * 整个函数返回值是“比较失败”返回false避免进入if,修改预期值为当前原子变量可以视为多余步骤。
 */

SingletonPattern* SingletonPattern::GetInstance(){
    SingletonPattern* tmp = instance_.load(std::memory_order_relaxed); // load()保证原子地读取
    std::atomic_thread_fence(std::memory_order_acquire); // 内存屏障：禁止cpu对后续读写指令进行前排，确保“读到最新的更新指令”
    if(tmp == nullptr){
        std::lock_guard<std::mutex> lock(mutex_);
        tmp = instance_.load(std::memory_order_relaxed);
        if(tmp == nullptr){
            tmp = new SingletonPattern;
            std::atomic_thread_fence(std::memory_order_release); // 内存屏障：禁止cpu对前面读写指令后排、禁止对后续写指令前排，确保“自己的修改被他人同步”
            instance_.store(tmp, std::memory_order_relaxed); // store()原子地写入
            // 避免多个线程同时注册退出处理函数
            bool expected = false;
            if(destructor_registerd_.compare_exchange_strong(expected, true)){
                atexit(Destructor); // 注册退出处理函数
            }
        }
    }
    return tmp;
}

/**
 * @brief 本文件实现版本主要侧重于对线程安全、和内存序的练习、下面有更简单的实现方法：
 * @note 最简单方式：在静态方法GetInstance()中声明静态局部实例instance（懒加载），返回其别名
 * static SingletonPattern* GetInstance(){
 *      static SingletonPattern instance;
 *      return &instance;
 * }
 */