#include <iostream>
#include <vector>

template<typename T>
class custom_vector {
 public:

  explicit custom_vector(int size) {
    array_ = reinterpret_cast<T*>(new int8_t[size * sizeof(T)]);
  }

  custom_vector() {
    array_ = reinterpret_cast<T*>(new int8_t[capacity_ * sizeof(T)]);
  }

  custom_vector(int amount, const T& value) {
    array_ = reinterpret_cast<T*>(new int8_t[capacity_ * sizeof(T)]);
    try {
      for (int i = 0; i < amount; ++i) {
        push_back(value);
      }
    } catch (...) {
      delete[] reinterpret_cast<int8_t*>(array_);
      throw;
    }
  }

  custom_vector(const custom_vector<T>& arg) {
    capacity_ = arg.capacity_;
    size_ = arg.size_;
    array_ = reinterpret_cast<T*>(new int8_t[capacity_ * sizeof(T)]);
    try {
      std::uninitialized_copy(arg.array_, arg.array_ + arg.size_, array_);
    } catch (...) {
      delete[] reinterpret_cast<int8_t*>(array_);
      throw;
    }
  }

  T& operator[](int i) {
    return array_[i];
  };

  const T& operator[](int i) const {
    return array_[i];
  }

  void push_back(const T& x) {
    try {
      new(array_ + size_) T(x);
    } catch (...) {
      delete[] reinterpret_cast<int8_t*>(array_);
      throw;
    }
    ++size_;
  }

  void pop_back() {
    --size_;
    (array_ + size_)->~T();
  }

  T* start() {
    return array_;
  }

  ~custom_vector() {
    delete[] reinterpret_cast<int8_t*>(array_);
    array_ = nullptr;
    size_ = 0;
  }

 private:

  T* array_;
  int size_{0};
  const int capacity_{31};
};

template<typename T>
class Deque {
 private:
  template<bool IsConst>
  class common_iterator;

 public:
  Deque() {
    start_ = 0;
    end_ = 0;
    auto temp = new custom_vector<T>;
    array_.push_back(temp);
  }

  explicit Deque(int amount, const T& value = T()) {
    start_ = 0;
    end_ = amount;
    for (int i = 0; i < amount / block_size_; ++i) {
      auto temp = new custom_vector<T>(block_size_, value);
      array_.push_back(temp);
    }
    auto last_vector = new custom_vector<T>;
    for (int i = 0; i < amount % block_size_; ++i) {
      last_vector->push_back(value);
    }
    array_.push_back(last_vector);
  }

  Deque(const Deque<T>& arg) {
    start_ = 0;
    end_ = 0;
    auto it = arg.begin();
    while (it != arg.end()) {
      push_back(*it);
      ++it;
    }
  }

  Deque& operator=(const Deque<T>& arg) {
    for (size_t i = 0; i < array_.size(); ++i) {
      delete array_[i];
    }
    array_.clear();
    start_ = 0;
    end_ = 0;
    auto it = arg.begin();
    while (it != arg.end()) {
      push_back(*it);
      ++it;
    }
    return *this;
  }

  size_t size() const {
    return static_cast<size_t>(end_ - start_);
  }

  T& operator[](int index) {
    return (*array_[(start_ + index) / block_size_])[(start_ + index) % block_size_];
  }

  const T& operator[](int index) const {
    return (*array_[(start_ + index) / block_size_])[(start_ + index) % block_size_];
  }

  T& at(int index) {
    if (index >= static_cast<int>(size())) {
      throw std::out_of_range("....");
    }
    if (index < 0) {
      throw std::out_of_range("....");
    }
    return (*this)[index];
  }

  const T& at(int index) const {
    if (index >= size()) {
      throw std::out_of_range("....");
    }
    if (index < 0) {
      throw std::out_of_range("....");
    }
    return (*this)[index];
  }

