#include <iostream>
#include <vector>
#include <set>

const int MX = INT32_MAX - 1'000;

struct edge{
  int to;
  int weight;
  edge(int to_arg, int weight_arg): to(to_arg), weight(weight_arg){}
};

int Prim(const std::vector<std::vector<edge>>& graph) {
  int n = static_cast<int>(graph.size());
  std::set<std::pair<int, int>> vertices_set;
  int answer = 0;
  std::vector<int> edge_weight(n, 0);
  std::vector<int> dist(n, MX);
  dist[0] = 0;
  for (int i = 0; i < n; ++i) {
    vertices_set.insert(std::make_pair(dist[i], i));
  }
  while (!vertices_set.empty()) {
    int cur = vertices_set.begin()->second;
    vertices_set.erase(vertices_set.begin());
    dist[cur] = -1;
    for (edge e : graph[cur]) {
      if (dist[e.to] != -1 && e.weight < dist[e.to]) {
        edge_weight[e.to] = e.weight;
        vertices_set.erase({dist[e.to], e.to});
        vertices_set.insert({e.weight, e.to});
        dist[e.to] = e.weight;
      }
    }
  }
  for (int i = 0; i < n; ++i) {
    answer += edge_weight[i];
  }
  return answer;
}

int main() {
  int n, m;
  std::cin >> n >> m;
  std::vector<std::vector<edge>> graph(n);
  for (int i = 0; i < m; ++i) {
    int first_vertex, second_vertex, w;
    std::cin >> first_vertex >> second_vertex >> w;
    graph[first_vertex - 1].push_back({second_vertex - 1, w});
    graph[second_vertex - 1].push_back({first_vertex - 1, w});
  }
  std::cout << Prim(graph);
  return 0;
}
