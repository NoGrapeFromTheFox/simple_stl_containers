
#ifndef SIMPLE_STL_UTILITIES_H
#define SIMPLE_STL_UTILITIES_H

#include <cstddef> // for std::size_t
#include <type_traits>

 

namespace simple_stl {

/*    ********************** 内存操作工具 **********************     */

// uninitialized_move（未初始化内存移动）： 在已分配内存地址的情况下，使用移动构造将旧内存地址上的对象移动到新地址上
template <class InputIt, class ForwardIt>
ForwardIt uninitialized_move(InputIt first, InputIt last, ForwardIt d_first) {
    ForwardIt current = d_first;
    try {
        for (; first != last; ++first, ++current) {
            // 使用 placement new 直接在指定原始地址创建对象，不分配新内存。语法格式： new (内存地址) 类型(构造参数) ，
            ::new (static_cast<void*>(&*current)) // 获取迭代器解引用的元素的地址
                typename simple_stl::iterator_traits<ForwardIt>::value_type
                (std::move(*first)); 
        }
        return current;
    } catch (...) {
        // 异常安全：销毁已构造的元素
        while (current != d_first) {
            --current;
            // 调用析构函数
            using ValueType = typename simple_stl::iterator_traits<ForwardIt>::value_type;
            current->~ValueType();
        }
        throw;
    }
}
}



#endif // SIMPLE_STL_UTILITIES_H

/**
 * @note std::move() 将输入的对象转换为“右值引用”，移动后first指向的原对象，处于“有效但未定义状态”，
 * 后续可以重新赋新有效值或者销毁，但不能再继续使用内存中的旧值，因为move被认为“窃取了源对象的资源”
 * 右值引用： int&& reff = 0; 必须初始化，且等号左侧不能是左值；
 */