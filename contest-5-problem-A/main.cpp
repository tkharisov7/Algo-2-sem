#include <iostream>
#include <vector>
#include <string>

const int PRIME = 1117809793;

class HashTable {
 public:
  HashTable(size_t capacity = 4) : capacity_(capacity), amount_ (0), hash_table_(std::vector<Node*> (capacity, nullptr)) {}

  void Add(const std::string& arg, int val, bool flag) {
    size_t hash_value = hash_function(arg);
    Node* cur = hash_table_[hash_value];
    Node* parent = nullptr;
    while (cur != nullptr && cur->value != arg) {
      parent = cur;
      cur = cur->son;
    }
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
      cur = new_node;
    } else {
      cur->x += val;
    }
    if (flag) {
      std::cout << cur->x << "\n";
    }
    if (static_cast<double>(amount_) / static_cast<double>(capacity_) > 0.95) {
      Resize();
    }
  }

 private:
  size_t capacity_{4};
  size_t amount_{0};
  struct Node{
    std::string value{""};
    int x{0};
    Node* son{nullptr};
  };
  std::vector<Node*> hash_table_;

  size_t hash_function(const std::string& arg) {//using a polynomial one here
    return static_cast<size_t>(std::hash<std::string>{}(arg) % PRIME % capacity_);
  }

  void Resize() {
    std::vector<Node*> copy_hash_table = hash_table_;
    capacity_ *= 2;
    hash_table_ = std::vector<Node*> (capacity_, nullptr);
    amount_ = 0;
    for (size_t i = 0; i < capacity_ / 2; ++i) {
      Node* cur = copy_hash_table[i];
      while (cur != nullptr) {
          Add(cur->value, cur->x, false);
          Node* t = cur;
          cur = cur->son;
          delete t;
      }
    }
  }
};

int main() {
  HashTable a(4);
  std::string s;
  while (std::cin >> s) {
    int x;
    std::cin >> x;
    a.Add(s, x, true);
  }
}
