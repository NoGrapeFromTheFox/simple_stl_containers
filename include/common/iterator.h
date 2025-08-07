#ifndef SIMPLE_STL_ITERATOR_H
#define SIMPLE_STL_ITERATOR_H
/**
 * @brief 迭代器类型萃取：不依赖具体迭代器的情况下，获取其内部关键类型信息
 * 如：迭代器类型、迭代器指向元素类型、迭代器之间的距离、指向元素指针类型、元素引用类型
 */

#include <bits/c++config.h>
#include <cstddef> 
#include <type_traits>

namespace simple_stl {

// 迭代器标签定义
struct input_iterator_tag {};                                      // 输入迭代器（仅支持读、单向遍历）
struct forward_iterator_tag : input_iterator_tag {};               // 前向迭代器 （支持读写、单向遍历）
struct bidirectional_iterator_tag : forward_iterator_tag {};       // 双向迭代器（读写）
struct random_access_iterator_tag : bidirectional_iterator_tag {}; // 随机访问迭代器（支持读写、双向遍历+随机访问）

// 基础模板（泛型迭代器）
template<typename _Iterator>
struct iterator_traits {
    typedef typename _Iterator::iterator_category iterator_category; // 迭代器类型
    typedef typename _Iterator::value_type        value_type;        // 迭代器指向元素类型
    typedef typename _Iterator::difference_type   difference_type;   // 迭代器之间的距离
    typedef typename _Iterator::pointer           pointer;           // 指向元素指针类型
    typedef typename _Iterator::reference         reference;         // 元素引用类型
};

// 部分特化：普通指针
template<typename _Tp>
struct iterator_traits<_Tp*> {
    typedef random_access_iterator_tag iterator_category;
    typedef _Tp                         value_type;
    typedef ptrdiff_t                   difference_type;
    typedef _Tp*                        pointer;
    typedef _Tp&                        reference;
};

// 部分特化：const 指针
template<typename _Tp>
struct iterator_traits<const _Tp*> {
    typedef random_access_iterator_tag iterator_category;
    typedef _Tp                         value_type;
    typedef ptrdiff_t                   difference_type;
    typedef const _Tp*                  pointer;
    typedef const _Tp&                  reference;
};

} // namespace simple_stl

#endif // SIMPLE_STL_ITERATOR_H