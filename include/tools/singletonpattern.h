#ifndef SINGLETONPATTERN_H
#define SINGLETONPATTERN_H

#include <atomic>
#include <mutex>
#include <cstdlib>

class SingletonPattern
{
private:
    static void Destructor();
    static std::atomic<SingletonPattern*> instance_;
    static std::atomic<bool> destructor_registerd_; // 确保atexit（destructor）只注册一次
    static std::mutex mutex_;
    ~SingletonPattern() {}
    SingletonPattern(const SingletonPattern& other) = delete;
    SingletonPattern& operator=(const SingletonPattern& other) = delete;
    SingletonPattern(SingletonPattern&& other) noexcept = delete;
    SingletonPattern& operator=(SingletonPattern && other) noexcept = delete;

protected:
    SingletonPattern() = default;

public:
    static SingletonPattern* GetInstance();

};

#endif // SINGLETONPATTERN_H