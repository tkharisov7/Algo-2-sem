#include <iostream>
#include <vector>
#include <string>

class BigInteger {
 public:

  ~BigInteger() = default;

  BigInteger() : BigInteger(0) {}

  BigInteger(long value) {
    if (value >= 0) {
      positive_ = true;
    } else {
      value *= -1;
      positive_ = false;
    }
    while (value > 0) {
      numbers_.push_back(value % base_);
      value /= base_;
    }
    if (numbers_.empty()) {
      numbers_.push_back(0);
    }
  }

  BigInteger(const BigInteger& value) : numbers_(value.numbers_), positive_(value.positive_) {}

  BigInteger(const std::string& value) { //+
    long starting_digit = 0;
    if (value[0] == '-') {
      starting_digit = 1;
      positive_ = false;
    } else {
      starting_digit = 0;
      positive_ = true;
    }
    long long multiplier = 1;
    long digit_number = 0;
    long long dig = 0;
    numbers_.resize((value.size() - starting_digit + base_number_ - 1) / base_number_);
    for (long i = static_cast<long>(value.size()) - 1; i >= starting_digit; --i) {
      dig += (value[i] - '0') * multiplier;
      multiplier *= 10;
      if (multiplier == base_ || i == starting_digit) {
        multiplier = 1;
        numbers_[digit_number] = dig;
        dig = 0;
        ++digit_number;
      }
    }
    normalize();
  }

  BigInteger& operator=(const long value) {
    *this = std::to_string(value);
    return *this;
  }

  BigInteger& operator=(const std::string& value) {
    *this = BigInteger(value);
    return *this;
  }

  void swap(BigInteger val) {
    std::swap(numbers_, val.numbers_);
    std::swap(positive_, val.positive_);
  }

  BigInteger& operator=(const BigInteger& val) {
    swap(val);
    return *this;
  }

  void normalize() {
    long i = static_cast<long>(numbers_.size()) - 1;
    while (i > 0 && numbers_[i] == 0) {
      --i;
      numbers_.pop_back();
    }
    if (numbers_.size() == 1 && numbers_[0] == 0) {
      positive_ = true;
    }
  }

  BigInteger operator-() const;

  BigInteger& operator++() {
    *this += 1;
    return *this;
  }

  BigInteger& operator--() {
    *this -= 1;
    return *this;
  }

  BigInteger operator++(int) {
    BigInteger copy = *this;
    ++*this;
    return copy;
  }

  BigInteger operator--(int) {
    BigInteger copy = *this;
    --*this;
    return copy;
  }

  BigInteger& operator+=(long value) {
    if (value < 0) {
      *this -= -value;
      return *this;
    }
    long carry = value;
    long temp;
    for (long i = 0; i < static_cast<long>(numbers_.size()) && value > 0; ++i) {
      temp = (carry + numbers_[i]) % base_;
      carry = (carry + numbers_[i]) / base_;
      numbers_[i] = temp;
    }
    if (carry > 0) {
      numbers_.push_back(carry);
    }
    normalize();
    return *this;
  }

  BigInteger& operator+=(const BigInteger& value);

  BigInteger& operator-=(const long value) {
    BigInteger temp(value);
    *this -= temp;
    return *this;
  }

  BigInteger& operator-=(const BigInteger& value);

  BigInteger& operator*=(const int value) {
    if (value == 0) {
      *this = 0;
      return *this;
    }
    if ((value >= 0) == positive_) {
      positive_ = true;
    } else {
      positive_ = false;
    }
    long carry = 0;
    long val = std::abs(value);
    for (long long& number : numbers_) {
      long copy = val * static_cast<long>(number);
      copy += carry;
      carry = copy / static_cast<long>(base_);
      number = copy % static_cast<long>(base_);
    }
    if (carry > 0) {
      numbers_.push_back(carry);
    }
    normalize();
    return *this;
  }

