#pragma once

#include <atomic>
#include <utility>
#include "non_copyable.h"

namespace hlp {

// Multi producer single consumer queue
template <typename T>
class MpscQueue : public NonCopyable {
 public:
  MpscQueue()
          : head_{ new BufferNode },
            tail_{ head_.load(std::memory_order_relaxed) } {
  }
  ~MpscQueue() {
    T output{};
    while (dequeue(output)) {
    }

    BufferNode* front{ head_.load(std::memory_order_relaxed) };
    delete front;
  }

  void enqueue(const T& input) {
    BufferNode* node{ new BufferNode{ input } };
    BufferNode* old_head{ head_.exchange(node, std::memory_order_acq_rel) };
    old_head->next_.store(node, std::memory_order_release);
  }

  void enqueue(T&& input) {
    BufferNode* node{ new BufferNode({ std::move(input) }) };
    BufferNode* old_head{ head_.exchange(node, std::memory_order_acq_rel) };
    old_head->next_.store(node, std::memory_order_release);
  }

  bool dequeue(T& output) {
    BufferNode* tail{ tail_.load(std::memory_order_relaxed) };
    BufferNode* next{ tail->next_.load(std::memory_order_acquire) };

    if (!next)
      return false;

    output = std::move(*next->data_ptr_);
    delete next->data_ptr_;
    tail_.store(next, std::memory_order_release);
    delete tail;

    return true;
  }

  bool empty() const {
    BufferNode* tail{ tail_.load(std::memory_order_relaxed) };
    BufferNode* next{ tail->next_.load(std::memory_order_acquire) };

    return !next;
  }

 private:
  class BufferNode {
   public:
    BufferNode() = default;
    explicit BufferNode(const T& data) : data_ptr_(new T(data)) {
    }
    explicit BufferNode(T&& data) : data_ptr_(new T(std::move(data))) {
    }

    T* data_ptr_{ nullptr };
    std::atomic<BufferNode*> next_{ nullptr };
  };
  std::atomic<BufferNode*> head_{ nullptr };
  std::atomic<BufferNode*> tail_{ nullptr };
};
}  // namespace hlp
