#pragma once

#ifndef ARX_RINGBUFFER_H
#define ARX_RINGBUFFER_H

#if __cplusplus < 201103L
    #error "C++11 must be enabled in the compiler for this library to work, please check your compiler flags"
#endif

#include "ArxContainer/has_include.h"
#include "ArxContainer/has_libstdcplusplus.h"

#ifdef ARDUINO
    #include <Arduino.h>
#endif

#include "ArxContainer/replace_minmax_macros.h"
#include "ArxContainer/initializer_list.h"

#if ARX_HAVE_LIBSTDCPLUSPLUS >= 201103L // Have libstdc++11

#include <vector>
#include <deque>
#include <map>

#else // Do not have libstdc++11


#ifndef ARX_VECTOR_DEFAULT_SIZE
#define ARX_VECTOR_DEFAULT_SIZE 16
#endif // ARX_VECTOR_DEFAULT_SIZE

#ifndef ARX_DEQUE_DEFAULT_SIZE
#define ARX_DEQUE_DEFAULT_SIZE 16
#endif // ARX_DEQUE_DEFAULT_SIZE

#ifndef ARX_MAP_DEFAULT_SIZE
#define ARX_MAP_DEFAULT_SIZE 16
#endif // ARX_MAP_DEFAULT_SIZE


namespace arx {

    namespace container
    {
        namespace detail
        {
            template<class T>
            T&& move(T& t){ return static_cast<T&&>(t); }
        }
    }

    template<typename T, size_t N>
    class RingBuffer
    {
    protected:

        T queue_[N];
        int head_ {0};
        int tail_ {0};

    public:

        RingBuffer()
        : queue_()
        , head_ {0}
        , tail_{0}
        {
        }

        RingBuffer(std::initializer_list<T> lst)
        : queue_()
        , head_(0)
        , tail_(0)
        {
            for (auto it = lst.begin() ; it != lst.end() ; ++it)
            {
                queue_[tail_] = *it;
                ++tail_;
            }
        }

        // copy
        explicit RingBuffer(const RingBuffer& r)
        : queue_()
        , head_(r.head_)
        , tail_(r.tail_)
        {
            for (size_t i = 0; i < r.size(); ++i)
                queue_[i] = r.queue_[i];
        }
        RingBuffer& operator= (const RingBuffer& r)
        {
            head_ = r.head_;
            tail_ = r.tail_;
            for (size_t i = 0; i < r.size(); ++i)
                queue_[i] = r.queue_[i];
            return *this;
        }

        // move
        RingBuffer(RingBuffer&& r)
        {
            head_ = container::detail::move(r.head_);
            tail_ = container::detail::move(r.tail_);
            for (size_t i = 0; i < r.size(); ++i)
                queue_[i] = container::detail::move(r.queue_[i]);
        }

        RingBuffer& operator= (RingBuffer&& r)
        {
            head_ = container::detail::move(r.head_);
            tail_ = container::detail::move(r.tail_);
            for (size_t i = 0; i < r.size(); ++i)
                queue_[i] = container::detail::move(r.queue_[i]);
            return *this;
        }

        virtual ~RingBuffer() {}

        using iterator = T*;
        using const_iterator = const T*;

        size_t capacity() const { return N; };
        size_t size() const { return tail_ - head_; }
        inline const T* data() const { return &(get(head_)); }
        T* data() { return &(get(head_)); }
        bool empty() const { return tail_ == head_; }
        void clear() { head_ = 0; tail_ = 0; }

        void pop() { pop_front(); }
        void pop_front()
        {
            if (size() == 0) return;
            if (size() == 1) clear();
            else head_++;
        }
        void pop_back()
        {
            if (size() == 0) return;
            if (size() == 1) clear();
            else tail_--;
        }

        void push(const T& data) { push_back(data); }
        void push(T&& data) { push_back(data); }
        void push_back(const T& data)
        {
            get(tail_++) = data;
            if (size() > N) ++head_;
        };
        void push_back(T&& data)
        {
            get(tail_++) = data;
            if (size() > N) ++head_;
        };
        void push_front(const T& data)
        {
            get(head_--) = data;
            if (size() > N) --tail_;
        };
        void push_front(T&& data)
        {
            get(head_--) = data;
            if (size() > N) --tail_;
        };
        void emplace(const T& data) { push(data); }
        void emplace(T&& data) { push(data); }
        void emplace_back(const T& data) { push_back(data); }
        void emplace_back(T&& data) { push_back(data); }

        const T& front() const { return get(head_); };
        T& front() { return get(head_); };

        const T& back() const { return get(tail_ - 1); }
        T& back() { return get(tail_ - 1); }

        const T& operator[] (size_t index) const { return get(head_ + (int)index); }
        T& operator[] (size_t index) { return get(head_ + (int)index); }

