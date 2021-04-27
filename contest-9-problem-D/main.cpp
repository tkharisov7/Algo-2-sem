#include <iostream>
#include <vector>
#include <climits>

const long long DELTA = 1e16;

struct edge {
  edge(const long long arg_from, const long long arg_weight) : from(arg_from), weight(arg_weight) {}
  long long from;
  long long weight;
};

void dfs(const std::vector<std::vector<long long>>& graph, long long start, std::vector<bool>& used) {
  used[start] = true;
  for (long long to : graph[start]) {
    if (!used[to]) {
      dfs(graph, to, used);
    }
  }
}

std::vector<long long> FordBellman(const std::vector<std::vector<edge>>& graph, long long start) {
  size_t n = graph.size();
  std::vector<long long> distance(n, LLONG_MAX - DELTA);
  distance[start] = 0;
  for (long long i = 0; i < n; ++i) {
    for (long long v = 0; v < n; ++v) {
      for (edge edge : graph[v]) {
        if (distance[edge.from] < LLONG_MAX - DELTA) {
          distance[v] = std::min(distance[v], std::max(LLONG_MIN + DELTA, distance[edge.from] + edge.weight));
        }
      }
    }
  }
  std::vector<bool> used(n, false);
  std::vector<std::vector<long long>> graph1(n);
  for (long long i = 0; i < n; ++i) {
    for (edge edge : graph[i]) {
      graph1[edge.from].push_back(i);
    }
  }
  for (long long v = 0; v < n; ++v) {
    for (edge edge : graph[v]) {
      if (distance[edge.from] < LLONG_MAX - DELTA) {
        if (distance[v] > distance[edge.from] + edge.weight) {
          dfs(graph1, v, used);
        }
      }
    }
  }
  for (long long i = 0; i < n; ++i) {
    if (used[i]) {
      distance[i] = LLONG_MIN;
    }
  }
  return distance;
}

int main() {
  long long n, m, s;
  std::cin >> n >> m >> s;
  --s;
  std::vector<std::vector<edge>> graph(n);
  for (long long i = 0; i < m; ++i) {
    long long a, b;
    long long w;
    std::cin >> a >> b >> w;
    graph[b - 1].push_back(edge(a - 1, w));
  }
  std::vector<long long> answer = FordBellman(graph, s);
  for (long long i = 0; i < n; ++i) {
    switch (answer[i]) {
      case (LLONG_MAX - DELTA): {
        std::cout << "*\n";
        break;
      }
      case LLONG_MIN: {
        std::cout << "-\n";
        break;
      }
      default: {
        std::cout << answer[i] << "\n";
        break;
      }
    }
  }
  return 0;
}
