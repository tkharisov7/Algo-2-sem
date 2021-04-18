#include <iostream>
#include <optional>
#include <vector>

enum class Color {
  White, Gray, Black
};

std::optional<std::pair<int, int>> DFS(int vertex, const std::vector<std::vector<int>>& graph,
                                         std::vector<Color>& used, std::vector<int>& parent) {
  used[vertex] = Color::Gray;
  std::optional<std::pair<int, int>> ans;
  for (int to : graph[vertex]) {
    if (used[to] == Color::White) {
      parent[to] = vertex;
      ans = DFS(to, graph, used, parent);
      if (ans) {
        return ans;
      }
    } else {
      if (used[to] == Color::Gray) {
        ans = {{to, vertex}};
        return ans;
      }
    }
  }
  used[vertex] = Color::Black;
  return ans;
}

void Solve(int n,
           int& start,
           int& end,
           const std::vector<std::vector<int>>& graph,
           std::vector<Color>& used,
           std::vector<int>& parent) {
  start = -1, end = -1;
  for (int i = 0; i < n; ++i) {
    auto ans = DFS(i, graph, used, parent);
    if (ans) {
      start = ans->first;
      end = ans->second;
      break;
    }
  }
}

int main() {
  int n, m;
  std::cin >> n >> m;
  std::vector<int> parent(n, -1);
  std::vector<Color> used(n, Color::White);
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
