#include <iostream>
#include <vector>

int Solve(const std::vector<int>& first_array, const std::vector<int>& second_array) {
  int dp[1001][1001];
  dp[0][1] = 0;
  dp[1][0] = 0;
  dp[0][0] = 0;
  size_t n = first_array.size();
  size_t m = second_array.size();
  for (size_t i = 1; i <= n; ++i) {
    for (size_t j = 1; j <= m; ++j) {
      if (first_array[i - 1] == second_array[j - 1]) {
        dp[i][j] = dp[i - 1][j - 1] + 1;
      } else {
        dp[i][j] = std::max(dp[i - 1][j], dp[i][j - 1]);
      }
    }
  }
  return dp[n][m];
}

int main() {
  int n, m;
  std::cin >> n;
  std::vector<int> first_array(n);
  for (int i = 0; i < n; ++i) {
    std::cin >> first_array[i];
  }
  std::cin >> m;
  std::vector<int> second_array(m);
  for (int i = 0; i < m; ++i) {
    std::cin >> second_array[i];
  }
  std::cout << Solve(first_array, second_array);
  return 0;
}
