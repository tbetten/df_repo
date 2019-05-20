
#pragma once
#include <iostream>
#include <string>
#include <map>
#include <exception>
#include <vector>


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