#pragma once
#include <memory>
#include <list>
#include <queue>
#include <thread>
#include <atomic>
#include <iostream>
#include<utility>
#include<iterator>
template <typename T>
class ruanode
{
public:
    typedef std::shared_ptr<T> valuePoint;
    ruanode* next;
    ruanode* pre;
    valuePoint value;
    ruanode(const T& data)
    {
        value = std::make_shared<T>(move(data));
        next = nullptr;
        pre = nullptr;
    }
    ruanode(const T&& data)
    {
        value = std::make_shared<T>(move(data));
        next = nullptr;
        pre = nullptr;
    }
    ruanode() 
    {
        next = nullptr;
        pre = nullptr;
    }
    ~ruanode()
    {
    }
};
template <typename T>
class _ruanode_iterator : public std::iterator<std::forward_iterator_tag, T>
{
protected:
    typedef T* pointer;
    typedef T value;
    typedef T& reference;
    typedef ruanode<T> node;
    node* _ListPointer;

public:
    _ruanode_iterator(node* p = nullptr) : _ListPointer(p) {}
    bool operator!=(_ruanode_iterator a) const
    {
        return a._ListPointer != this->_ListPointer;
    }
    bool operator==(_ruanode_iterator a) const
    {
        return a._ListPointer == this->_ListPointer;
    }
    reference operator*() const
    {
        return *(_ListPointer->value);
    }
    typename std::shared_ptr<value>& operator->() const
    {
        return _ListPointer->value;
    }
    _ruanode_iterator& operator++()
    {
        _ListPointer = _ListPointer->next;
        return *this;
    }
    _ruanode_iterator operator++(int)
    {
        _ruanode_iterator tem(*this);
        _ListPointer = _ListPointer->next;
        return tem;
    }
};
template <typename T, typename allo = std::allocator<ruanode<T>>>
class ruaque
{

public:
    typedef _ruanode_iterator<T> iterator;
    typedef T* pointer;
    typedef T value;
    typedef T& reference;
    typedef ruanode<T> node;
    ruaque()
    {
        count_size = 0;
        _end = queallo.allocate(1);
        queallo.construct(_end);
        _end->next = _end;
        _end->pre = _end;
    }
    ~ruaque()
    {
        node* tem1 = _end;
        node* tem2=nullptr;
        while (tem2 != _end)
        {
            tem2 = tem1->next;
            queallo.destroy(tem1);
            queallo.deallocate(tem1, 1);
            tem1 = tem2;
        }
    }
    bool empty()
    {
        return _first() == _end;
    }
    void push_back(const value& data)
    {
        node* newnode = queallo.allocate(1);
        queallo.construct(newnode, data);
        newnode->next = _end;
        newnode->pre = _end->pre;
        _end->pre->next = newnode;
        _end->pre = newnode;
        ++count_size;
    }
    void push_back(value&& data)
    {
        node* newnode = queallo.allocate(1);
        queallo.construct(newnode, std::forward<T>(data));
        newnode->next = _end;
        newnode->pre = _end->pre;
        _end->pre->next = newnode;
        _end->pre = newnode;
        ++count_size;
    }
    void push_top(const value& data)
    {
        node* newnode = queallo.allocate(1);
        queallo.construct(newnode, data);
        newnode->next = _end->next;
        newnode->pre = _end;
        _end->next->pre = newnode;
        _end->next = newnode;
        ++count_size;
    }
    void push_top(value&& data)
    {
        node* newnode = queallo.allocate(1);
        queallo.construct(newnode, data);
        newnode->next = _end->next;
        newnode->pre = _end;
        _end->next->pre = newnode;
        _end->next = newnode;
        ++count_size;
    }
    void pop_back()
    {
        node* delnode = _end->pre;
        node* lastnode = delnode->pre;
        lastnode->next = _end;
        _end->pre = lastnode;
        queallo.destroy(delnode);
        queallo.deallocate(delnode, 1);
        --count_size;
    }
    void pop_front()
    {
        node* delnode = _end->next;
        node* firnode = delnode->next;
        firnode->pre = _end;
        _end->next = firnode;
        queallo.destroy(delnode);
        queallo.deallocate(delnode, 1);
    }
    reference front()
    {
        return *(_end->next->value);
    }
    reference top()
    {
        return *(_end->pre->value);
    }
    iterator begin()
    {
        return iterator(_end->next);
    }
    iterator end()
    {
        return iterator(_end);
    }
    ruaque(ruaque&) = delete;
    ruaque(ruaque&&) = default;

protected:
    node* _first()
    {
        return _end->next;
    }
    allo queallo;
    node* _end;
    size_t count_size;
};