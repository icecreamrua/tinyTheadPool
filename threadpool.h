#pragma once
#include <utility>
#include <functional>
#include <thread>
#include "ruaque.h"
#include<iostream>
#include<future>
#include<mutex>
#include<memory>
#include<condition_variable>
template<class T>
class threadpool_queue
{
protected:
	typedef T* pointer;
	typedef T value;
	typedef T& reference;
	ruaque<T> mydeque;
public:
	value front_pop()
	{
		auto tem = mydeque.front();
		mydeque.pop_front();
		return tem;
	}
	void push(value&& data)
	{
		mydeque.push_back(move(data));
	}
	void push(const value& data)
	{
		mydeque.push_back(move(data));
	}
	bool empty()
	{
		return mydeque.empty();
	}
};
class threadpool
{
protected:
	using task = std::function<void()>;
	threadpool_queue<task> tackque;
	std::vector<std::thread> pool;
	std::condition_variable is_Empty;
	int is_shutdown;
	enum shutdownState
	{
		graceful=1,
		immediate=2,
	};
	std::mutex mtx;
	int max_thread_num;
	void workLoop()
	{
		for(;;)
		{
			std::unique_lock<std::mutex> lock(this->mtx);
			while (this->tackque.empty()&&!is_shutdown)
			{
				this->is_Empty.wait(lock);
			}
			if(is_shutdown==immediate||(is_shutdown==graceful&&this->tackque.empty()))
			{
				break;
			}
			auto onetask = this->tackque.front_pop();
			onetask();
		}
		return;
	}
public:
	threadpool():max_thread_num(0),is_shutdown(1)
	{

	}
	threadpool(threadpool&) = delete;
	threadpool(threadpool&&) = delete;
	~threadpool()
	{
		shutdown();
	}
	void shutdown(bool is_graceful=true)
	{
		if(is_shutdown)
		{
			return;
		}
		is_shutdown=is_graceful?graceful:immediate;
		is_Empty.notify_all();
		for(auto& onethread:pool)
		{
			onethread.join();
		}
		return;
	}
	void initialization(int thread_num=5)
	{
		max_thread_num=thread_num;
		is_shutdown = 0;
		for (int i = 0; i < max_thread_num; i++)
		{
			auto temfunc = std::bind(&threadpool::workLoop,this);
			pool.emplace_back(std::thread(temfunc));
		}
		return;
	}
	template <typename F, typename... Args>
	auto pushTask(F f, Args&&... args)
	{
		using retType=typename std::result_of<F (Args...)>::type;
		auto taskpoint = std::make_shared<std::packaged_task<retType()>>
			([f, &args...]() {return f(std::forward<Args>(args)...); });
			//(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
		auto ret = taskpoint->get_future();
		std::unique_lock<std::mutex> lock(mtx);
		tackque.push([taskpoint]() 
			{
			(*taskpoint)(); 
			});
		is_Empty.notify_one();
		return ret;
	}
};