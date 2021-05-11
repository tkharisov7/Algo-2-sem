#include <vector>
#include <functional>
#include <memory>
#include <iostream>

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

  void deallocate(void* ptr, size_t n) {
    Block* current = static_cast<Block*>(ptr);

  }

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
  Block* free_;
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
    explicit Node(const T& arg, Node* arg_next = nullptr, Node* arg_previous = nullptr)
        : value(arg), next(arg_next), previous(arg_previous) {}
    /*explicit Node(T&& arg, Node* arg_next = nullptr, Node* arg_previous = nullptr)
        : value(std::make_pair(std::move(const_cast<typename std::remove_const<decltype(value.first)>::type&>(value.first)),
                               std::move(value.second))), next(arg_next), previous(arg_previous) {}*/
    template<typename... Args>
    explicit Node(Node* arg_next, Node* arg_previous, Args&& ... args) : value(std::forward<Args>(
        args)...), next(arg_next), previous(arg_previous) {}
    explicit Node(Node* arg_next, Node* arg_previous) : value(), next(arg_next), previous(arg_previous) {}
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
    if (pointer == nullptr) {
      return;
    }
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

  List(List<T, Allocator>&& other)
      : start_(other.start_), end_(other.end_), size_(other.size_), allocator_(other.allocator_) {
    other.start_ = allocator_.allocate(1);
    other.end_ = allocator_.allocate(1);
    other.start_->next = other.end_;
    other.end_->previous = other.start_;
    other.size_ = 0;
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

  List<T, Allocator>& operator=(List&& other) noexcept {
    if (&other == this) {
      return *this;
    }
    allocator_ = other.allocator_;
    while (size_ > 0) {
      pop_back();
    }
    swap(other);
    return *this;
  }

  void swap(List& other) {
    std::swap(start_, other.start_);
    std::swap(end_, other.end_);
    std::swap(size_, other.size_);
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

  template<typename... Args>
  iterator emplace(iterator it, Args&& ... args) {
    Node* new_node = AllocTraits::allocate(allocator_, 1);
    auto pointer = it.pointer_->previous;
    AllocTraits::construct(allocator_, new_node, pointer->next, pointer, std::forward<Args>(args)...);
    if (pointer->next != nullptr) {
      pointer->next->previous = new_node;
    }
    pointer->next = new_node;
    ++size_;
    return --it;
  }

  void splice(iterator it_to, List& from, iterator it_from) {
    if (it_from.pointer_->next != nullptr) {
      it_from.pointer_->next->previous = it_from.pointer_->previous;
    }
    if (it_from.pointer_->previous != nullptr) {
      it_from.pointer_->previous->next = it_from.pointer_->next;
    }
    --from.size_;
    ++size_;
    if (it_to.pointer_->previous != nullptr) {
      it_to.pointer_->previous->next = it_from.pointer_;
    }
    it_from.pointer_->previous = it_to.pointer_->previous;
    it_from.pointer_->next = it_to.pointer_;
    it_to.pointer_->previous = it_from.pointer_;
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

    common_iterator(Node* arg_pointer) : pointer_(arg_pointer) {}

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

template<typename Key, typename Value, typename Hash = std::hash<Key>, typename Equal = std::equal_to<Key>,
    typename Alloc = std::allocator<std::pair<const Key, Value>>>
class UnorderedMap {
 public:
  using NodeType = std::pair<const Key, Value>;
 private:

  class Iterator : public List<NodeType>::iterator {
   public:
    friend class UnorderedMap;
    Iterator(const typename List<NodeType>::iterator& it) : List<NodeType>::iterator(it) {}
  };

  class ConstIterator : public List<NodeType>::const_iterator {
   public:
    friend class UnorderedMap;
    ConstIterator(const typename List<NodeType>::const_iterator& it) : List<NodeType>::const_iterator(it) {}
    ConstIterator(const Iterator& it) : List<NodeType>::const_iterator(it) {}
  };
 public:

  friend int main();

  //default constructor
  UnorderedMap() {
    heads_ = std::vector<Iterator>(10, end());
  }

  //copy constructor
  UnorderedMap(const UnorderedMap& arg)
      : max_load_factor_(arg.max_load_factor_) {
    heads_ = std::vector<Iterator>(10, end());
    for (auto it = arg.begin(); it != arg.end(); ++it) {
      insert(*it);
    }
  }

  //move constructor
  UnorderedMap(UnorderedMap&& arg)
      : list_(std::move(arg.list_)),
        heads_(std::move(arg.heads_)), size_(std::move(arg.size_)), max_load_factor_(std::move(arg.max_load_factor_)) {
  }

  //copy assignment operator
  UnorderedMap& operator=(const UnorderedMap& arg) {
    if (&arg != this) {
      max_load_factor_ = arg.max_load_factor_;
      erase(begin(), end());
      for (auto it = arg.begin(); it != arg.end(); ++it) {
        insert(*it);
      }
    }
    return *this;
  }

  //move assignment operator
  UnorderedMap& operator=(UnorderedMap&& arg) {
    if (&arg != this) {
      erase(begin(), end());
      list_ = std::move(arg.list_);
      heads_ = std::move(arg.heads_);
      size_ = std::move(arg.size_);
      max_load_factor_ = std::move(arg.max_load_factor_);
    }
    return *this;
  }

  //operator []
  Value& operator[](const Key& key_arg) {
    return (insert(std::make_pair(key_arg, Value())).first)->second;
  }

  //operator at
  Value& at(const Key& key_arg) {
    Iterator ans = find(key_arg);
    if (ans != end()) {
      return ans->second;
    } else {
      throw std::out_of_range("There is no element at current key!");
    }
  }

  //begin iterator method
  Iterator begin() {
    return list_.begin();
  }

  //end iterator method
  Iterator end() {
    return list_.end();
  }

  //begin iterator method from const
  ConstIterator begin() const {
    return list_.begin();
  }

  //end iterator method from const
  ConstIterator end() const {
    return list_.end();
  }

  //cbegin iterator method
  ConstIterator cbegin() const {
    return list_.begin();
  }

  //cend iterator method
  ConstIterator cend() const {
    return list_.end();
  }

  //insert method from NodeType
  std::pair<Iterator, bool> insert(const NodeType& arg_node) {
    return emplace(arg_node);
  }

  //insert method from iterator to iterator
  template<typename IteratorType>
  void insert(IteratorType arg_iterator_first, IteratorType arg_iterator_second) {
    for (auto it = arg_iterator_first; it != arg_iterator_second; ++it) {
      insert(*it);
    }
  }

  //insert method from rvalue
  template<typename Type>
  std::pair<Iterator, bool> insert(Type&& arg) {
    return emplace(std::forward<Type>(arg));
  }

  //emplace method
  template<typename... Args>
  std::pair<Iterator, bool> emplace(Args&& ... args) {
    NodeType* new_node = std::allocator_traits<Alloc>::allocate(allocator_, 1);
    std::allocator_traits<Alloc>::construct(allocator_, new_node, std::forward<Args>(args)...);
    Iterator cur = find(new_node->first);
    if (cur != end()) {
      std::allocator_traits<Alloc>::destroy(allocator_, new_node);
      std::allocator_traits<Alloc>::deallocate(allocator_, new_node, 1);
      return std::make_pair(cur, false);
    } else {
      size_t index = Hash()(new_node->first) % heads_.size();
      heads_[index] = list_.emplace(heads_[index],
                                    std::move(const_cast<Key&>(new_node->first)),
                                    std::move(const_cast<Value&>(new_node->second)));
      ++size_;
      check();
      std::allocator_traits<Alloc>::destroy(allocator_, new_node);
      std::allocator_traits<Alloc>::deallocate(allocator_, new_node, 1);
      return std::make_pair(heads_[index], true);
    }
  }

  //erase method
  void erase(Iterator arg) {
    if (arg == end()) {
      return;
    }
    size_t index = Hash()(arg->first) % heads_.size();
    if (heads_[index] == arg) {
      auto it_next = arg;
      ++it_next;
      if (Equal()((*it_next).first, (*arg).first)) {
        heads_[index] = it_next;
      } else {
        heads_[index] = end();
      }
    }
    list_.erase(arg);
    --size_;
  }

  //erase method from iterator to iterator
  void erase(Iterator arg_iterator_first, Iterator arg_iterator_second) {
    for (auto it = arg_iterator_first; it != arg_iterator_second;) {
      auto copy_it = it;
      ++it;
      erase(copy_it);
    }
  }

  //find method
  Iterator find(const Key& arg_key) {
    size_t index = Hash()(arg_key) % heads_.size();
    Iterator it = heads_[index];
    while (it != end() && index == Hash()(it->first) % heads_.size()) {
      if (Equal()(arg_key, (it->first))) {
        return it;
      }
      ++it;
    }
    return end();
  }

  //Returns average amount of elements in bucket
  float load_factor() const {
    return static_cast<float>(size_) / static_cast<float>(heads_.size());
  }

  //Returns maximum value of load_factor
  float max_load_factor() const {
    return max_load_factor_;
  }

  //Updates the number of buckets to needed
  void reserve(size_t arg_size) {
    if (arg_size > heads_.size()) {
      rehash(arg_size);
    }
  }

  size_t size() {
    return size_;
  }

  //Destructor
  ~UnorderedMap() = default;

 private:
  List<NodeType> list_;
  std::vector<Iterator> heads_;
  size_t size_{0};
  float max_load_factor_{0.95};
  Alloc allocator_;

  //Makes a rehash of current hash-table
  void rehash(size_t arg_size) {
    List<NodeType> list_copy;
    heads_ = std::vector<Iterator>(arg_size, list_copy.end());
    for (auto it1 = list_.begin(); it1 != list_.end();) {
      size_t index = Hash()(it1->first) % heads_.size();
      Iterator it = heads_[index];
      auto copy_it1 = it1;
      ++it1;
      list_copy.splice(it, list_, copy_it1);
      --it;
      heads_[index] = it;
      //list_copy.insert(it, std::move(*it1));
    }
    list_ = std::move(list_copy);
    for (auto& it : heads_) {
      if (it == list_copy.end()) {
        it = list_.end();
      }
    }
  };

  void check() {
    if (load_factor() > max_load_factor_) {
      rehash(2 * heads_.size());
    }
  }

};
