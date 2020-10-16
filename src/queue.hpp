#pragma once

#include "common.hpp"

namespace com
{
    template<typename T>
    class Queue
    {
        public:
            Queue() = default;
            Queue(const queue<T>&) = delete;
            virtual ~Queue() { clear(); }

            const T& front()
            {
                std::scoped_lock lock(mux_queue);
                return deq_queue.front();
            }

            const T& back()
            {
                std::scoped_lock lock(mux_queue);
                return deq_queue.back();
            }

            void push_back(const T& item)
            {
                std::scoped_lock lock(mux_queue);
                deq_queue.emplace_back(std::move(item));
            }

            void push_front(const T& item)
            {
                std::scoped_lock lock(mux_queue);
                deq_queue.emplace_front(std::move(item));
            }

            bool empty()
            {
                std::scoped_lock lock(mux_queue);
                return deq_queue.size();
            }

            size_t count()
            {
                std::scoped_lock lock(mux_queue);
                return deq_queue.size();
            }

            void clear()
            {
                std::scoped_lock lock(mux_queue);
                deq_queue.clear();
            }

            T pop_front()
            {
                std::scoped_lock lock(mux_queue);
                auto t = std::move(deq_queue.front());
                deq_queue.pop_front();
                return t;
            }

            T pop_back()
            {
                std::scoped_lock lock(mux_queue);
                auto t = std::move(deq_queue.back());
                deq_queue.pop_back();
                return t;
            }

        protected:
            std::mutex mux_queue;
            std::deque<T> deq_queue;
    };
}