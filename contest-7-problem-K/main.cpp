#include <iostream>
#include <vector>

using Matr = std::vector<std::vector<int>>;

Matr operator* (const Matr& first, const Matr& second) {
  size_t n = first.size();
  size_t m = second[0].size();
  size_t k = second.size();
  Matr result(first.size(), std::vector<int> (second[0].size(), 0));
  for (size_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < m; ++j) {
      int sum = 0;
      for (int l = 0; l < k; ++l) {
        sum += first[i][l] * second[l][j];
      }
      result[i][j] = sum;
    }
  }
  return result;
}

template <typename T, typename P>
T BinaryPower(const T& argument, const P& power) {
  P copy = power;
  std::vector<T> temp;
  temp.push_back(argument);
  size_t k = 0;
  copy /= 2;
  while (copy > 0) {
    temp.push_back(temp[k] * temp[k]);
    ++k;
    copy /= 2;
  }
  copy = power;
  k = 0;
  T answer;
  bool f = true;
  while (copy > 0) {
    if (copy % 2 == 1) {
      if (f) {
        answer = temp[k];
        f = false;
      } else {
        answer = answer * temp[k];
      }
    }
    copy /= 2;
    ++k;
  }
  return answer;
}

bool IsCorrect(int first, int second, int m) {
  int a1 = first % 2;
  int a2 = (first % 4) / 2;
  int b1 = second % 2;
  int b2 = (second % 4) / 2;
  for (int i = 1; i < m; ++i) {
    if (a1 == a2 && b1 == b2 && a1 == b1) {
      return false;
    }
    a1 = a2;
    b1 = b2;
    first /= 2;
    second /= 2;
    a2 = (first % 4) / 2;
    b2 = (second % 4) / 2;
  }
  return true;
}

int Solve(int sz, int n, int m) {
  Matr A (sz, std::vector<int> (sz, 0));
  for (int i = 0; i < sz; ++i) {
    for (int j = 0; j < sz; ++j) {
      if (IsCorrect(i, j, m)) {
        A[i][j] = 1;
      }
    }
  }
  Matr res = BinaryPower<Matr, int>(A, n - 1);
  int ans = 0;
  for (int i = 0; i < sz; ++i) {
    for (int j = 0; j < sz; ++j) {
      ans += res[i][j];
    }
  }
  return ans;
}

int main() {
  int m, n;
  std::cin >> n;
  std::cin >> m;
  if (n < m) {
    std::swap(n, m);
  }
  int sz = 1;
  for (int i = 0; i < m; ++i) {
    sz *= 2;
  }
  if (n == 1) {
    std::cout << sz;
    return 0;
  }
  std::cout << Solve(sz, n, m);
  return 0;
}
