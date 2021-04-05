#include <iostream>
#include <vector>

std::pair<bool, std::pair<int, int>> DFS(int vertex, const std::vector<std::vector<int>>& graph,
                                         std::vector<int>& used, std::vector<int>& parent) {
  used[vertex] = 1;
  std::pair<bool, std::pair<int, int>> ans;
  for (int to : graph[vertex]) {
    if (used[to] == 0) { // 0 - WHITE
      parent[to] = vertex;
      ans = DFS(to, graph, used, parent);
      if (ans.first) {
        return ans;
      }
    } else {
      if (used[to] == 1) { // 1 - GRAY
        ans.first = true;
        ans.second = {to, vertex};
        return ans;
      }
    }
  }
  used[vertex] = 2; // 2 - BLACK
  return ans;
}

void Solve(int n,
           int& start,
           int& end,
           const std::vector<std::vector<int>>& graph,
           std::vector<int>& used,
           std::vector<int>& parent) {
  start = -1, end = -1;
  for (int i = 0; i < n; ++i) {
    auto ans = DFS(i, graph, used, parent);
    if (ans.first) {
      start = ans.second.first;
      end = ans.second.second;
      break;
    }
  }
}

int main() {
  int n, m;
  std::cin >> n >> m;
  std::vector<int> parent(n, -1);
  std::vector<int> used(n, 0);
  std::vector<std::vector<int>> graph(n);
  for (int i = 0; i < m; ++i) {
    int first_vertex, second_vertex;
    std::cin >> first_vertex >> second_vertex;
    graph[first_vertex - 1].push_back(second_vertex - 1);
  }
  int start, end; //cycle start and cycle end vertices 
  Solve(n, start, end, graph, used, parent);
  if (start == -1) {
    std::cout << "NO";
  } else {
    std::cout << "YES" << "\n";
    std::vector<int> answer;
    for (int i = end; i != start; i = parent[i]) {
      answer.push_back(i);
    }
    answer.push_back(start);
    for (int i = static_cast<int>(answer.size()) - 1; i > -1; --i) {
      std::cout << answer[i] + 1 << " ";
    }
  }
  return 0;
}
