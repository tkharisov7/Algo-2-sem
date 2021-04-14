//
// Created by Timur Kharisov on 12.04.2021.
//

#ifndef LIST__FASTALLOCATOR_H_
#define LIST__FASTALLOCATOR_H_

#include <iostream>
#include <memory>
#include <vector>
#include <list>

template<size_t chunkSize>
class FixedAllocator {
 public:
  FixedAllocator() {
    pools_.push_back(new Block(1));
  }

  void* allocate() {
    if (size_ == index_) {
      Resize();
      index_ = 0;
    }
    ++index_;
    return static_cast<void*>(pools_.back()->data + (index_ - 1) * chunkSize);
  }

  void deallocate() {}

  ~FixedAllocator() {
    for (auto pointer : pools_) {
      delete pointer;
    }
  }
 private:
  struct Block {
    int8_t* data;
    size_t sz{1};
    Block(const size_t n) {
      data = static_cast<int8_t*>(::operator new(chunkSize * n));
      sz = n;
    }
    ~Block() {
      ::operator delete(data, chunkSize * sz);
    }
  };

  std::vector<Block*> pools_;
  const size_t max_size_ = 1'000'000;
  size_t index_{0};
  size_t size_{1};

  void Resize() {
    if (size_ < max_size_) {
      size_ *= 2;
    }
    pools_.push_back(new Block(size_));
  }

};

template<typename T>
class FastAllocator {
 public:
  FastAllocator() = default;
  FastAllocator(const FastAllocator<T>&) = default;

  template<typename S>
  FastAllocator(const FastAllocator<S>&) {}

  T* allocate(const size_t amount) {
    return (amount == 1) ? static_cast<T*>(fixed->allocate()) : static_cast<T*>(::operator new(amount * sizeof(T)));
  }

  void deallocate(T* pointer, size_t arg) {
    if (arg != 1) {
      ::operator delete(pointer, arg);
    }
  }

  template<typename... Args>
  void construct(T* pointer, const Args& ... args) {
    new(pointer) T(args...);
  }

  void destroy(T* pointer) {
    pointer->~T();
  }

  using value_type = T;

  template<typename S>
  struct rebind {
    using other = FastAllocator<S>;
  };

 private:
  std::shared_ptr<FixedAllocator<sizeof(T)>> fixed = std::make_shared<FixedAllocator<sizeof(T)>>();
};

template<typename T, typename S>
bool operator==(const FastAllocator<T>& first_argument, const FastAllocator<S>& second_argument) {
  return (&first_argument == &second_argument);
}

template<typename T, typename S>
bool operator!=(const FastAllocator<T>& first_argument, const FastAllocator<S>& second_argument) {
  return !(first_argument == second_argument);
}

template<typename T, typename Allocator = std::allocator<T>>
class List {
 private:
  struct Node {
    T value;
    Node* next{nullptr};
    Node* previous{nullptr};
    Node() = default;
    Node(const T& arg, Node* arg_next = nullptr, Node* arg_previous = nullptr)
        : value(arg), next(arg_next), previous(arg_previous) {}
    Node(Node* arg_next, Node* arg_previous) : value(), next(arg_next), previous(arg_previous) {}
  };

  void custom_insert(Node* pointer, const T& value) {
    Node* new_node = AllocTraits::allocate(allocator_, 1);
    AllocTraits::construct(allocator_, new_node, value, pointer->next, pointer);
    if (pointer->next != nullptr) {
      pointer->next->previous = new_node;
    }
    pointer->next = new_node;
    ++size_;
  }

  void custom_erase(Node* pointer) {
    if (pointer->next != nullptr) {
      pointer->next->previous = pointer->previous;
    }
    if (pointer->previous != nullptr) {
      pointer->previous->next = pointer->next;
    }
    allocator_.destroy(pointer);
    allocator_.deallocate(pointer, 1);
    --size_;
  }

  template<bool IsConst>
  class common_iterator;

 public:
  using AllocTraits = typename std::allocator_traits<typename std::allocator_traits<Allocator>::template rebind_alloc<
      Node>>;
  explicit List(const Allocator& alloc = Allocator()) : allocator_(alloc) {
    start_ = allocator_.allocate(1);
    end_ = allocator_.allocate(1);
    start_->next = end_;
    end_->previous = start_;
  }

  List(size_t count, const T& value, const Allocator& alloc = Allocator()) : List(alloc) {
    for (size_t i = 0; i < count; ++i) {
      push_back(value);
    }
  }

