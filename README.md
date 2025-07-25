simple_stl_containers
 
项目简介
 
 simple_stl_containers  是一个基于C++11实现的STL容器模拟、包含线程池、定时器、单例模式等工具项目，旨在通过其实现，而深入理解C++容器、内存管理、模板编程、线程安全、基础设计模式、类的设计等基础原理。
 
项目结构
  
simple_stl_containers  
├── .vscode/          # VS Code 配置目录（项目与集成环境交互，测试配置等.json文件）  
├── build/            # 构建输出目录（编译生成中间文件）  
├── include/          # 头文件目录  
│   ├── common/       # 通用工具/基础头文件、memory等  
│   ├── containers/   # 容器相关头文件   
│   └── tools/        # 线程池、计时器、单例模式等
├── src/              # 源码目录  
│   ├── containers/    
│   └── tools/          
├── test/             # 测试目录  
│   ├── src/          # 测试用例源码目录  
│   ├── catch_amalgamated.cpp # Catch2 单文件集成（测试框架）别删  
│   ├── catch_amalgamated.hpp # Catch2 头文件  别删
│   ├── CMakeLists.txt        # 测试模块 CMake 配置  
│   └── test_main.cpp         # 测试入口  
├── CMakeLists.txt    # 项目根 CMake 配置  
└── README.md         # 项目说明文档  
 
 
编译与运行
 
环境要求
 
- 操作系统：Linux

- 编译器：GCC 或 Clang（C++17及以上标准，因为测试框架Catch的限制，编译器版本低会报错）

- 构建工具：CMake 3.10及以上版本
 
步骤
 
1. 克隆项目：
 
bash
  
git clone https://github.com/NoGrapeFromTheFox/simple_stl_containers.git

cd simple_stl_containers
 
 
2. 创建构建目录并配置CMake：
 
bash
  
mkdir build
cd build
cmake ..
 
 
3. 编译项目：
 
bash
  
make
 
 
4. 运行测试：(可执行文件最终在build/bin/目录下)
 
bash

 ./bin/test_shared_ptr_thread_safety
 
未来计划
 
1. 实现更多STL容器，如  map 、 unordered_map 、 list 等。

2. 实现更多设计模式工具类、加深对面向对象的理解。

3. 完成这个“学习项目”，迈出走向程序员道路的第一步

已完成模块总结

1、关于vector.h

（1）总结：内存分配器（连续内存） + vector 本身三个指针
（2）内存分配器Allocator: 主要接口rebind类型转换 + 对底层构建对象（placement new）、内存申请释放（::operator new）的二次封装
（3）vector 最核心的扩容接口reserve，实际是利用placement new和std::move 将旧地址上的对象移动到新申请的内存上，并在分配失败的情况下回滚到原始状态，如指针对象的保存、先申请新内存、再释放旧内存
（4）注意： 实现拷贝构造函数/拷贝赋值运算符的时候，要避免浅拷贝
placement new语法： ::new(内存地址p) T(参数agrs) 调用T的拷贝构造函数