  void add_nulls(const long null_amount) {
    if (null_amount > 0) {
      long t = numbers_.size();
      numbers_.resize(t + null_amount);
      for (long i = static_cast<long>(numbers_.size()) - 1; i > -1; --i) {
        numbers_[i] = numbers_[i - null_amount];
      }
      for (long i = 0; i < null_amount; ++i) {
        numbers_[i] = 0;
      }
    }
  }

  void del_last() {
    for (size_t i = 0; i < numbers_.size() - 1; ++i) {
      numbers_[i] = numbers_[i + 1];
    }
    numbers_.pop_back();
  }

  BigInteger& operator*=(const BigInteger& value) {
    bool positiv;
    if (positive_ != value.positive_) {
      positiv = false;
    } else {
      positiv = true;
    }
    BigInteger copy;
    BigInteger sum = 0;
    for (size_t i = 0; i < value.numbers_.size(); ++i) {
      copy = *this;
      copy *= value.numbers_[i];
      copy.add_nulls(i);
      copy.normalize();
      sum += copy;
    }
    *this = sum;
    this->positive_ = positiv;
    normalize();
    return *this;
  }

  BigInteger abs() const {
    BigInteger copy = *this;
    copy.positive_ = true;
    return copy;
  }

  BigInteger& operator/=(const BigInteger& value);

  BigInteger& operator%=(const int value) {
    BigInteger copy = value;
    *this %= copy;
    return *this;
  }

  BigInteger& operator%=(const BigInteger& value);

  void Clear() {
    numbers_.clear();
    positive_ = true;
  }

  std::string toString() const;

  explicit operator bool() const;

  explicit operator double() const {
    double res = 0;
    double multiplier = 1;
    for (long digit : numbers_) {
      res += digit * multiplier;
      multiplier *= base_;
    }
    if (!positive_) {
      res *= -1;
    }
    return res;
  }

  void div_2() {
    for (size_t i = 0; i < numbers_.size(); ++i) {
      if (numbers_[i] & 1) {
        if (i > 0) {
          numbers_[i - 1] += base_ / 2;
        }
      }
      numbers_[i] /= 2;
    }
    for (size_t i = 0; i < numbers_.size(); ++i) {
      if (numbers_[i] > base_) {
        numbers_[i + 1] += numbers_[i] / base_;
        numbers_[i] %= base_;
      }
    }
    normalize();
  }

 private:
  std::vector<long long> numbers_;
  bool positive_{true};
  const long long base_ = 10000;
  const int base_number_ = 4;

  friend BigInteger operator+(const BigInteger& number_first, const BigInteger& number_second);

  friend BigInteger operator*(const BigInteger& number_first, const BigInteger& number_second);

  friend std::ostream& operator<<(std::ostream& out, const BigInteger& value);

  friend std::ostream& operator<<(std::ostream& out, const BigInteger& value);

  friend bool operator<(const BigInteger& first_number, const BigInteger& second_number);

  friend class Rational;

  std::pair<BigInteger, BigInteger> div_and_mod(const BigInteger& value);
};


bool operator<(const BigInteger& first_number, const BigInteger& second_number) { //+
  if (first_number.positive_ && !second_number.positive_) {
    return false;
  }
  if (!first_number.positive_ && second_number.positive_) {
    return true;
  }
  bool same_sign;
  if (first_number.positive_ && second_number.positive_) {
    same_sign = true;
  } else {
    same_sign = false;
  }
  if (first_number.numbers_.size() < second_number.numbers_.size()) {
    return same_sign;
  }
  if (first_number.numbers_.size() > second_number.numbers_.size()) {
    return !same_sign;
  }
  if (first_number.numbers_.size() == second_number.numbers_.size()) {
    for (long i = static_cast<long>(first_number.numbers_.size()) - 1; i > -1; --i) {
      if (first_number.numbers_[i] > second_number.numbers_[i]) {
        return !same_sign;
      }
      if (first_number.numbers_[i] < second_number.numbers_[i]) {
        return same_sign;
      }
    }
    return false;
  }
  return true;
}