  List(size_t count, const Allocator& alloc = Allocator()) : List(alloc) {
    for (size_t i = 0; i < count; ++i) {
      Node* pointer = end_->previous;
      Node* new_node = AllocTraits::allocate(allocator_, 1);
      AllocTraits::construct(allocator_, new_node, pointer->next, pointer);
      if (pointer->next != nullptr) {
        pointer->next->previous = new_node;
      }
      pointer->next = new_node;
      ++size_;
    }
  }

  List(const List<T, Allocator>& arg) :
      List(std::allocator_traits<Allocator>::select_on_container_copy_construction(arg.get_allocator())) {
    for (auto it = arg.begin(); it != arg.end(); ++it) {
      push_back(*it);
    }
  }

  List<T, Allocator>& operator=(const List& other) {
    if (&other == this) {
      return *this;
    }
    bool if_copy_alloc = std::allocator_traits<Allocator>::propagate_on_container_copy_assignment::value;
    while (size_ > 0) {
      pop_back();
    }
    if (if_copy_alloc) {
      allocator_ = other.allocator_;
    }
    for (auto it = other.begin(); it != other.end(); ++it) {
      push_back(*it);
    }
    return *this;
  }

  void push_back(const T& value = T()) {
    custom_insert(end_->previous, value);
  }

  void push_front(const T& value = T()) {
    custom_insert(start_, value);
  }

  void pop_back() {
    custom_erase(end_->previous);
  }

  void pop_front() {
    custom_erase(start_->next);
  }

  Allocator get_allocator() const {
    return allocator_;
  }

  size_t size() const {
    return size_;
  }

  using iterator = common_iterator<false>;

  using const_iterator = common_iterator<true>;

  using reverse_iterator = std::reverse_iterator<iterator>;

  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  iterator begin() {
    return iterator(start_->next);
  }

  iterator end() {
    return iterator(end_);
  }

  const_iterator cbegin() const {
    return const_iterator(start_->next);
  }

  const_iterator cend() const {
    return const_iterator(end_);
  }

  const_iterator begin() const {
    return cbegin();
  }

  const_iterator end() const {
    return cend();
  }

  reverse_iterator rbegin() {
    return reverse_iterator(end_);
  }

  reverse_iterator rend() {
    return reverse_iterator(start_->next);
  }

  const_reverse_iterator crbegin() const {
    return const_reverse_iterator(end_);
  }

  const_reverse_iterator crend() const {
    return const_reverse_iterator(start_->next);
  }

  reverse_iterator rbegin() const {
    return crbegin();
  }

  reverse_iterator rend() const {
    return crend();
  }

  template<typename Iterator>
  void insert(Iterator pos, const T& value) {
    custom_insert(pos.pointer_->previous, value);
  }

  template<typename Iterator>
  void erase(Iterator pos) {
    custom_erase(pos.pointer_);
  }

  ~List() {
    while (size_ > 0) {
      pop_back();
    }
    allocator_.deallocate(start_, 1);
    allocator_.deallocate(end_, 1);
    start_ = nullptr;
    end_ = nullptr;
  }

 private:

  Node* start_{nullptr};
  Node* end_{nullptr};
  size_t size_{0};
  typename AllocTraits::template rebind_alloc<Node> allocator_;

  template<bool IsConst>
  class common_iterator {
   public:
    friend List;

    using value_type = T;
    using pointer = std::conditional_t<IsConst, const T*, T*>;
    using reference = std::conditional_t<IsConst, const T&, T&>;
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;

    //common_iterator() = default;

    common_iterator(Node* arg_pointer) : pointer_(arg_pointer) {}

    //common_iterator(const common_iterator<IsConst>&) = default;

    //common_iterator<IsConst>& operator=(const common_iterator<IsConst>&) = default;

    template<bool IsConst2>
    explicit operator common_iterator<IsConst2>() {
      common_iterator<IsConst2> copy = common_iterator<IsConst2>(pointer_);
      return copy;
    }

    std::conditional_t<IsConst, const T*, T*> operator->() {
      return &(pointer_->value);
    }

    std::conditional_t<IsConst, const T&, T&> operator*() {
      return pointer_->value;
    }

    common_iterator& operator++() {
      pointer_ = pointer_->next;
      return *this;
    }

    common_iterator operator++(int) {
      auto return_value = *this;
      pointer_ = pointer_->next;
      return return_value;
    }

    common_iterator& operator--() {
      pointer_ = pointer_->previous;
      return *this;
    }

    common_iterator operator--(int) {
      auto return_value = *this;
      pointer_ = pointer_->previous;
      return return_value;
    }

    bool operator==(const common_iterator& other) {
      return (other.pointer_ == pointer_);
    }

    bool operator!=(const common_iterator& other) {
      return !(*this == other);
    }

   private:
    Node* pointer_;

  };
};

#endif //LIST__FASTALLOCATOR_H_