  void push_back(const T& arg) {
    if (end_ % static_cast<int>(block_size_) == 0 && end_ / block_size_ == static_cast<int>(array_.size())) {
      array_.push_back(new custom_vector<T>);
    }
    array_[end_ / block_size_]->push_back(arg);
    ++end_;
  }

  void push_front(const T& arg) {
    if (start_ == 0) {
      move_front();
    }
    --start_;
    (*this)[0] = arg;
  }

  void pop_back() {
    --end_;
    if (end_ % block_size_ == 0) {
      delete array_[array_.size() - 1];
      array_.pop_back();
    }
    array_[end_ / block_size_]->pop_back();
  }

  void pop_front() {
    ((*this)[0]).~T();
    ++start_;
  }

  ~Deque() {
    for (size_t i = 0; i < array_.size(); ++i) {
      //array_[i]->~custom_vector<T>();
      delete array_[i];
    }
    array_.clear();
  }

  using iterator = common_iterator<false>;

  using const_iterator = common_iterator<true>;

  using reverse_iterator = std::reverse_iterator<iterator>;

  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  iterator begin() {
    return iterator(0, this);
  }

  iterator end() {
    return iterator(size(), this);
  }

  const_iterator cbegin() const {
    return const_iterator(0, this);
  }

  const_iterator cend() const {
    return const_iterator(size(), this);
  }

  const_iterator begin() const {
    return cbegin();
  }

  const_iterator end() const {
    return cend();
  }

  reverse_iterator rbegin() {
    return reverse_iterator(size() - 1, this);
  }

  reverse_iterator rend() {
    return reverse_iterator(-1, this);
  }

  const_reverse_iterator crbegin() const {
    return const_reverse_iterator(-1, this);
  }

  const_reverse_iterator crend() const {
    return const_reverse_iterator(size() - 1, this);
  }

  const_reverse_iterator rbegin() const {
    return crbegin();
  }

  const_reverse_iterator rend() const {
    return crend();
  }

  void insert(const iterator& pos, const T& value) {
    push_back(value);
    for (int i = static_cast<int>(size()) - 1; i > pos.index(); --i) {
      (*this)[i] = (*this)[i - 1];
    }
    (*this)[pos.index()] = value;
  }

  void erase(const iterator& pos) {
    for (int i = pos.index(); i < static_cast<int>(size()); ++i) {
      (*this)[i] = (*this)[i + 1];
    }
    pop_back();
  }

 private:
  static const int block_size_{31};
  int start_{0};
  int end_{0};

  std::vector<custom_vector<T>*> array_;

  void move_front() {
    int prev_size = array_.size();
    array_.resize(prev_size * 2);
    for (int i = 0; i < prev_size; ++i) {
      array_[i + prev_size] = array_[i];
      array_[i] = new custom_vector<T>;
    }
    start_ += prev_size * block_size_;
    end_ += prev_size * block_size_;
  }

  template<bool IsConst>
  class common_iterator {
   public:
    friend class Deque;

    common_iterator(const int index, const Deque<T>* arg) {
      deque_ = arg;
      index_ = (index + deque_->start_) % (deque_->block_size_);
      block_index_ = (index + deque_->start_) / (deque_->block_size_);
      int sz = deque_->array_.size();
      if (block_index_ >= sz) {
        pointer_ = (deque_->array_[sz - 1]->start() + deque_->block_size_);
      } else {
        pointer_ = (deque_->array_[block_index_])->start() + index_;
      }
    }

    common_iterator(const common_iterator& arg)
        : pointer_(arg.pointer_), index_(arg.index_), block_index_(arg.block_index_), deque_(arg.deque_) {}

    explicit operator common_iterator<true>() {
      common_iterator<true> copy = common_iterator<true>(*this);
      return copy;
    }

    std::conditional_t<IsConst, const T*, T*> operator->() {
      return pointer_;
    }

    std::conditional_t<IsConst, const T&, T&> operator*() {
      return *pointer_;
    }

