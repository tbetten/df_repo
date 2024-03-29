#include <compare>
#include <iostream>

struct Fraction
{
	Fraction (unsigned int d, unsigned int n) : den { d }, nom { n }{}
	const unsigned int den;
	const unsigned int nom;

/*	std::strong_ordering operator<=> (const Fraction& other)
	{
		unsigned int a = nom * other.den;
		unsigned int b = den * other.nom;
		return a <=> b;
//		return (nom * other.den) <=> (den * other.nom);
	}*/
};

std::strong_ordering operator<=> (const Fraction& first, const Fraction& second)
{
	return  (first.den * second.nom) <=> (first.nom * second.den);
}

int main ()
{
	Fraction f1 { 1u, 2u };
	if (f1 < Fraction { 2u, 3u })
	{
		std::cout << "smaller\n";
	}
	else
	{
		std::cout << "bigger\n";
	}
}