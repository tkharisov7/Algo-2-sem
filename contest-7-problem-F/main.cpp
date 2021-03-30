#include <iostream>
#include <vector>

std::vector<int> GetRoute(const std::vector<std::vector<int>>& dp, const std::vector<std::vector<int>>& previous,
                          int j, int all) {
  std::vector<int> arr;
  while (j > -1) {
    arr.push_back(j + 1);
    int l = j;
    j = previous[all][j];
    all -= (1 << l);
  }
  return arr;
}

std::pair<int, std::vector<int>> Solve(const std::vector<std::vector<int>>& distance, int n) {
  const int INF = 2e9 + 7;
  std::vector<std::vector<int>> dp(1 << n, std::vector<int>(n, INF));
  std::vector<std::vector<int>> previous(1 << n, std::vector<int>(n, -1));
  for (size_t i = 0; i < n; ++i) {
    dp[1 << i][i] = 0;
  }
  for (size_t mask = 0; mask < (1 << n); ++mask) {
    for (size_t i = 0; i < n; ++i) {
      if (!(mask & (1 << i))) {
        continue;
      }
      for (size_t j = 0; j < n; ++j) {
        if (i == j || !(mask & (1 << j))) {
          continue;
        }
        size_t prev_mask = mask - (1 << i);
        if (dp[prev_mask][j] + distance[i][j] < dp[mask][i]) {
          dp[mask][i] = dp[prev_mask][j] + distance[i][j];
          previous[mask][i] = j;
        }
      }
    }
  }
  int all = (1 << n) - 1;
  int ans = dp[all][0];
  int j = 0;
  for (int i = 1; i < n; ++i) {
    if (dp[all][i] < ans) {
      ans = dp[all][i];
      j = i;
    }
  }
  return {ans, GetRoute(dp, previous, j, all)};
}

int main() {
  int n;
  std::cin >> n;
  std::vector<std::vector<int>> distance(n, std::vector<int>(n, 0));
  for (size_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < n; ++j) {
      std::cin >> distance[i][j];
    }
  }
  std::pair<int, std::vector<int>> ans = Solve(distance, n);
  std::cout << ans.first << "\n";
  for (int i = n - 1; i > -1; --i) {
    std::cout << ans.second[i] << " ";
  }
  return 0;
}
