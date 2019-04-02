#pragma once

#ifndef RINGBUFFER_H
#define RINGBUFFER_H

template<typename T, size_t SIZE>
class RingBuffer
{
public:

    inline size_t capacity() const { return SIZE; };
    inline size_t size() const { return (tail_ - head_); };
    inline const T* data() const { return &(queue_[head_]); }
    inline T* data() { return &(queue_[head_]); }
    inline bool empty() const { return tail_ == head_; };
    inline void clear() { head_ = 0; tail_ = 0; };
    inline void pop()
    {
        if (size() == 0) return;
        if (size() == 1) clear();
        else head_++;
    };
    inline void pop_back()
    {
        if (size() == 0) return;
        if (size() == 1) clear();
        else tail_--;
    };
    inline void push(const T& data)
    {
        queue_[(tail_++) % SIZE] = data;
        if      (size() > SIZE) head_++;
    };
    inline void push(T&& data)
    {
        queue_[(tail_++) % SIZE] = data;
        if      (size() > SIZE) head_++;
    };
    inline void push_back(const T& data)
    {
        queue_[(tail_++) % SIZE] = data;
        if      (size() > SIZE) head_++;
    };
    inline void push_back(T&& data)
    {
        queue_[(tail_++) % SIZE] = data;
        if      (size() > SIZE) head_++;
    };

    inline const T& front() const // throw(Exception)
    {
        // if(empty()) throw Exception();
        return *(queue_ + head_ % SIZE);
    };
    inline T& front() // throw(Exception)
    {
        // if(empty()) throw Exception();
        return *(queue_ + head_ % SIZE);
    };

    inline const T& back() const // throw(Exception)
    {
        // if(empty()) throw Exception();
        return *(queue_ + (SIZE + tail_ - 1) % SIZE);
    }
    inline T& back() // throw(Exception)
    {
        // if(empty()) throw Exception();
        return *(queue_ + (SIZE + tail_ - 1) % SIZE);
    }

    inline const T& operator[] (uint8_t index) const
    {
        return *(queue_ + (head_ + index) % SIZE);
    }
    inline T& operator[] (uint8_t index)
    {
        return *(queue_ + (head_ + index) % SIZE);
    }

    inline const T* begin() const { return &(queue_[head_]); }
    inline T* begin() { return &(queue_[head_]); }
    inline const T* end() const { return &(queue_[tail_]); }
    inline T* end() { return &(queue_[tail_]); }

    inline T* erase(T* p)
    {
        if (p == end()) return p;
        for (T* pos = p + 1; pos != end(); ++pos)
            *(pos - 1) = *pos;
        --tail_;
        return p;
    }

    inline void resize(size_t sz)
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

    inline void assign(const T* const first, const T* const end)
    {
        clear();
        const char* p = first;
        while (p != end) push(*p++);
    }

private:

    T queue_[SIZE];
    volatile size_t head_ {0};
    volatile size_t tail_ {0};
};

#endif // RINGBUFFER_H
