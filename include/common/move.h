#ifndef SIMPLE_STL_MOVE_H
#define SIMPLE_STL_MOVE_H

#include <cstddef>
#include <type_traits>


namespace simple_stl {

// ==================== 核心工具函数 ====================

/**
 * @brief 获取对象实际地址（即使重载了 operator&）
 * @ingroup utilities
 */
template<typename T>
inline constexpr T* addressof(T& r) noexcept {
    return simple_builtin_addressof(r);
}

// 禁止对右值取地址
template<typename T>
const T* addressof(const T&&) = delete;

/**
 * @brief 完美转发左值
 * @ingroup utilities
 */
template<typename T>
constexpr T&& forward(typename std::remove_reference<T>::type& t) noexcept {
    return static_cast<T&&>(t);
}

/**
 * @brief 完美转发右值
 * @ingroup utilities
 */
template<typename T>
constexpr T&& forward(typename std::remove_reference<T>::type&& t) noexcept {
    static_assert(!std::is_lvalue_reference<T>::value, 
                 "template argument substituting T must not be lvalue reference");
    return static_cast<T&&>(t);
}

/**
 * @brief 转换为右值引用（移动语义）
 * @ingroup utilities
 */
template<typename T>
constexpr typename std::remove_reference<T>::type&& move(T&& t) noexcept {
    return static_cast<typename std::remove_reference<T>::type&&>(t);
}

// ==================== 条件移动工具 ====================

// 定义 move_if_noexcept_cond（注意命名空间内定义）
template<typename _Tp>
struct move_if_noexcept_cond 
: public __and<
    __not<std::is_nothrow_move_constructible<_Tp>>, // 若在 __stl 中，is_nothrow_move_constructible 需正确引用
    is_copy_constructible<_Tp> 
>::type { };

// 定义 move_if_noexcept 函数（注意命名空间内定义）
template<typename T>
typename std::conditional<move_if_noexcept_cond<T>::value, const T&, T&&>::type
move_if_noexcept(T& x) noexcept {
    return static_cast<T&&>(x); // 显式使用 static_cast 实现 move 语义，避免依赖未定义的 move 函数
}

// ==================== 交换操作 ====================

/**
 * @brief 交换两个值（支持移动语义）
 * @ingroup utilities
 */
template<typename T>
inline typename std::enable_if<
    std::is_move_constructible<T>::value && 
    std::is_move_assignable<T>::value
>::type
swap(T& a, T& b) noexcept(
    std::is_nothrow_move_constructible<T>::value &&
    std::is_nothrow_move_assignable<T>::value
) {
    T tmp = move(a);
    a = move(b);
    b = move(tmp);
}

/**
 * @brief 交换两个数组
 * @ingroup utilities
 */
template<typename T, size_t N>
inline typename std::enable_if<std::is_swappable<T>::value>::type
swap(T (&a)[N], T (&b)[N]) noexcept(std::is_nothrow_swappable<T>::value) {
    for (size_t n = 0; n < N; ++n) {
        swap(a[n], b[n]);
    }
}

// ==================== 宏定义（兼容旧代码） ====================
#define SIMPLE_STL_MOVE(val) ::std::move(val)
#define SIMPLE_STL_FORWARD(T, val) ::std::forward<T>(val)

} // namespace simple_stl

#endif /* SIMPLE_STL_MOVE_H */