        iterator begin() { return ptr(head_); }
        iterator end() { return (queue_ + tail_); }

        const_iterator begin() const { return (const_iterator)ptr(head_); }
        const_iterator end() const { return (const_iterator)(queue_ + tail_); }

        iterator erase(iterator p)
        {
            if (p == end()) return p;
            for (T* pos = p + 1; pos != end(); ++pos)
                *(pos - 1) = *pos;
            --tail_;
            return p;
        }

        void resize(size_t sz)
        {
            size_t s = size();
            if (sz > size())
            {
                for (size_t i = 0; i < sz - s; ++i) push(T());
            }
            else if (sz < size())
            {
                for (size_t i = 0; i < s - sz; ++i) pop();
            }
        }

        void assign(const_iterator first, const_iterator end)
        {
            clear();
            const char* p = first;
            while (p != end) push(*p++);
        }

        void shrink_to_fit()
        {
            // dummy
        }

        void reserve(size_t n)
        {
            (void)n;
            // dummy
        }

        void insert(const_iterator pos, const_iterator first, const_iterator last)
        {
            if (pos != end())
            {
                size_t sz = 0;
                {
                    iterator it = (iterator)first;
                    for (; it != last; ++it) ++sz;
                }
                iterator it = end() + sz - 1;
                for (int i = sz; i > 0; --i, --it)
                {
                    *it = *(it - sz);
                }
                it = (iterator)pos;
                for (size_t i = 0; i < sz; ++i)
                {
                    *it = *(first + i);
                }
            }
            else
            {
                iterator it = (iterator)first;
                for (; it != last; ++it)
                {
                    push_back(*it);
                }
            }
        }

    private:

        T& get(const int i)
        {
            if (i >= 0) return queue_[i % N];
            else        return queue_[N - abs(i % N)];
        }

        const T& get(const int i) const
        {
            if (i >= 0) return queue_[i % N];
            else        return queue_[N - abs(i % N)];
        }

        T* ptr(const int i)
        {
            if (i >= 0) return (T*)(queue_ + i % N);
            else        return (T*)(queue_ + N - abs(i % N));
        }

        const T* ptr(const int i) const
        {
            if (i >= 0) return (T*)(queue_ + i % N);
            else        return (T*)(queue_ + N - abs(i % N));
        }

    };

    template <typename T, size_t N>
    bool operator== (const RingBuffer<T, N>& x, const RingBuffer<T, N>& y)
    {
        if (x.size() != y.size()) return false;
        for (size_t i = 0; i < x.size(); ++i)
            if (x[i] != y[i]) return false;
        return true;
    }

    template <typename T, size_t N>
    bool operator!= (const RingBuffer<T, N>& x, const RingBuffer<T, N>& y)
    {
        return !(x == y);
    }


    template <typename T, size_t N = ARX_VECTOR_DEFAULT_SIZE>
    struct vector : public RingBuffer<T, N>
    {
        vector() : RingBuffer<T, N>() { }
        vector(std::initializer_list<T> lst) : RingBuffer<T, N>(lst) { }

        // copy
        vector(const vector& r) : RingBuffer<T, N>(r) { }

        vector& operator= (const vector& r)
        {
            RingBuffer<T, N>::operator=(r);
            return *this;
        }

        // move
        vector(vector&& r) : RingBuffer<T, N>(r) { }

        vector& operator= (vector&& r)
        {
            RingBuffer<T, N>::operator=(r);
            return *this;
        }

        virtual ~vector() {}

    private:
        using RingBuffer<T, N>::pop;
        using RingBuffer<T, N>::pop_front;
        using RingBuffer<T, N>::push;
        using RingBuffer<T, N>::push_front;
        using RingBuffer<T, N>::emplace;
    };


    template <typename T, size_t N = ARX_DEQUE_DEFAULT_SIZE>
    struct deque : public RingBuffer<T, N>
    {
        deque() : RingBuffer<T, N>() { }
        deque(std::initializer_list<T> lst) : RingBuffer<T, N>(lst) { }

        // copy
        deque(const deque& r) : RingBuffer<T, N>(r) { }

        deque& operator= (const deque& r)
        {
            RingBuffer<T, N>::operator=(r);
            return *this;
        }

        // move
        deque(deque&& r) : RingBuffer<T, N>(r) { }

        deque& operator= (deque&& r)
        {
            RingBuffer<T, N>::operator=(r);
            return *this;
        }

        virtual ~deque() {}

    private:
        using RingBuffer<T, N>::capacity;
        using RingBuffer<T, N>::pop;
        using RingBuffer<T, N>::push;
        using RingBuffer<T, N>::emplace;
        using RingBuffer<T, N>::assign;
        using RingBuffer<T, N>::begin;
        using RingBuffer<T, N>::end;
    };


