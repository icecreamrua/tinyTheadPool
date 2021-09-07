# tinyTheadPool
一个简单、轻便的线程池实现，支持返回函数执行结果，限制最大线程数量及直接传入参数引用。
内部参考STL实现了一个基于链表的deque，以及通过适配器模式实现了用于线程池的taskqueue。
使用用例：
``` cpp
// 包含必要的头文件
#include "threadpool.h"
//创建threadpool
threadpool pool;
//初始化
pool.initialization();
//放入任务：函数及函数参数，可通过future获取函数执行结果
pool.pushTask(func,argv...);
//auto ret=pool.pushTask(func,argv...);
``` 