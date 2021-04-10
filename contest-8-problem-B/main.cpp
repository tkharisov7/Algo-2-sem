#include <iostream>
#include <vector>

void DFS(int vertex, const std::vector<std::vector<int>>& graph, std::vector<bool>& used, int& timer,
         std::vector<int>& in, std::vector<int>& out) {
  ++timer;
  in[vertex] = timer;
  used[vertex] = true;
  for (int to : graph[vertex]) {
    if (!used[to]) {
      DFS(to, graph, used, timer, in, out);
    }
  }
  ++timer;
  out[vertex] = timer;
}

int main() {
  int n;
  std::cin >> n;
  int j;
  std::vector<std::vector<int>> graph(n);
  std::vector<int> in(n), out(n);
  for (int i = 0; i < n; ++i) {
    int is_connected;
    std::cin >> is_connected;
    if (is_connected == 0) {
      j = i;
    } else {
      graph[is_connected - 1].push_back(i);
    }
  }
  std::vector<bool> used(n, false);
  int timer = 0;
  DFS(j, graph, used, timer, in, out);
  int m;
  std::cin >> m;
  for (int i = 0; i < m; ++i) {
    int first_vertex, second_vertex;
    std::cin >> first_vertex >> second_vertex;
    --first_vertex;
    --second_vertex;
    if (in[first_vertex] < in[second_vertex] && out[first_vertex] > out[second_vertex]) {
      std::cout << 1 << "\n";
    } else {
      std::cout << 0 << "\n";
    }
  }
  return 0;
}
