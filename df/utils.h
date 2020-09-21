
#pragma once
#include <iostream>
#include <string>
#include <map>
#include <exception>
#include <vector>
#include <SFGUI/Widget.hpp>
#include <SFGUI/Label.hpp>

template <typename T>
class String_bimap
{
	static_assert (!std::is_same<T, std::string>::value, "Should map to something different from std::string");
public:
	explicit String_bimap(std::size_t num_values, std::size_t num_specials = 0) : m_strings(num_values + num_specials), m_mappings{}, m_num_specials{ num_specials } {}
	void insert(T t, const std::string& string)
	{
		m_strings[static_cast<int>(t) + m_num_specials] = string;
		m_mappings[string] = t;
	}
	const std::string& at(T t) const
	{
		std::size_t index = static_cast<std::size_t> (t + m_num_specials);
		if (index >= m_strings.size() + m_num_specials)
		{
			throw std::out_of_range("no match found");
		}
		else
		{
			return m_strings[index];
		}
	}
	T at(const std::string& string) const
	{
		auto itr = m_mappings.find(string);
		if (itr == m_mappings.end())
		{
			throw std::out_of_range("no match found");
		}
		else
		{
			return itr->second;
		}
	}

	bool contains(const std::string& string) const
	{
		auto itr = m_mappings.find(string);
		return itr != m_mappings.end();
	}

	std::size_t size() const
	{
		return m_mappings.size();
	}

private:
	std::vector<std::string> m_strings;
	std::map<std::string, T> m_mappings;
	std::size_t m_num_specials;
};

bool iequal(std::string_view s1, std::string_view s2);

std::string to_lower(std::string s);

/*
Conversion from tuple to struct. First template is helper, use 
tuple_to_struct<S, Tup>(tup);
*/
template <typename S, std::size_t... Is, typename Tup>
S tuple_to_struct(std::index_sequence<Is...>, Tup&& tup)
{
	return { std::get<Is>(std::forward<Tup>(tup))... };
}

template <typename S, typename Tup>
S tuple_to_struct(Tup&& tup)
{
	using T = std::remove_reference_t<Tup>;
	return tuple_to_struct(std::make_index_sequence < std::tuple_size<T>{} > {}, std::forward<Tup>(tup));
}

constexpr int square (int num)
{
	return num * num;
}

constexpr double square (double num)
{
	return num * num;
}

namespace detail
{
	constexpr double sqrt_help (double x, double curr, double prev)
	{
		return curr == prev ? curr : sqrt_help (x, 0.5 * (curr + x / curr), curr);
	}
}

constexpr double my_sqrt (double x)
{
	return x >= 0 && x < std::numeric_limits<double>::infinity ()
		? detail::sqrt_help (x, x, 0)
		: std::numeric_limits<double>::quiet_NaN ();
}

template <typename T = sfg::Label>
std::shared_ptr<T> find_widget (const std::string& id)
{
	return std::dynamic_pointer_cast<T>(sfg::Widget::GetWidgetById (id));
}
