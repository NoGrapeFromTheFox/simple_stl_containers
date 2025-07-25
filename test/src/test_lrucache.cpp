#define CATCH_CONFIG_MAIN

#include "catch_amalgamated.hpp"
#include "tools/lrucache.h" 

TEST_CASE("LRUCache 基本功能测试", "[LRU]") {

    SECTION("基础读写与淘汰机制") {
        LRUCache cache(2);
        
        cache.put(1, 1);
        cache.put(2, 2);
        REQUIRE(cache.get(1) == 1); // 访问1，1移到头部
        
        cache.put(3, 3); // 容量超限，淘汰最久未用的2
        REQUIRE(cache.get(2) == -1); // 2已被淘汰
        
        cache.put(4, 4); // 容量超限，淘汰最久未用的1
        REQUIRE(cache.get(1) == -1);
        REQUIRE(cache.get(3) == 3); // 3存在
        REQUIRE(cache.get(4) == 4); // 4存在
    }


    SECTION("更新已有键值对") {
        LRUCache cache(2);
        
        cache.put(1, 10);
        cache.put(1, 20); 
        REQUIRE(cache.get(1) == 20); 
        
        cache.put(2, 2);
        REQUIRE(cache.get(1) == 20); 
    }

    SECTION("容量为1时的淘汰逻辑") {
        LRUCache cache(1);
        
        cache.put(1, 1);
        REQUIRE(cache.get(1) == 1);
        
        cache.put(2, 2); 
        REQUIRE(cache.get(1) == -1);
        REQUIRE(cache.get(2) == 2);
        
        cache.put(2, 3); 
        REQUIRE(cache.get(2) == 3);
    }

    SECTION("空缓存获取不存在的键") {
        LRUCache cache(3);
        REQUIRE(cache.get(99) == -1); 
    }

    SECTION("频繁访问同一节点不触发淘汰") {
        LRUCache cache(2);
        
        cache.put(1, 1);
        cache.put(2, 2);
        
        for (int i = 0; i < 5; ++i) {
            cache.get(1); 
        }
        
        cache.put(3, 3); 
        REQUIRE(cache.get(2) == -1);
        REQUIRE(cache.get(1) == 1);
        REQUIRE(cache.get(3) == 3);
    }
}