#ifndef SAFEQUEUE_HPP
#define SAFEQUEUE_HPP

#include <queue>
#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <atomic>
#include <typeinfo>

#include <pthread.h>
#define MyMap_CLock_Mutex_t                 pthread_mutex_t
#define MyMap_CLock_Mutex_Init(_mutex)      (pthread_mutex_init(&_mutex, NULL))
#define MyMap_CLock_Mutex_Lock(_mutex)      (pthread_mutex_lock(&_mutex))
#define MyMap_CLock_Mutex_UnLock(_mutex)    (pthread_mutex_unlock(&_mutex))
#define MyMap_CLock_Mutex_Destroy(_mutex)   (pthread_mutex_destroy(&_mutex))

class concurrent_lock
{
public:
    concurrent_lock()
    {
        MyMap_CLock_Mutex_Init(cs_);
    }

    ~concurrent_lock()
    {
        MyMap_CLock_Mutex_Destroy(cs_);
    }

    void lock()
    {
        MyMap_CLock_Mutex_Lock(cs_);
    }

    void unlock()
    {
        MyMap_CLock_Mutex_UnLock(cs_);
    }

private:
    MyMap_CLock_Mutex_t cs_;
};


class scoped_lock
{
public:
    scoped_lock(const concurrent_lock& lock) : lock_(lock)
    {
        const_cast<concurrent_lock&>(lock_).lock();
    }

    ~scoped_lock()
    {
        const_cast<concurrent_lock&>(lock_).unlock();
    }

private:
    const concurrent_lock& lock_;
};

template <typename T>
class SafeQueue
{
public:
    SafeQueue()
    {
        NODE* node = new NODE();
        node->next = nullptr;

        head_ = node;
        tail_ = node;
    }

    ~SafeQueue()
    {
        NODE* node = head_;

        do
        {
            node = erase_(node);
        }
        while(node != nullptr);
    }

    void push(const T& val)
    {
        NODE* node = new NODE();
        node->val = val;
        node->next = nullptr;

        scoped_lock lock(h_lock_);
        ++size_;
        tail_->next = node;
        tail_ = node;
    }

    T pop()
    {
        scoped_lock lock(t_lock_);
        if(empty_())
        {
            /*
            if(typeid(T).name()[0] == 'S' ||typeid(T).name()[0] == 'P')
                return nullptr;
            else
            */
                //如果类型为是指针类型，且队列为空时，终止程序
                assert(false);
        }
        --size_;
        head_ = erase_(head_);
        return head_->val;
    }


    int size(){
        return size_;
    }

private:
    struct NODE
    {
        T val;
        NODE* next;
    };

private:
    bool empty_() const
    {
        return head_->next == nullptr;
    }

    NODE* erase_(NODE* node) const
    {
        NODE* tmp = node->next;
        delete node;
        return tmp;
    }

private:
    std::atomic_int size_=ATOMIC_VAR_INIT(0);

    NODE* head_;
    NODE* tail_;
    concurrent_lock h_lock_;
    concurrent_lock t_lock_;
};





#endif // SAFEQUEUE_HPP
