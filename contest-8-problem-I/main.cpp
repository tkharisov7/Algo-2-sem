#include <iostream>
#include <vector>
#include <set>

void DFS(int vertex, int parent,
         const std::vector<std::vector<std::pair<int, int>>>& graph,
         std::vector<int>& in,
         std::vector<int>& return_time,
         std::set<int>& answer, int& timer, std::vector<bool>& used) {
  used[vertex] = true;
  ++timer;
  in[vertex] = timer;
  return_time[vertex] = in[vertex];
  int count = 0; // amount of used and adjacent to <vertex> vertices
  for (auto to_pair : graph[vertex]) {
    int to = to_pair.first;
    if (to == parent) {
      continue;
    }
    if (used[to]) {
      return_time[vertex] = std::min(return_time[vertex], in[to]);
    } else {
      DFS(to, vertex, graph, in, return_time, answer, timer, used);
      ++count;
      return_time[vertex] = std::min(return_time[vertex], return_time[to]);
      if (parent != -1) {
        if (return_time[to] >= in[vertex]) {
          answer.insert(vertex);
        }
      } else {
        if (count >= 2) {
          answer.insert(vertex);
        }
      }
    }
  }
}

int main() {
  int n, m;
  std::cin >> n >> m;
  std::vector<std::vector<std::pair<int, int>>> graph(n);
  for (int i = 0; i < m; ++i) {
    int first_vertex, second_vertex;
    std::cin >> first_vertex >> second_vertex;
    --first_vertex;
    --second_vertex;
    graph[first_vertex].push_back(std::make_pair(second_vertex, i));
    graph[second_vertex].push_back(std::make_pair(first_vertex, i));
  }
  std::vector<int> in(n);
  std::vector<int> return_time(n);
  std::vector<bool> used(n, false);
  std::set<int> answer;
  int time = 0; // current number of iterations
  for (int i = 0; i < n; ++i) {
    if (!used[i]) {
      DFS(i, -1, graph, in, return_time, answer, time, used);
    }
  }
  std::cout << answer.size() << "\n";
  for (int first_vertex : answer) {
    std::cout << first_vertex + 1 << " ";
  }
  return 0;
}
