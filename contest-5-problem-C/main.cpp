#include <iostream>
#include <vector>

const long PRIME1 = 1117809793;
const long PRIME2 = 965100379;

class HashTable {
 private:
  struct Node{
    std::pair<int, int> value;
    Node* son{nullptr};
  };
 public:
  HashTable(size_t capacity = 4) : capacity_(capacity), amount_(0), hash_table_(std::vector<Node*> (capacity, nullptr)) {}

  size_t Amount() const {
    return amount_;
  }

  void Add(const int x, const int y, const int w, const int h) {
    if (x < 1 || x > w || y < 1 || y > h) {
      return;
    }
    size_t hash_value = hash_function(x, y);
    Node* cur = hash_table_[hash_value];
    Node* parent = nullptr;
    std::pair<int, int> point = {x, y};
    while (cur != nullptr && cur->value != point) {
      parent = cur;
      cur = cur->son;
    }
    if (cur == nullptr) {
      ++amount_;
      Node* new_node = new Node;
      new_node->value = point;
      if (parent == nullptr) {
        hash_table_[hash_value] = new_node;
      } else {
        parent->son = new_node;
      }
    }
  }

 private:
  size_t capacity_{4};
  size_t amount_{0};
  std::vector<Node*> hash_table_;

  size_t hash_function(const long arg1, const long arg2) const {//using a polynomial one here
    return static_cast<size_t>((arg1 * PRIME1 + arg2) % PRIME2 % capacity_);
  }

};

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
  HashTable a(2500000);
  int w, h, n;
  std::cin >> w >> h >> n;
  while (n) {
    int x, y;
    std::cin >> x >> y;
    a.Add(x, y, w, h);
    a.Add(x, y - 1, w, h);
    a.Add(x, y + 1, w, h);
    a.Add(x - 1, y, w, h);
    a.Add(x + 1, y, w, h);
    --n;
  }
  long long w1 = w;
  long long h1 = h;
  if (static_cast<long long>(a.Amount()) < w1 * h1) {
    std::cout << "No";
  } else {
    std::cout << "Yes";
  }
}
