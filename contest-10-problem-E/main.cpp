#include <iostream>
#include <set>
#include <vector>

struct answer {
  int number{0};
  std::set<int> first_set;
  std::set<int> second_set;
  answer() = default;
  answer(const int arg_number,
         const std::set<int>& arg_first_set,
         const std::set<int>& arg_second_set)
      : number(arg_number), first_set(arg_first_set), second_set(arg_second_set) {}
  ~answer() = default;
};

void DFS(int vertex,
         std::set<int>& l_plus,
         std::set<int>& r_plus,
         const std::vector<std::vector<int>>& graph,
         std::vector<bool>& used,
         int m) {
  if (used[vertex]) {
    return;
  }
  used[vertex] = true;
  if (vertex < m) {
    l_plus.insert(vertex);
  } else {
    r_plus.insert(vertex);
  }
  for (int to : graph[vertex]) {
    if (!used[to]) {
      DFS(to, l_plus, r_plus, graph, used, m);
    }
  }
}

answer Solve(const std::vector<std::vector<int>>& graph, const std::vector<int>& pair, const int m, const int n) {
  std::vector<std::vector<int>> graph1(n + m);
  for (int vertex = 0; vertex < m; ++vertex) {
    for (auto to : graph[vertex]) {
      if (to == pair[vertex]) {
        graph1[to].push_back(vertex);
      } else {
        graph1[vertex].push_back(to);
      }
    }
  }
  std::set<int> l_plus, r_plus;
  std::vector<bool> used(n + m, false);
  for (int vertex = 0; vertex < m; ++vertex) {
    if (pair[vertex] == m - 1) {
      DFS(vertex, l_plus, r_plus, graph1, used, m);
    }
  }
  std::set<int> l_minus;
  for (int vertex = 0; vertex < m; ++vertex) {
    if (l_plus.find(vertex) == l_plus.end()) {
      l_minus.insert(vertex);
    }
  }
  return answer(static_cast<int>(l_minus.size() + r_plus.size()), l_minus, r_plus);
}

void Output(const answer& ans, int m) {
  std::cout << ans.number << "\n";
  std::cout << ans.first_set.size() << " ";
  for (int vertex : ans.first_set) {
    std::cout << vertex + 1 << " ";
  }
  std::cout << "\n" <<  ans.second_set.size() << " ";
  for (int vertex : ans.second_set) {
    std::cout << vertex + 1 - m << " ";
  }
}

int main() {
  int m, n;
  std::cin >> m >> n;
  std::vector<std::vector<int>> graph(m);
  for (int i = 0; i < m; ++i) {
    int k;
    std::cin >> k;
    for (int j = 0; j < k; ++j) {
      int to;
      std::cin >> to;
      to += m - 1;
      graph[i].push_back(to);
    }
  }
  std::vector<int> pair(m);
  for (int vertex = 0; vertex < m; ++vertex) {
    std::cin >> pair[vertex];
    pair[vertex] += m - 1;
  }
  answer ans = Solve(graph, pair, m, n);
  Output(ans, m);
  return 0;
}