bool operator==(const BigInteger& first_number, const BigInteger& second_number) {//+
  return !((first_number < second_number) || (second_number < first_number));
}

bool operator>(const BigInteger& first_number, const BigInteger& second_number) {//+
  return (second_number < first_number);
}

bool operator<=(const BigInteger& first_number, const BigInteger& second_number) {//+
  return !(first_number > second_number);
}

bool operator>=(const BigInteger& first_number, const BigInteger& second_number) {//+
  return !(first_number < second_number);
}

bool operator!=(const BigInteger& first_number, const BigInteger& second_number) {//+
  return !(first_number == second_number);
}

BigInteger BigInteger::operator-() const { //+
  BigInteger ret_val = *this;
  if (ret_val != 0) {
    ret_val.positive_ = !positive_;
  }
  return ret_val;
}

BigInteger operator+(const BigInteger& number_first, const BigInteger& number_second) {
  BigInteger return_value;
  return_value = number_first;
  return_value += number_second;
  return return_value;
}

BigInteger operator-(const BigInteger& number_first, const BigInteger& number_second) {
  BigInteger return_value = number_first;
  return_value -= number_second;
  return return_value;
}

BigInteger operator*(const BigInteger& number_first, const BigInteger& number_second) {
  BigInteger return_value;
  return_value = number_first;
  return_value *= number_second;
  return return_value;
}

BigInteger operator/(const BigInteger& number_first, const BigInteger& number_second) {
  BigInteger return_value = number_first;
  return_value /= number_second;
  return return_value;
}

BigInteger operator%(const BigInteger& number_first, const BigInteger& number_second) {
  BigInteger return_value = number_first;
  return_value %= number_second;
  return return_value;
}


BigInteger& BigInteger::operator+=(const BigInteger& value) {//++
  if (value.positive_ != positive_) {
    if (positive_) {
      *this -= -value;
    } else {
      *this = value + *this;
    }
    return *this;
  }
  long carry = 0;
  numbers_.resize(std::max(numbers_.size(), value.numbers_.size()));
  size_t i;
  for (i = 0; i < std::min(numbers_.size(), value.numbers_.size()); ++i) {
    carry += numbers_[i] + value.numbers_[i];
    numbers_[i] = carry % base_;
    carry /= base_;
  }
  while (carry > 0) {
    if (i >= numbers_.size()) {
      numbers_.push_back(0);
    }
    carry += numbers_[i];
    numbers_[i] = carry % base_;
    carry /= base_;
    ++i;
  }
  normalize();
  return *this;
}

BigInteger& BigInteger::operator-=(const BigInteger& value) {//+
  if (!value.positive_) {
    *this += -value;
    return *this;
  }
  if (positive_) {
    if (*this >= value) {
      int carry = 0;
      size_t i = 0;
      for (i = 0; i < std::min(numbers_.size(), value.numbers_.size()); ++i) {
        if (numbers_[i] < value.numbers_[i] + carry) {
          numbers_[i] += base_;
          numbers_[i] -= value.numbers_[i];
          numbers_[i] -= carry;
          carry = 1;
        } else {
          numbers_[i] -= value.numbers_[i];
          numbers_[i] -= carry;
          carry = 0;
        }
      }
      while (carry > 0) {
        if (i >= numbers_.size()) {
          numbers_.push_back(0);
        }
        if (numbers_[i] < carry) {
          numbers_[i] += base_;
          numbers_[i] -= carry;
          carry = 1;
        } else {
          numbers_[i] -= carry;
          carry = 0;
        }
        ++i;
      }
    } else {
      *this = -(value - *this);
    }
  } else {
    positive_ = true;
    *this += value;
    positive_ = false;
  }
  normalize();
  return *this;
}

BigInteger& BigInteger::operator/=(const BigInteger& value) {
  *this = div_and_mod(value).first;
  return *this;
}

BigInteger& BigInteger::operator%=(const BigInteger& value) {
  *this = div_and_mod(value).second;
  return *this;
}

