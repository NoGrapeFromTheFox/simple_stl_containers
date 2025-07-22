#ifndef SIMPLE_STL_CONTAINERS_VECTOR_H
#define SIMPLE_STL_CONTAINERS_VECTOR_H

#include "common/allocator.h"    // 自定义分配器
#include "common/iterator.h"     // 迭代器基类（使用原始指针替代）
#include "common/utilities.h"    // 工具函数（示例中包含move/swap等）

#include <cstddef>    // size_t
#include <stdexcept>  // std::length_error
#include <type_traits> // 类型萃取

namespace simple_stl {

template <typename T, typename Allocator = simple_stl::allocator<T>>
class vector {
public:

    // 类型别名
    using value_type = T;
    using allocator_type = Allocator;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using iterator = pointer;          // 简化为原始指针迭代器
    using const_iterator = const_pointer;

private:
    
    allocator_type alloc_;    // 分配器对象，管理内存
    pointer start_;            // 指针，指向数据起始位置
    pointer finish_;           // 指针，指向最后一个元素的下一个位置
    pointer end_of_storage_;    // 指针，已分配内存的末尾位置
    size_type size_;          // 已存元素数量
    size_type capacity_;      // 总容量，可存储元素的最大数量

public:
    // 构造函数: 仅初始化空容器
    vector() : alloc_(), start_(nullptr),finish_(nullptr), end_of_storage_(nullptr), capacity_(0),size_(0) {}

    // 析构函数
    ~vector() {
        if (start_) {
            // 释放内存（使用分配器释放）
            alloc_.deallocate(start_, capacity_);
            start_ = nullptr;
        }
    }

    // 获取当前元素数量
    size_type size() const { return size_; }

    // 获取总容量
    size_type capacity() const { return capacity_; }

    size_type max_size() const { return alloc_.max_size(); } // 简单实现，返回分配器支持的最大值
    
    const pointer start() const { return start_; }
        iterator end(){
        return iterator(this->finish_);
    }

    const_iterator end() const{
        return const_iterator(this->finish_);
    }

    reference operator[](size_type n){
        return *(start_ + n);
    }

    const_reference operator[](size_type n) const {
        return *(start_ + n);
    }

    void reserve(size_type n) {

        if (n > max_size()) {
            throw std::length_error("vector::reserve");
        }

        if (capacity_ >= n) return;

        // 保存旧状态
        size_type old_size = size_;
        pointer old_data = start_;
        pointer old_finish = finish_;

        // 分配新内存
        pointer new_data = alloc_.allocate(n);

        try {
            // 使用 uninitialized_move 转移元素（移动构造 + 销毁旧对象）
            if (start_) {
                
                simple_stl::uninitialized_move(start_, start_ + old_size, new_data);
                // 销毁旧内存中的对象（已通过移动构造转移，此处需确保旧对象可正确销毁）
                // 调用对象析构函数
                for(auto p = start_; p < start_ + old_size; ++p){
                    alloc_.destroy(p);
                }
            }
        } catch (...) {
            // 异常安全：若转移过程中抛出异常，释放新内存并恢复旧状态
            alloc_.deallocate(new_data, n);
            start_ = old_data; // 恢复旧数据指针（仅在有旧数据时有效）
            finish_ = old_finish;
            throw; // 重新抛出异常
        }

        // 释放旧内存
        alloc_.deallocate(old_data, capacity_);

        // 更新状态
        start_ = new_data;
        finish_ = new_data + old_size;
        capacity_ = n;
        end_of_storage_ = new_data + n;
}

    // 为了避免写复杂的类模版限定符，简化成内联实现
    template<typename... Args> 
    reference emplace_back(Args&& ...args){
        if(size_ >= capacity_){
            size_type new_capacity = (capacity_ == 0)?1: capacity_ * 2;
            reserve(new_capacity);
        }
        alloc_.construct(finish_, std::forward<Args>(args)...);
        ++finish_;
        ++size_;
        return *(finish_ - 1); // 返回新插入元素的引用
    }

    void push_back(const T& value){
        emplace_back(value);
    }

    void push_back(T&& value){
        emplace_back(std::move(value));
    }

    void pop_back(){
        if(size_ == 0){
            throw std::out_of_range("vector::pop_back: container is empty!");
        }
        alloc_.destroy(finish_ - 1);
        --finish_;
        --size_;
    }
};

} // namespace simple_stl

#endif // SIMPLE_STL_CONTAINERS_VECTOR_H

/**
 * @brief 主要实现模拟动态数组的数据结构（三个指针+连续内存），简单实现插入、删除、扩容基础操作，
 * 暂时没有实现： 移动、赋值 四大成员函数
 */