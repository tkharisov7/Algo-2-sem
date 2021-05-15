#include <algorithm>
#include <iostream>
#include <vector>

struct edge{
  int from{0};
  int to{0};
  int weight{0};
  edge(int arg_from, int arg_to, int arg_weight) : from(arg_from), to(arg_to), weight(arg_weight){}
  edge() = default;
};

bool cmp(const edge& x, const edge& y) {
  return x.weight < y.weight;
}

int get(int vertex, std::vector<int>& parent) {
  if (parent[vertex] != vertex) {
    parent[vertex] = get(parent[vertex], parent);
  }
  return parent[vertex];
}

void uni(int vertex_1, int vertex_2, std::vector<int>& parent, std::vector<int>& rank) {
  int parent_1 = get(vertex_1, parent);
  int parent_2 = get(vertex_2, parent);
  if (parent_1 != parent_2) {
    if (rank[parent_1] == rank[parent_2]) {
      ++rank[parent_1];
    }
    if (rank[parent_1] > rank[parent_2]) {
      parent[parent_2] = parent_1;
    } else {
      parent[parent_1] = parent_2;
    }
  }
}

int Kruskal(const std::vector<edge>& graph, int n) {
  int answer = 0;
  std::vector<int> parent(n);
  std::vector<int> rank(n, 0);
  for (int i = 0; i < n; ++i) {
    parent[i] = i;
  }
  int component_number = n;
  for (int i = 0; i < graph.size() && component_number > 0; ++i) {
    edge e = graph[i];
    if (get(e.from, parent) != get(e.to, parent)) {
      answer += e.weight;
      --component_number;
      uni(e.from, e.to, parent, rank);
    }
  }
  return answer;
}

int main() {
  int n, m;
  std::cin >> n >> m;
  std::vector<edge> graph(m);
  for (int i = 0; i < m; ++i) {
    int first_vertex, second_vertex, weight;
    std::cin >> first_vertex >> second_vertex >> weight;
    graph[i] = edge(first_vertex - 1, second_vertex - 1, weight);
  }
  std::sort(graph.begin(), graph.end(), cmp);
  std::cout << Kruskal(graph, n);
  return 0;
}
