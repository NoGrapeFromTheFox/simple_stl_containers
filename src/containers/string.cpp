#include "containers/string.h"

// 重新分配内存
void simple_stl::string::realloc_data(size_type new_cap){
    new_cap = std::max(new_cap, str_min_capacity);
    char* new_data = new char[new_cap + 1];
    if(size_ > 0){
        std::memcpy(new_data, data_, size_);
    }
    new_data[size_] = '\0';
    delete[] data_;
    data_ = new_data;
    capacity_ = new_cap;
} 
void simple_stl::string::reserve(size_type new_cap){
    if(new_cap > capacity_){
        realloc_data(new_cap);
    }
}

void simple_stl::string::shrink_to_fit(){
    if(capacity_ > size_){
        realloc_data(size_);
    }
}

simple_stl::string& simple_stl::string::append(const char* str, size_type len){
    if(!str) throw std::invalid_argument("null pointer");
    if( size_ + len > capacity_ ){
        reserve((size_ + len) * 2);
    }
    std::memcpy(data_ + size_ , str, len);
    size_ += len;
    data_[size_] = '\0';
    return *this;
}
simple_stl::string& simple_stl::string::append(const char* str){
    if(!str) throw std::invalid_argument("null pointer");
    return append(str, std::strlen(str));
}