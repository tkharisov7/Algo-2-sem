#include <iostream>
#include <vector>

bool IsInside(int x, int y, int width, int height) {
  return !(x < 1 || x > width || y < 1 || y > height);
}

class HashTable {
 public:
  HashTable(size_t capacity = 4) : capacity_(capacity), amount_(0), hash_table_(std::vector<Node*> (capacity, nullptr)) {}

  size_t Amount() const {
    return amount_;
  }

  void Add(const int x, const int y, const int width, const int height) {
    if (!IsInside(x, y, width, height)) {
      return;
    }
    size_t hash_value = HashFunction(x, y);
    Node* cur = hash_table_[hash_value];
    Node* parent = nullptr;
    std::pair<int, int> point = {x, y};
    while (cur != nullptr && cur->value != point) {
      parent = cur;
      cur = cur->child;
    }
    if (cur == nullptr) {
      ++amount_;
      Node* new_node = new Node;
      new_node->value = point;
      if (parent == nullptr) {
        hash_table_[hash_value] = new_node;
      } else {
        parent->child = new_node;
      }
    }
  }

 private:
  struct Node{
    std::pair<int, int> value;
    Node* child{nullptr};
  };

  size_t capacity_{4};
  size_t amount_{0};
  std::vector<Node*> hash_table_;
  const long PRIME1 = 1117809793;
  const long PRIME2 = 965100379;

  size_t HashFunction(const long arg1, const long arg2) const {//using hash_table polynomial one here
    return static_cast<size_t>((arg1 * PRIME1 + arg2) % PRIME2 % capacity_);
  }

};

  void Solve(HashTable& hash_table, int width, int height, int n) {
    while (n-- > 0) {
      int x, y;
      std::cin >> x >> y;
      hash_table.Add(x, y, width, height);
      hash_table.Add(x, y - 1, width, height);
      hash_table.Add(x, y + 1, width, height);
      hash_table.Add(x - 1, y, width, height);
      hash_table.Add(x + 1, y, width, height);
    }
    if (static_cast<long long>(hash_table.Amount()) < static_cast<long long>(width) * static_cast<long long>(height)) {
      std::cout << "No";
    } else {
      std::cout << "Yes";
    }
  }

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
  HashTable hash_table(2500000);
  int width, height, n;
  std::cin >> width >> height >> n;
  Solve(hash_table, width, height, n);
}
