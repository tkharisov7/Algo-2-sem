#include <iostream>
#include <vector>

int Solve(const std::vector<int>& array_one, const std::vector<int>& array_two) {
  size_t n = array_one.size();
  size_t m = array_two.size();
  std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1, 0));
  for (size_t i = 1; i <= n; ++i) {
    int optimal_value = 0;
    for (size_t j = 1; j <= m; ++j) {
      if (array_one[i - 1] != array_two[j - 1]) {
        dp[i][j] = dp[i - 1][j];
      } else {
        dp[i][j] = optimal_value + 1;
      }
      if (dp[i - 1][j] > optimal_value && array_two[j - 1] < array_one[i - 1]) {
        optimal_value = dp[i - 1][j];
      }
    }
  }
  int answer = 0;
  for (size_t j = 1; j <= m; ++j) {
    answer = std::max(dp[n][j], answer);
  }
  return answer;
}

int main() {
  int n;
  int m;
  std::cin >> n >> m;
  std::vector<int> array_one(n);
  std::vector<int> array_two(m);
  for (int i = 0; i < n; ++i) {
    std::cin >> array_one[i];
  }
  for (int i = 0; i < m; ++i) {
    std::cin >> array_two[i];
  }
  std::cout << Solve(array_one, array_two);
  return 0;
}