std::pair<BigInteger, BigInteger> BigInteger::div_and_mod(const BigInteger& value) {
  bool mod_positive = positive_;
  if (positive_ == value.positive_) {
    positive_ = true;
  } else {
    positive_ = false;
  }
  BigInteger copy = value;
  copy.positive_ = true;
  BigInteger res;
  res.positive_ = positive_;
  positive_ = true;
  if (*this < copy) {
    positive_ = mod_positive;
    return {0, *this};
  }
  res.numbers_.resize(numbers_.size() - value.numbers_.size() + 1);
  copy.add_nulls(static_cast<long>(numbers_.size() - value.numbers_.size()));
  for (long i = static_cast<long>(numbers_.size() - value.numbers_.size()); i > -1; --i) {
    long left = 0;
    long right = base_;
    while (right > left + 1) {
      long m = (right + left) / 2;
      if (*this >= copy * m) {
        left = m;
      } else {
        right = m;
      }
    }
    positive_ = mod_positive;
    res.numbers_[i] = left;
    *this -= left * copy;
    copy.del_last();
  }
  normalize();
  res.normalize();
  return {res, *this};
}

int digits_count(long value) {
  int ret_val = 0;
  if (value == 0) {
    return 1;
  }
  while (value > 0) {
    ++ret_val;
    value /= 10;
  }
  return ret_val;
}

std::string BigInteger::toString() const {
  std::string ret_val;
  if (numbers_.empty()) {
    return ret_val;
  }
  if (!positive_) {
    ret_val += "-";
  }
  ret_val += std::to_string(numbers_[numbers_.size() - 1]);
  for (long i = static_cast<long>(numbers_.size()) - 2; i > -1; --i) {
    for (int j = 0; j < base_number_ - digits_count(numbers_[i]); ++j) {
      ret_val += "0";
    }
    ret_val += std::to_string(numbers_[i]);
  }
  return ret_val;
}

std::ostream& operator<<(std::ostream& out, const BigInteger& value) {
  out << value.toString();
  return out;
}

std::istream& operator>>(std::istream& in, BigInteger& value) {
  value.Clear();
  std::string input;
  in >> input;
  value = input;
  return in;
}

BigInteger::operator bool() const {
  return (*this != 0);
}

using Matr = std::vector<std::vector<int>>;

const int mod = 999999937;

Matr operator* (const Matr& first, const Matr& second) {
  size_t n = first.size();
  size_t m = second[0].size();
  size_t k = second.size();
  Matr result(first.size(), std::vector<int> (second[0].size(), 0));
  for (size_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < m; ++j) {
      int sum = 0;
      for (int l = 0; l < k; ++l) {
        sum += (static_cast<long long>(first[i][l]) * second[l][j]) % mod;
        sum %= mod;
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
  copy.div_2();
  while (copy > 0) {
    temp.push_back(temp[k] * temp[k]);
    ++k;
    copy.div_2();
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
    copy.div_2();
    ++k;
  }
  return answer;
}

int Solve(const BigInteger& n) {
  Matr A(5, std::vector<int>(5, 1));
  for (int i = 0; i < 5; ++i) {
    for (int j = 0; j < 5; ++j) {
      if ((i == 2 && j == 3) || (i == 2 && j == 4) || (i == 4 && j == 3) || (i == 4 && j == 4)) {
        A[i][j] = 0;
      }
    }
  }
  Matr res = BinaryPower<Matr, BigInteger>(A, n - 1);
  int ans = 0;
  for (int i = 0; i < 5; ++i) {
    for (int j = 0; j < 5; ++j) {
      ans += res[i][j];
      ans %= mod;
    }
  }
  return ans;
}

int main() {
  BigInteger n;
  std::cin >> n;
  while (n != 0) {
    if (n == 1) {
      std::cout << 5 << "\n";
    } else {
      std::cout << Solve(n) << "\n";
    }
    std::cin >> n;
  }
  return 0;
}
