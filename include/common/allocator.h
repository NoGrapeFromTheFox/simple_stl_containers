#ifndef SIMPLE_STL_ALLOCATOR_H
#define SIMPLE_STL_ALLOCATOR_H

#include <cstddef>      // size_t, ptrdiff_t
#include <new>          // placement new
#include <type_traits>


namespace simple_stl {

// 主模板 allocator<T>
template<typename T>
class allocator {
public:
    // 定义类型别名
    typedef T         value_type;
    typedef T*        pointer;
    typedef const T*  const_pointer;
    typedef T&        reference;
    typedef const T&  const_reference;
    typedef size_t    size_type;       // 无符号整数类型的别名（不同系统下可能不同 64 unsign long 32 unsign int）
    typedef ptrdiff_t difference_type; // 两个迭代器之间距离

    // 允许动态获取其他类型的分配器 allocator<T>::rebind<U>::other 使外部代码可以通过rebind<U>::other 访问到 allocator<U>
    template<typename U>
    struct rebind { typedef allocator<U> other; };

    // 默认构造/拷贝构造
    allocator() noexcept = default;
    allocator(const allocator&) noexcept = default;

    // 模板化的拷贝构造函数
    template<typename U>
    allocator(const allocator<U>&) noexcept {}

    size_type max_size() const noexcept{
        return std::numeric_limits<size_type>::max()/sizeof(T);
    }
    
    // 内存分配/释放
    pointer allocate(size_type n) {
        // 返回类型void*前置转化成T*,即pointer
        // ::operator new C++全局内存分配函数（不调用析构函数，可以重写，需要强制类型转换，类似于malloc）
        return static_cast<pointer>(::operator new(n * sizeof(T)));
    }

    void deallocate(pointer p, size_type) {
        ::operator delete(p);
    }

    // 通过对参数的完美转发，实现多个对象在p指向的地址上的构造
    // c++17的标准中，在<type_traits>加入了模板， is_trivially_constructible_v 可以进行if constexpr判断（与编译器配合）实现对一些平凡类型的构造优化
    template<typename U, typename... Args>
    void construct(U* p, Args&&... args) {
        // 在指定地址分配对象 placement new 语法::new (address) Type(initializer_arguments);
        // alloc_.construct(finish_++, *it); 这里*it是U类型的对象，并且*it会作为args传入U(*it)中, 触发元素U的拷贝构造函数
        ::new((void*)p) U(std::forward<Args>(args)...);
    }

    template<typename U>
    void destroy(U* p) {
        p->~U();
    }
};

// allocator<void> 特化
template<>
class allocator<void> {
public:
    typedef void* pointer;
    typedef const void* const_pointer;
    typedef void value_type;

    template<typename U>
    struct rebind { typedef allocator<U> other; };
};

// 比较操作符 == 和！=
template<typename T1, typename T2>
bool operator==(const allocator<T1>&, const allocator<T2>&) noexcept {
    return true;
}

template<typename T1, typename T2>
bool operator!=(const allocator<T1>&, const allocator<T2>&) noexcept {
    return false;
}

} // namespace simple_stl
#endif // SIMPLE_STL_ALLOCATOR_H

/**
 * @brief rebind接口通过定义别名，实现分配器类型的灵活转换；
 * allocator接口 static_cast<Type>强制转换 + “:operator new”: 包装底层内存分配函数
 * construct接口： 完美转发forword + palcement new在已分配内存上构造对象
 * @note 区分 ::operator new、 new 、 malloc、 placement new
 * ① ::operator new :c++底层内存分配函数工具，类内可以重载它实现自定义分配，失败时抛出异常。
 * ② placement new: 在已有内存上构造对象，常用语内存复用场景
 * ③ new： c++表达式本质是“::operator new”分配内存+“placement new”构造对象的组合，面向对象
 * ④ malloc: c标准库函数，只分配内存，分配失败会返回nullptr，使用前要检查分配情况
 */