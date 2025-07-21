/**
 * @brief 六大成员函数、实现append函数、为简化实现短字符串不用栈优化
 * 构造函数要求：默认是一个空字符串、一个提供c风格字符串构造+二进制安全的构造函数（提供长度）
 * @note ①移动语义使用noexcept
 * ② 拷贝构造函数、拷贝运算符 传入参数 const string& other_str
 * ③ "operator=运算符"返回值为别名 string&
 * ④ 拷贝（赋值）、移动运算符需要进行自赋值检查
 */
#ifndef SIMPLE_STL_CONTAINERS_STRING_H
#define SIMPLE_STL_CONTAINERS_STRING_H

#include <cstddef> // size_t
#include <algorithm>
#include <cstring>

namespace simple_stl {

class string
{
public:
    using size_type = std::size_t;
    static const size_type str_min_capacity;
private:
    char* data_; // 数据部分
    size_type size_; // 字符串长度（实际）
    size_type capacity_; // 字符串容量

    // 辅助函数：重新分配内存
    void realloc_data(size_type new_cap); 

public:
    // 默认构造函数
    inline string():size_(0),capacity_(str_min_capacity){
        data_ = new char[capacity_ + 1];
        data_[0] = '\0';
    }

    // c风格字符串构造
    inline string(const char* str){
        if(!str){
            throw std::invalid_argument("null pointer");
        }
        size_ = std::strlen(str);
        capacity_ = std::max(size_, str_min_capacity);
        data_ = new char[capacity_ + 1];
        std::memcpy(data_, str, size_ + 1); // 这里在进行内存复制的时候要把字符串尾部标识符也带上
    }

    // 二进制安全构造函数
    inline string(const void* data, size_type len){
        if(!data){
            throw std::invalid_argument("null pointer");
        }
        size_ = len;
        capacity_ = std::max(len, str_min_capacity);
        data_ = new char[capacity_ + 1];
        std::memcpy(data_, data, size_ + 1);
        data_[size_] = '\0'; // 补一个字符串尾部标识
    }

    // 拷贝构造
    inline string(const string& other): size_(other.size_), capacity_(other.capacity_) {
        data_ = new char[capacity_ + 1];
        std::memcpy(data_, other.data_, size_ + 1);
    }

    // 拷贝（赋值）运算符
    inline string& operator= (const string& other) {
        if(this != &other){
            char* new_data = new char[other.capacity_ + 1];
            std::memcpy(new_data, other.data_, other.size_ + 1);
            delete[] data_;
            data_ = new_data;
            size_ = other.size_;
            capacity_ = other.capacity_;
        }
        return *this;
    }

    // 移动构造
    inline string(string&& other) noexcept: data_(other.data_), size_(other.size_), capacity_(other.capacity_){
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    // 移动赋值运算符
    inline string& operator=(string&& other) noexcept {
        if(this != &other ){
            delete[] data_;
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }

    ~string(){
        delete[] data_;
    };

    // 预分配内存
    void reserve(size_type new_cap);
    // 释放多余内存
    void shrink_to_fit();
    // 两种风格的append函数
    string& append(const char* str, size_type len);
    string& append(const char* str);

    // 获取数据
    inline const char* c_str() const noexcept { return data_; }
    inline const char* data() const noexcept { return data_; }
    inline size_type size() const noexcept { return size_; }
    inline size_type capacity() const noexcept { return capacity_; }
    inline bool empty() const noexcept { return size_ == 0; }
};

} // namespace simple_stl
#endif // SIMPLE_STL_CONTAINERS_STRING_H