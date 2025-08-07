#ifndef SIMPLE_STL_CONTAINERS_LIST_H
#define SIMPLE_STL_CONTAINERS_LIST_H

#include <cstddef> // ptrdiff_t 用于表示两个指针之间的差值
#include "common/iterator.h"
#include "common/allocator.h"

namespace simple_stl {

template <typename T, typename Allocator> // 前置声明中不要重复指定默认值
class List;

template <typename T>
class ListIterator
{
public:
    using iterator_category = simple_stl::bidirectional_iterator_tag; // 双向迭代器
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T*;
    using reference = T&;

    ListIterator(typename simple_stl::List<T,simple_stl::allocator<T>>::ListNode* node = nullptr):current_(node) {}

    // 解引用，自增，自减
    reference operator*() const { return current_->data; }
    pointer operator->() const { return &(current_->data); }

    ListIterator& operator++() { // 前置++
        current_ = current_->next;
        return *this;
    }
    ListIterator operator++(int){ // int不实际使用，仅用于区分前置、后置
        ListIterator temp = *this;
        current_ = current_->next;
        return temp;
    }

    ListIterator& operator--() {
        current_ = current_->prev;
        return *this;
    }

    ListIterator operator--(int) {
        ListIterator temp = *this;
        current_ = current_->prev;
        return temp;
    }

    // 相等/不等比较
    bool operator==(const ListIterator& other) const {
        return current_ == other.current_;
    }

    bool operator!=(const ListIterator& other) const {
        return !(*this == other);
    }
private:
    typename simple_stl::List<T, simple_stl::allocator<T>>::ListNode* current_;
};

template <typename T, typename Allocator = simple_stl::allocator<T>>
class List 
{
private:
    struct  ListNode; // 前置声明
    
public:
    using size_type = std::size_t;
    using iterator = ListIterator<T>;
    using const_iterator = ListIterator<T>;  // 为简化逻辑，这里省略了常量迭代器的实现，先调用普通迭代器
    using NodeAllocator = typename Allocator::template rebind<ListNode>::other;

    // 构造函数

    // 默认构造
    List(const Allocator& alloc = Allocator()) : node_alloc_(alloc) {
        initDummy();
    }

    // 用迭代[b,e)区间内的元素构造
    template <typename InputIterator>
    List(InputIterator b, InputIterator e,const Allocator& alloc = Allocator()):node_alloc_(alloc){ 
        initDummy();
        while (b != e)
        {
            push_back(*b);
            ++b; // 前自增无需额外复制迭代器，循环效率更高；只有“需要先用当前值，再增加”时使用后自增
        }
    }

    // 初始化有n个值为 elem的节点的链表
    List(int n, const T& elem, const Allocator& alloc = Allocator()):node_alloc_(alloc) {
        initDummy();
        for (int i = 0; i < n; ++i) {
            push_back(elem); 
        }
    }

    // 拷贝构造 无需冗余夹带模版参数
    List(const List& other):node_alloc_(other.node_alloc_){
        initDummy();
        auto it = other.begin();
        auto end = other.end();
        while(it != end){
            push_back(*it);
            ++it;
        }
    }

    // 析构函数
    ~List() {
        
        ListNode* curr = head_->next; 
        while (curr != head_) {      
            ListNode* next = curr->next;  
            destroyNode(curr);            
            curr = next;                  
        }

        destroyNode(head_);

        head_ = nullptr;
        m_size_ = 0;
    }

    // 插入、删除操作
    void push_back(const T& val) {
       
        ListNode* newNode = createNode(val);

        // 默认：链表不为空的时候，哨兵头结点的前指针指向尾节点；链表为空时，头结点的prev和next都指向自己
        ListNode* last = head_->prev;
        newNode->prev = last;
        newNode->next = head_;
        last->next = newNode;
        head_->prev = newNode;
        m_size_++;
    }

    // begin、end迭代器
    iterator begin() noexcept {
        return iterator(head_->next);
    }
    iterator end() noexcept {
        return iterator(head_); 
    }

    const_iterator begin() const noexcept {
        return const_iterator(head_->next);
    }
    const_iterator end() const noexcept {
        return const_iterator(head_);
    }
    

private:

    template <typename U>
    friend class ListIterator;

    struct ListNode{
        T data;
        ListNode* prev;
        ListNode* next;
        ListNode(const T& val):data(val),prev(nullptr),next(nullptr) {}
        ListNode(const T& val, ListNode* p, ListNode* n) : data(val), prev(p), next(n) {}
    };

    ListNode* head_;
    size_type m_size_;
    NodeAllocator node_alloc_; 

    void initDummy(){
        ListNode* dummy = node_alloc_.allocate(1);
        try
        {
            // 参数含义 内存地址、(构造ListNode用的)data,prev,next
            node_alloc_.construct(dummy, T(), dummy, dummy);
        }
        catch(...)
        {
            node_alloc_.deallocate(dummy, 1);
            throw;
        }
        head_ = dummy;
        m_size_ = 0;
    }

    ListNode* createNode(const T& val){
        ListNode* node = node_alloc_.allocate(1);
        try
        {
            node_alloc_.construct(node, val, nullptr, nullptr);
        }
        catch(...)
        {
            node_alloc_.deallocate(node, 1);
            throw;
        }
        return node;
    }

    void destroyNode(ListNode* p){
        if(p == nullptr) return;
        node_alloc_.destroy(p); // 先析构对象
        node_alloc_.deallocate(p, 1);
    }
};

} // namespace simple_stl
#endif // SIMPLE_STL_CONTAINERS_LIST_H