#include <iostream>
#include <vector>

class HashTable {
 private:
  struct Node {
    int value{0};
    int x{0};
    Node* child{nullptr};
  };
 public:
  HashTable(size_t capacity = 4) : capacity_(capacity), amount_(0),
                                   hash_table_(std::vector<Node*>(capacity, nullptr)) {}

  std::pair<Node*, Node*> Find(const int key) const {
    size_t hash_value = hash_function(key);
    Node* current_node = hash_table_[hash_value];
    Node* parent = nullptr;
    while (current_node != nullptr && current_node->value != key) {
      parent = current_node;
      current_node = current_node->child;
    }
    return {parent, current_node};
  }

  int Operation(const int arg1, const int arg2) {
    std::pair<Node*, Node*> pos1 = Find(arg1);
    std::pair<Node*, Node*> pos2 = Find(arg2);
    if (pos1.second == nullptr) {
      create_node(pos1, arg1);
    }
    if (pos2.second == nullptr) {
      create_node(pos2, arg2);
    }
    std::swap(pos1.second->x, pos2.second->x);
    return std::abs(pos1.second->x - pos2.second->x);
  }

  ~HashTable() {
    for (auto current : hash_table_) {
      while (current != nullptr) {
        Node* child = current->child;
        delete current;
        current = child;
      }
    }
  }

 private:
  size_t capacity_{4};
  size_t amount_{0};
  std::vector<Node*> hash_table_;
  const int PRIME = 1117809793;

  size_t hash_function(const int argument) const {//using a polynomial one here
    return static_cast<size_t>(std::hash<int>{}(argument) % PRIME % capacity_);
  }

  void create_node(std::pair<Node*, Node*>& position, const int value) {
    ++amount_;
    size_t hash_value = hash_function(value);
    Node* new_node = new Node;
    new_node->value = value;
    new_node->x = value;
    new_node->child = nullptr;
    if (position.first != nullptr) {
      position.first->child = new_node;
    } else {
      hash_table_[hash_value] = new_node;
    }
    position.second = new_node;
  }
};

int main() {
  HashTable hash_table(200000);
  int n;
  std::cin >> n;
  while (n) {
    int x, y;
    std::cin >> x >> y;
    std::cout << hash_table.Operation(x, y) << "\n";
    --n;
  }
}
