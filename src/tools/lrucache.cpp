#include "tools/lrucache.h"

// 将最新使用节点插入链表头部
void LRUCache::moveToHead(std::shared_ptr<LRUCache::ListNode> node){

    if(node == head_ || node == tail_ || node == head_->next) return;
    // 将节点node拆下链表
    auto nodePrev = node->prev.lock();
    auto nodeNext = node->next;
    if(nodePrev && nodeNext){
        nodePrev->next = nodeNext;
        nodeNext->prev = nodePrev;
    }
    auto headNext = head_->next;
    head_->next = node;
    node->next = headNext;
    node->prev = head_;
    headNext->prev = node;
}

// 移除链表尾部节点
std::shared_ptr<LRUCache::ListNode> LRUCache::removeTail(){
    auto tailPrev = tail_->prev.lock();
    if(tailPrev == head_) return nullptr;

    auto tailPrevPrev = tailPrev->prev.lock();
    tailPrevPrev->next = tail_;
    tail_->prev = tailPrevPrev;
    return tailPrev;
}

// 如果关键字 key 存在于缓存中，则返回关键字的值，否则返回 -1
int LRUCache::get(int key){
    if(cacheMap_.find(key) != cacheMap_.end()){
        auto node = cacheMap_[key];
        moveToHead(node);
        return node->val;
    }
    return -1;
}

// 如果关键字 key 已经存在，则变更其数据值 value ；
// 如果不存在，则向缓存中插入该组 key-value 。
// 如果插入操作导致关键字数量超过 capacity ，则应该 逐出 最久未使用的关键字。
void LRUCache::put(int key, int value){
    if(cacheMap_.find(key) != cacheMap_.end()){
        auto node = cacheMap_[key];
        moveToHead(node);
        node->val = value;
        return;
    }

    if(cacheMap_.size() >= capacity_){
        auto temp = removeTail();
        if(temp){
            cacheMap_.erase(temp->key);
        }
    }
    
    auto new_node = std::make_shared<ListNode>(value, key);
    moveToHead(new_node);
    cacheMap_.emplace(key, new_node);
}