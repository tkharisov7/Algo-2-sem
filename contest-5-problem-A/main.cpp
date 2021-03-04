#include <iostream>
#include <vector>
#include <string>

class HashTable {
 public:
  HashTable(size_t capacity = 4) : capacity_(capacity), amount_(0),
                                   hash_table_(std::vector<Node*>(capacity, nullptr)) {}

  std::pair<int, bool> Add(const std::string& argument, int addition_value, bool not_resize) {
    size_t hash_value = HashFunction(argument);
    Node* current_node = hash_table_[hash_value];
    Node* parent = nullptr;
    int return_value = -1;
    bool is_new = true;
    while (current_node != nullptr && current_node->value != argument) {
      parent = current_node;
      current_node = current_node->child;
    }
    if (current_node == nullptr) {
      ++amount_;
      Node* new_node = new Node;
      new_node->value = argument;
      new_node->x = addition_value;
      if (parent != nullptr) {
        parent->child = new_node;
      } else {
        hash_table_[hash_value] = new_node;
      }
      current_node = new_node;
    } else {
      current_node->x += addition_value;
    }
    if (not_resize) {
      return_value = current_node->x;
      is_new = false;
    }
    if (static_cast<double>(amount_) / static_cast<double>(capacity_) > LOAD_FACTOR) {
      Resize();
    }
    return {return_value, is_new};
  }

 private:
  size_t capacity_{4};
  size_t amount_{0};
  struct Node {
    std::string value;
    int x{0};
    Node* child{nullptr};
  };
  std::vector<Node*> hash_table_;
  const int PRIME = 1117809793;
  const double LOAD_FACTOR = 0.95;

  size_t HashFunction(const std::string& argument) const {//using a polynomial one here
    return static_cast<size_t>(std::hash<std::string>{}(argument) % PRIME % capacity_);
  }

  void Resize() {
    std::vector<Node*> copy_hash_table = hash_table_;
    capacity_ *= 2;
    hash_table_ = std::vector<Node*>(capacity_, nullptr);
    amount_ = 0;
    for (size_t i = 0; i < capacity_ / 2; ++i) {
      Node* current_node = copy_hash_table[i];
      while (current_node != nullptr) {
        Add(current_node->value, current_node->x, false);
        Node* t = current_node;
        current_node = current_node->child;
        delete t;
      }
    }
  }
};

int main() {
  HashTable device(4);
  std::string s;
  while (std::cin >> s) {
    int x;
    std::cin >> x;
    std::pair<int, bool> output_value = device.Add(s, x, true);
    if (!output_value.second) {
      std::cout << output_value.first << "\n";
    }
  }
}
