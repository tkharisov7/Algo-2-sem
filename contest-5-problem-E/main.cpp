#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

const long PRIME1 = 1117809793;

class HashTable {
 private:
  struct Node{
    std::string key;
    std::string val;
    Node* child{nullptr};
  };
 public:
  HashTable(size_t capacity = 4) : capacity_(capacity), hash_table_(std::vector<Node*> (capacity, nullptr)) {}

  void Put(const std::string& x, const std::string& y) {
    size_t hash_value = hash_function(x);
    std::pair<Node*, Node*> temp = find(x);
    Node* cur = temp.second;
    Node* parent = temp.first;
    if (cur == nullptr) {
      Node* new_node = new Node;
      new_node->key = x;
      new_node->val = y;
      if (parent == nullptr) {
        hash_table_[hash_value] = new_node;
      } else {
        parent->child = new_node;
      }
    } else {
      cur->val = y;
    }
  }

  void Get(const std::string& x) {
    std::pair<Node*, Node*> temp = find(x);
    Node* cur = temp.second;
    if (cur != nullptr) {
      std::cout << cur->val << "\n";
    } else {
      std::cout << "none" << "\n";
    }
  }

  void Delete(const std::string& x) {
    size_t hash_value = hash_function(x);
    std::pair<Node*, Node*> temp = find(x);
    Node* parent = temp.first;
    Node* cur = temp.second;
    if (cur != nullptr) {
      if (parent != nullptr) {
        parent->child = cur->child;
      } else {
        hash_table_[hash_value] = nullptr;
      }
    }
    delete cur;
  }

 private:
  size_t capacity_{4};
  std::vector<Node*> hash_table_;

  size_t hash_function(const std::string& arg) const {//using a polynomial one here
    return static_cast<size_t>(std::hash<std::string>{}(arg) % PRIME1 % capacity_);
  }

  std::pair<Node*, Node*> find(const std::string& arg) const {
    size_t hash_value = hash_function(arg);
    Node* cur = hash_table_[hash_value];
    Node* parent = nullptr;
    while (cur != nullptr && cur->key != arg) {
      parent = cur;
      cur = cur->child;
    }
    return {parent, cur};
  }

};

int main() {
  freopen("map.in", "r", stdin);
  freopen("map.out", "w", stdout);
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
  HashTable a(100000);
  std::string command;
  while (std::cin >> command) {
    std::string x;
    std::string y;
    std::cin >> x;
    if (command == "put") {
      std::cin >> y;
      a.Put(x, y);
    }
    if (command == "get") {
      a.Get(x);
    }
    if (command == "delete") {
      a.Delete(x);
    }
  }
}
