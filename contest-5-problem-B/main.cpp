#include <iostream>
#include <vector>

const int PRIME = 1117809793;

class HashTable {
 private:
  struct Node{
    int value{0};
    int x{0};
    Node* son{nullptr};
  };
 public:
  HashTable(size_t capacity = 4) : capacity_(capacity), amount_(0), hash_table_(std::vector<Node*> (capacity, nullptr)) {}

  std::pair<Node*, Node*> Find(const int arg) {
    size_t hash_value = hash_function(arg);
    Node* cur = hash_table_[hash_value];
    Node* parent = nullptr;
    while (cur != nullptr && cur->value != arg) {
      parent = cur;
      cur = cur->son;
    }
    return {parent, cur};
  }

  void Upd(const int arg, const int val) {
    size_t hash_value = hash_function(arg);
    std::pair<Node*, Node*> pos = Find(arg);
    Node* cur = pos.second;
    Node* parent = pos.first;
    if (cur == nullptr) {
      ++amount_;
      Node* new_node = new Node;
      new_node->value = arg;
      new_node->x = val;
      if (parent != nullptr) {
        parent->son = new_node;
      } else {
        hash_table_[hash_value] = new_node;
      }
    }
  }

  void Oper(const int arg1, const int arg2) {
    std::pair<Node*, Node*> pos1 = Find(arg1);
    std::pair<Node*, Node*> pos2 = Find(arg2);
    if (pos1.second == nullptr) {
      ++amount_;
      size_t hash_value = hash_function(arg1);
      Node* new_node = new Node;
      new_node->value = arg1;
      new_node->x = arg1;
      new_node->son = nullptr;
      if (pos1.first != nullptr) {
        pos1.first->son = new_node;
      } else {
        hash_table_[hash_value] = new_node;
      }
      pos1.second = new_node;
    }
    if (pos2.second == nullptr) {
      ++amount_;
      size_t hash_value = hash_function(arg2);
      Node* new_node = new Node;
      new_node->value = arg2;
      new_node->x = arg2;
      new_node->son = nullptr;
      if (pos2.first != nullptr) {
        pos2.first->son = new_node;
      } else {
        hash_table_[hash_value] = new_node;
      }
      pos2.second = new_node;
    }
    std::swap(pos1.second->x, pos2.second->x);
    //if (static_cast<double>(amount_) / static_cast<double>(capacity_) > 0.95) {
    //  Resize();
    //}
    std::cout << std::abs(pos1.second->x - pos2.second->x) << "\n";
  }
 private:
  size_t capacity_{4};
  size_t amount_{0};
  std::vector<Node*> hash_table_;

  size_t hash_function(const int arg) {//using a polynomial one here
    return static_cast<size_t>(std::hash<int>{}(arg) % PRIME % capacity_);
  }

  void Resize() {
    std::vector<Node*> copy_hash_table = hash_table_;
    capacity_ *= 2;
    hash_table_ = std::vector<Node*> (capacity_, nullptr);
    amount_ = 0;
    for (size_t i = 0; i < capacity_ / 2; ++i) {
      Node* cur = copy_hash_table[i];
      while (cur != nullptr) {
        Upd(cur->value, cur->x);
        Node* t = cur;
        cur = cur->son;
        delete t;
      }
    }
  }
};

int main() {
  HashTable a(200000);
  int t;
  std::cin >> t;
  while (t) {
    int x, y;
    std::cin >> x >> y;
    a.Oper(x, y);
    --t;
  }
}