    common_iterator& operator++() {
      ++index_;
      if (index_ >= static_cast<int>(deque_->block_size_)) {
        index_ = 0;
        ++block_index_;
        if (block_index_ < static_cast<int>(deque_->array_.size())) {
          pointer_ = (deque_->array_[block_index_])->start();
        } else {
          ++pointer_;
        }
      } else {
        ++pointer_;
      }
      return *this;
    }

    common_iterator operator++(int) {
      common_iterator<IsConst> result = *this;
      ++index_;
      if (index_ >= deque_->block_size_) {
        index_ = 0;
        ++block_index_;
        if (block_index_ < static_cast<int>(deque_->array_.size())) {
          pointer_ = (deque_->array_[block_index_])->start();
        } else {
          ++pointer_;
        }
      } else {
        ++pointer_;
      }
      return result;
    }

    common_iterator& operator--() {
      --index_;
      if (index_ < 0) {
        index_ = 0;
        --block_index_;
        pointer_ = deque_->array_[block_index_]->start() + deque_->block_size_ - 1;
      } else {
        --pointer_;
      }
      return *this;
    }

    common_iterator operator--(int) {
      common_iterator<IsConst> result = *this;
      --index_;
      if (index_ < 0) {
        index_ = 0;
        --block_index_;
        pointer_ = deque_->array_[block_index_]->start() + deque_->block_size_ - 1;
      } else {
        --pointer_;
      }
      return result;
    }

    common_iterator& operator+=(const int arg) {
      if (arg >= 0) {
        index_ += arg;
        block_index_ += index_ / deque_->block_size_;
        index_ %= deque_->block_size_;
        pointer_ = deque_->array_[block_index_]->start();
        pointer_ += index_;
      } else {
        int arg1 = -arg;
        index_ -= arg1;
        block_index_ += (index_ - deque_->block_size_ + 1) / deque_->block_size_;
        (index_) += (deque_->block_size_) * ((deque_->array_).size());
        index_ %= deque_->block_size_;
        pointer_ = deque_->array_[block_index_]->start();
        pointer_ += index_;
      }
      return *this;
    }

    common_iterator operator+(const int arg) {
      common_iterator<IsConst> result = *this;
      result += arg;
      return result;
    }

    common_iterator& operator-=(const int arg) {
      return (*this += (-arg));
    }

    common_iterator operator-(const int arg) {
      common_iterator<IsConst> result = *this;
      result -= arg;
      return result;
    }

    int operator-(const common_iterator<IsConst>& arg) {
      return (block_index_ * deque_->block_size_ + index_ - arg.block_index_ * deque_->block_size_ - arg.index_);
    }

    bool operator<(const common_iterator<IsConst>& arg) {
      if (block_index_ < arg.block_index_) {
        return true;
      }
      if (block_index_ > arg.block_index_) {
        return false;
      }
      return (index_ < arg.index_);
    }

    bool operator==(const common_iterator<IsConst>& arg) {
      if (block_index_ == arg.block_index_ && index_ == arg.index_) {
        return true;
      } else {
        return false;
      }
    }

    bool operator!=(const common_iterator<IsConst>& arg) {
      return !(*this == arg);
    }

    bool operator>(const common_iterator<IsConst>& arg) {
      if (block_index_ > arg.block_index_) {
        return true;
      }
      if (block_index_ < arg.block_index_) {
        return false;
      }
      return (index_ > arg.index_);
    }

    bool operator<=(const common_iterator<IsConst>& arg) {
      return !(*this > arg);
    }

    bool operator>=(const common_iterator<IsConst>& arg) {
      return !(*this < arg);
    }

    int index() const {
      return (index_ + block_index_ * (deque_->block_size_) - deque_->start_);
    }

   private:
    std::conditional_t<IsConst, const T*, T*> pointer_;
    int index_;
    int block_index_;
    const Deque<T>* deque_;

  };

};
