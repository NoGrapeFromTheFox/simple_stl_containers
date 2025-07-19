#ifndef SIMPLE_STL_ITERATOR_H
#define SIMPLE_STL_ITERATOR_H

#include <bits/c++config.h>
#include <cstddef> 
#include <type_traits>

namespace simple_stl {

// 迭代器标签定义
struct input_iterator_tag {};
struct forward_iterator_tag : input_iterator_tag {};
struct bidirectional_iterator_tag : forward_iterator_tag {};
struct random_access_iterator_tag : bidirectional_iterator_tag {};

// 基础模板（泛型迭代器）
template<typename _Iterator>
struct iterator_traits {
    typedef typename _Iterator::iterator_category iterator_category;
    typedef typename _Iterator::value_type        value_type;
    typedef typename _Iterator::difference_type   difference_type;
    typedef typename _Iterator::pointer           pointer;
    typedef typename _Iterator::reference         reference;
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