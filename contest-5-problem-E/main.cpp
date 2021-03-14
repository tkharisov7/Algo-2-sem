#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class HashTable {
 public:
  enum class OperationType {
    Put, Get, Delete
  };

  HashTable(size_t capacity = 4) : capacity_(capacity), hash_table_(std::vector<Node*>(capacity, nullptr)) {}

  void Put(const std::string& key, const std::string& value) {
    size_t hash_value = HashFunction(key);
    std::pair<Node*, Node*> temp = Find(key);
    Node* current = temp.second;
    Node* parent = temp.first;
    if (current == nullptr) {
      Node* new_node = new Node;
      new_node->key = key;
      new_node->val = value;
      if (parent == nullptr) {
        hash_table_[hash_value] = new_node;
      } else {
        parent->child = new_node;
      }
    } else {
      current->val = value;
    }
  }

  std::string Get(const std::string& key) const {
    std::pair<Node*, Node*> temp = Find(key);
    Node* current = temp.second;
    if (current != nullptr) {
      return current->val;
    } else {
      return "none";
    }
  }

  void Delete(const std::string& key) {
    size_t hash_value = HashFunction(key);
    std::pair<Node*, Node*> temp = Find(key);
    Node* parent = temp.first;
    Node* current = temp.second;
    if (current != nullptr) {
      if (parent != nullptr) {
        parent->child = current->child;
      } else {
        hash_table_[hash_value] = nullptr;
      }
    }
    delete current;
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
  struct Node {
    std::string key;
    std::string val;
    Node* child{nullptr};
  };
  size_t capacity_{4};
  std::vector<Node*> hash_table_;
  const long PRIME1 = 1117809793;

  size_t HashFunction(const std::string& arg) const {//using a polynomial one here
    return static_cast<size_t>(std::hash<std::string>{}(arg) % PRIME1 % capacity_);
  }

  std::pair<Node*, Node*> Find(const std::string& key) const {
    size_t hash_value = HashFunction(key);
    Node* current = hash_table_[hash_value];
    Node* parent = nullptr;
    while (current != nullptr && current->key != key) {
      parent = current;
      current = current->child;
    }
    return {parent, current};
  }
};

HashTable::OperationType InputToEnum(const std::string& s) {
  if (s == "put") {
    return HashTable::OperationType::Put;
  }
  if (s == "get") {
    return HashTable::OperationType::Get;
  }
  if (s == "delete") {
    return HashTable::OperationType::Delete;
  }
}

void Solve(std::ifstream& in, std::ofstream& out, HashTable& hash_table) {
  std::string command;
  while (in >> command) {
    std::string key;
    std::string value;
    in >> key;
    switch (InputToEnum(command)) {
      case HashTable::OperationType::Put:
        in >> value;
        hash_table.Put(key, value);
        break;
      case HashTable::OperationType::Get:
        out << hash_table.Get(key) << "\n";
        break;
      case HashTable::OperationType::Delete:
        hash_table.Delete(key);
    }
  }
}

int main() {
  std::ifstream in("map.in");
  std::ofstream out("map.out");
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
  HashTable hash_table(100000);
  Solve(in, out, hash_table);
}
