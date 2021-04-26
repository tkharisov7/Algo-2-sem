#include <iostream>
#include <set>
#include <vector>

struct edge{
  int to{0};
  int weight{0};
};

std::vector<int> dijsktra(const std::vector<std::vector<edge>>& graph, int start) {
  size_t n = graph.size();
  std::vector<int> distance(n, INT16_MAX);
  std::vector<bool> used(n, false);
  std::set<std::pair<int, int>> distance_set;
  distance[start] = 0;
  distance_set.insert({0, start});
  while (!distance_set.empty()) {
    int vertex = (*distance_set.begin()).second;
    distance_set.erase(distance_set.begin());
    used[vertex] = true;
    for (edge current_edge : graph[vertex]) {
      if (distance[vertex] + current_edge.weight < distance[current_edge.to]) {
        distance_set.erase(std::make_pair(distance[current_edge.to], current_edge.to));
        distance[current_edge.to] = distance[vertex] + current_edge.weight;
        distance_set.insert(std::make_pair(distance[current_edge.to], current_edge.to));
      }
    }
  }
  return distance;
}

int main() {
  int n, m, s, t;
  std::cin >> n >> m >> s >> t;
  std::vector<std::vector<edge>> graph(n);
  for (int i = 0; i < m; ++i) {
    int b, e, w;
    std::cin >> b >> e >> w;
    graph[b - 1].push_back({e - 1, w});
    graph[e - 1].push_back({b - 1, w});
  }
  std::vector<int> answer = dijsktra(graph, s - 1);
  if (answer[t - 1] == INT16_MAX) {
    std::cout << -1;
  } else {
    std::cout << answer[t - 1];
  }
  return 0;
}
