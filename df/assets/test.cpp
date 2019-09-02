#include <bitset>
#include <iostream>
#include <vector>
#include <array>

constexpr unsigned int pow2(int val)
{
int res{ 1 };
for (auto i{ 0 }; i < val; ++i)
{
 res *= 2;
}
return res;
}
enum class Bla { zero, one, two, three };

constexpr unsigned int pow2(Bla b)
{
auto j = static_cast<unsigned int>(b);
return 1 << j;
}

constexpr unsigned int to_num(Bla b)
{
return static_cast<unsigned int> (b);
}

template <std::size_t s>
constexpr unsigned int pow2_2(std::array<Bla, s> a)
{
unsigned int res{ 0 };
for (Bla b : a)
{
 auto q = static_cast<unsigned int> (b);
 auto r = 1 << q;
 res += r;
}
return res;
}

int main()
{
constexpr int pos = 3;
constexpr auto val = pow2(pos);

constexpr auto i = to_num(Bla::zero);
constexpr auto k = pow2(Bla::zero);

constexpr std::array<Bla, 3> a{ Bla::zero, Bla::two, Bla::three };
constexpr auto l = pow2_2<3>(a);
std::bitset<8> bi{ l };
std::cout << bi << "\n";

std::bitset<8> b{ k };
std::cout << b << "\n";
}