    template <class T1, class T2>
    struct pair
    {
        T1 first;
        T2 second;
    };

    template <class T1, class T2>
    pair<T1, T2> make_pair(const T1& t1, const T2& t2)
    {
        return {t1, t2};
    };

    template <typename T1, typename T2>
    bool operator== (const pair<T1, T2>& x, const pair<T1, T2>& y)
    {
        return (x.first == y.first) && (x.second == y.second);
    }

    template <typename T1, typename T2>
    bool operator!= (const pair<T1, T2>& x, const pair<T1, T2>& y)
    {
        return !(x == y);
    }


    template <class Key, class T, size_t N = ARX_MAP_DEFAULT_SIZE>
    struct map : public RingBuffer<pair<Key, T>, N>
    {
        using iterator = typename RingBuffer<pair<Key, T>, N>::iterator;
        using const_iterator = typename RingBuffer<pair<Key, T>, N>::const_iterator;

        map() : RingBuffer<pair<Key, T>, N>() { }
        map(std::initializer_list<pair<Key, T>> lst) : RingBuffer<pair<Key, T>, N>(lst) { }

        // copy
        map(const map& r) : RingBuffer<pair<Key, T>, N>(r) { }

        map& operator= (const map& r)
        {
            RingBuffer<pair<Key, T>, N>::operator=(r);
            return *this;
        }

        // move
        map(map&& r) : RingBuffer<T, N>(r) { }

        map& operator= (map&& r)
        {
            RingBuffer<pair<Key, T>, N>::operator=(r);
            return *this;
        }

        virtual ~map() {}

        const_iterator find(const Key& key) const
        {
            for (size_t i = 0; i < this->size(); ++i)
            {
                const_iterator it = this->begin() + i;
                if (key == it->first)
                    return it;
            }
            return this->end();
        }

        iterator find(const Key& key)
        {
            for (size_t i = 0; i < this->size(); ++i)
            {
                iterator it = this->begin() + i;
                if (key == it->first)
                    return it;
            }
            return this->end();
        }

        pair<iterator, bool> insert(const Key& key, const T& t)
        {
            bool b {false};
            iterator it = find(key);
            if (it == this->end())
            {
                this->push(make_pair(key, t));
                b = true;
                it = this->end() - 1;
            }
            return {it, b};
        }

        pair<iterator, bool> insert(const pair<Key, T>& p)
        {
            bool b {false};
            const_iterator it = find(p.first);
            if (it == this->end())
            {
                this->push(p);
                b = true;
                it = this->end() - 1;
            }
            return {(iterator)it, b};
        }

        pair<iterator, bool> emplace(const Key& key, const T& t)
        {
            return insert(key, t);
        }

        pair<iterator, bool> emplace(const pair<Key, T>& p)
        {
            return insert(p);
        }

        const T& at(const Key& key) const
        {
            // iterator it = find(key);
            // if (it != this->end()) return it->second;
            // return T();
            return find(key)->second;
        }

        T& at(const Key& key)
        {
            // iterator it = find(key);
            // if (it != this->end()) return it->second;
            // return T();
            return find(key)->second;
        }

        iterator erase(const iterator it)
        {
            iterator i = (iterator)find(it->first);
            if (i != this->end())
            {
                return this->erase(i);
            }
            return this->end();
        }

        iterator erase(const size_t index)
        {
            if (index < this->size())
            {
                iterator it = this->begin() + index;
                return this->erase(it);
            }
            return this->end();
        }

        T& operator[] (const Key& key)
        {
            iterator it = find(key);
            if (it != this->end()) return it->second;

            insert(::arx::make_pair(key, T()));
            return this->back().second;
        }

    private:

        using RingBuffer<pair<Key, T>, N>::capacity;
        using RingBuffer<pair<Key, T>, N>::data;
        using RingBuffer<pair<Key, T>, N>::pop;
        using RingBuffer<pair<Key, T>, N>::pop_front;
        using RingBuffer<pair<Key, T>, N>::pop_back;
        using RingBuffer<pair<Key, T>, N>::push;
        using RingBuffer<pair<Key, T>, N>::push_back;
        using RingBuffer<pair<Key, T>, N>::push_front;
        using RingBuffer<pair<Key, T>, N>::emplace_back;
        using RingBuffer<pair<Key, T>, N>::front;
        using RingBuffer<pair<Key, T>, N>::back;
        using RingBuffer<pair<Key, T>, N>::resize;
        using RingBuffer<pair<Key, T>, N>::assign;
        using RingBuffer<pair<Key, T>, N>::shrink_to_fit;
    };

} // namespace arx

template<typename T, size_t N>
using ArxRingBuffer = arx::RingBuffer<T, N>;


#endif // Do not have libstdc++11
#endif // ARX_RINGBUFFER_H
