/*六大成员函数、重写解引用和箭头运算符、引用计数（考虑线程安全）、重置指针*/
/*
    不考录删除器和空间配置器
    不考虑弱引用
*/
#ifndef SIMPLE_STL_CONTAINERS_SHARED_PTR_H
#define SIMPLE_STL_CONTAINERS_SHARED_PTR_H

#include <atomic> // 引入原子操作
namespace simple_stl {
template <typename T>
class shared_ptr
{
public:
    using size_type = std::size_t;

private:
    /* data */
    T* ptr;
    std::atomic<size_type>* ref_count; // 计数

    // 释放资源 内存序接口
    void release(){
        // memory_order_relaxed 不关注原子操作顺序，仅保证原子操作本身的原子性
        // memory_order_acquire “确保自己的信息是最新的同步结果” 允许cpu对前面的读写指令向后排，不允许cpu将后面的读写指令前排
        // memory_order_release “只保证自己的修改被同步” 不允许cpu对前面的读写指令进行重拍，不允许写指令前排，允许cpu将后面的读指令前排，
        // memory_order_acq_rel 进入该操作时同步别人的修改，离开该操作时让别人同步自己的修改
        if(ref_count && ref_count->fetch_sub(1,std::memory_order_acq_rel) == 1){
            delete ptr;
            delete ref_count;
        }
    }
public:
    shared_ptr(): ptr(nullptr), ref_count(nullptr) {}
    // 加入explicit防止发生 share_ptr(int) p = new int(200) 类似的隐式类型转换
    explicit shared_ptr(T* p): ptr(p), ref_count(p? new std::atomic<size_type>(1): nullptr) {}
    
    ~shared_ptr() {
        release();
    }

    // 拷贝构造函数
    shared_ptr(const shared_ptr<T>& other): ptr(other.ptr),ref_count(other.ref_count) {
        if(ref_count) {
            ref_count->fetch_add(1,std::memory_order_relaxed);
        }
    }

    // 拷贝赋值运算符
    shared_ptr<T> operator=(const shared_ptr<T>& other) {
        // 内存地址不相等则释放当前资源，并拷贝成员变量
        if(this != &other) {
            release();
            ptr = other.ptr;
            ref_count = other.ref_count;
            if(ref_count) {
                ref_count = ref_count->fetch_add(1, std::memory_order_relaxed);
            }
        }
        return *this;
    }

    // 移动构造函数
    shared_ptr(shared_ptr<T>&& other) noexcept : ptr(other.ptr), ref_count(other.ref_count) {
        other.ptr = nullptr;
        other.ref_count = nullptr;
    }

    // 为什么"移动运算符"返回类型是 T& 对象别名：返回 “=”左操作数的别名，支持 a = b = c = d 链式操作，每一步生成并返回左操作数如c的别名，避免每一步都要生成c的副本
    shared_ptr<T>& operator= (shared_ptr<T>&& other) noexcept{
        if(this != &other){
            release();
            ptr = other.ptr;
            ref_count = other.ref_count;
            other.ptr = nullptr;
            other.ref_count = nullptr;
        }
        return *this; // 语义层面：函数可以根据返回值是引用类型，而将对象本身处理为对该对象的引用
    }

    
    T& operator*() const { 
        return *ptr;  // T类型对象本身 
    }

    // 箭头运算符原始语义：通过指针访问对象的成员 p->member = (*p).member, 因此重写时要返回shared_ptr的T*类型指针成员变量
    T* operator->() const { return ptr; }

    size_type use_count()  const{
        return ref_count? ref_count->load(std::memory_order_acquire) : 0;
    }

    // const修饰函数确保编译期间不会修改类的任何非mutable成员变量
    // get()函数的本质是返回指向被管理T对象的原始指针，即share_ptr中的T*成员变量
    T* get() const {
        return ptr; // 返回指针本身（地址），指针本身const（地址不变，数据可变）
    }

    // 重置指针
    void reset(T* p = nullptr) {
        release();
        ptr = p;
        ref_count = p? new std::atomic<size_type>(1) : nullptr;
    }
};

} // namespace simple_stl


#endif // SHARE_PTR

/**
 * @note: shared_ptr四个缺点
 * ① 菱形引用导致计数无法清空，导致内存泄漏问题 --> 使用 weak_ptr()
 * ② 多个shared_ptr管理同一个裸指针 shared_ptr<T> shared_ptr_name(raw_ptr)，可能导致悬垂指针 
 * --> 使用 make_shared()
 * ③ 从shared_ptr中取出原始指针或引用，并长期保存传递它时，要确保原始指针在此期间不被销毁 
 * --> 一直保持传递智能指针/ 在传入后备份保持至少有一个shared_ptr实例在别名引用期间存活
 * ④ 不适用于高性能、实时性系统
 */