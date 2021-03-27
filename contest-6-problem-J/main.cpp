#include <iostream>
#include <vector>

int Solve(std::vector<int> weights, int n, int s) {
  int dp[n + 1][s + 1];
  for (size_t i = 0; i < n + 1; ++i) {
    dp[i][0] = 0;
  }
  for (size_t i = 0; i < s + 1; ++i) {
    dp[0][i] = 0;
  }
  for (size_t i = 1; i < n + 1; ++i) {
    for (size_t j = 1; j < s + 1; ++j) {
      if (j >= weights[i]) {
        dp[i][j] = std::max(dp[i - 1][j], dp[i - 1][j - weights[i]] + weights[i]);
      } else {
        dp[i][j] = dp[i - 1][j];
      }
    }
  }
  return dp[n][s];
}

int main() {
  int s, n;
  std::cin >> s >> n;
  std::vector<int> weights(n + 1);
  for (int i = 1; i < n + 1; ++i) {
    std::cin >> weights[i];
  }
  std::cout << Solve(weights, n, s);
  return 0;
}
