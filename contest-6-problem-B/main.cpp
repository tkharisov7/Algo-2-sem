#include <algorithm>
#include <iostream>
#include <vector>

std::pair<int, std::vector<int>> Solve(const int n, const std::vector<long long>& numbers) {
  const long long INFINITY = -(1LL << 32);
  std::vector<long long> dp(n + 5, INFINITY);
  std::vector<long long> dp_index(n + 5, -1);
  std::vector<long long> previous(n + 5, -1);
  dp[0] = numbers[0];
  dp_index[0] = -1;
  previous[1] = -1;
  dp_index[1] = 1;
  for (int i = 1; i < n; ++i) {
    auto it = std::upper_bound(dp.begin(), dp.end(), numbers[i], std::greater<>());
    int j = (it - dp.begin());
    dp[j] = numbers[i];
    dp_index[j + 1] = i + 1;
    previous[i + 1] = dp_index[j];
  }
  int i = 0;
  while (dp[i] > INFINITY) {
    ++i;
  }
  std::pair<int, std::vector<int>> return_value;
  return_value.first = i;
  i = dp_index[i];
  std::vector<int> ans;
  while (i != -1) {
    ans.push_back(i);
    i = previous[i];
  }
  std::reverse(ans.begin(), ans.end());
  return_value.second = ans;
  return return_value;
}

int main() {
  int n;
  std::cin >> n;
  std::vector<long long> numbers(n);
  for (int i = 0; i < n; ++i) {
    std::cin >> numbers[i];
  }
  std::pair<int, std::vector<int>> answer = Solve(n, numbers);
  std::cout << answer.first << "\n";
  for (int i = 0; i < answer.second.size(); ++i) {
    std::cout << answer.second[i] << " ";
  }
  return 0;
}
