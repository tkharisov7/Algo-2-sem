#include <iostream>
#include <vector>

void FindEulerCycle(int vertex,
           const std::vector<std::vector<int>>& graph,
           std::vector<std::vector<bool>>& used,
           std::vector<std::pair<int, int>>& answer,
           std::vector<int>& pointer) {
  while (pointer[vertex] != graph[vertex].size()) {
    int to = graph[vertex][pointer[vertex]];
    ++pointer[vertex];
    if (!used[vertex][to]) {
      used[vertex][to] = true;
      FindEulerCycle(to, graph, used, answer, pointer);
      answer.emplace_back(vertex, to);
    }
  }
}

int main() {
  int n, a;
  std::cin >> n >> a;
  --a;
  std::vector<std::vector<int>> graph(n);
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      int is_connected;
      std::cin >> is_connected;
      if (i == j) {
        continue;
      }
      if (is_connected == 0) {
        graph[i].push_back(j);
      }
    }
  }
  std::vector<std::vector<bool>> used(n, std::vector<bool> (n,false));
  std::vector<int> pointer(n, 0); // vector of int pointers to the first unused adjacent to [i] vertex
  std::vector<std::pair<int, int>> answer;
  FindEulerCycle(a, graph, used, answer, pointer);
  for (int i = static_cast<int>(answer.size()) - 1; i > -1; --i) {
    auto current_edge = answer[i];
    std::cout << current_edge.first + 1 << " " << current_edge.second + 1 << "\n";
  }
  return 0;
}
