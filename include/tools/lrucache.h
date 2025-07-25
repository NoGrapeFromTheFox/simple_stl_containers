#ifndef LRUCACHE_H
#define LRUCACHE_H

#include <memory>
#include <unordered_map>

class LRUCache
{
private:
    // 链表节点
    struct ListNode
    {
        int val;
        int key;
        std::weak_ptr<ListNode> prev; 
        std::shared_ptr<ListNode> next;

        ListNode(int v, int k):val(v), key(k),prev(),next(nullptr) {}
    };
    // 链表头尾哑节点
    std::shared_ptr<ListNode> head_;
    std::shared_ptr<ListNode> tail_;
    // 哈希表
    std::unordered_map<int, std::shared_ptr<ListNode>> cacheMap_;
    int capacity_; // 最近最少使用缓存的容量

    void moveToHead(std::shared_ptr<ListNode> node);
    std::shared_ptr<ListNode> removeTail();
    
public:
    LRUCache(int cap):capacity_(cap){
        head_ = std::make_shared<ListNode>(-1, -1);
        tail_ = std::make_shared<ListNode>(-1, -1);
        head_->next = tail_;
        tail_->prev = head_;
    };

    LRUCache(const LRUCache&) = delete;
    LRUCache& operator=(const LRUCache&) = delete;
    LRUCache(LRUCache&&) = default;
    LRUCache& operator=(LRUCache&&) = default;

    int get(int key);
    void put(int key, int value);
};

#endif // LRUCACHE_H

/**
 * @note share_ptr<TypeName> 模版语法：传入的参数应该是类型名，返回一个 *TypeName 指针
 * @note 删除移动/拷贝语义,避免出现浅拷贝而导致两个LRUCache共享同一套节点
 * 如果想将它“过度设计成类似stl容器”，则重写拷贝构造函数的时候，需要注意“深拷贝”，建立独立的链表结构和哈希表映射
 */