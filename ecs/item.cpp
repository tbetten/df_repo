#include "item.h"
#include <iostream>

void item::Item_shared::load_from_db(db::db_connection* db, const std::string& key)
{
	if (m_stmt == nullptr)
	{
		try
		{
			m_stmt = db->prepare(m_sql);
		}
		catch (std::string& msg)
		{
			std::cout << "error preparing statement " << msg << std::endl;
		}
	}
	m_stmt->bind(1, key);
	auto res = m_stmt->fetch_table();
	for (auto& row : res)
	{
		m_name = row["name"].string_value;
		m_description = row["description"].string_value;
		m_price = row["price"].integer_value;
		m_weight = row["weight"].double_value;
	}
}

void item::Projectile::load_from_db(db::db_connection* db, const std::string& key)
{
	if (m_stmt == nullptr)
	{
		m_stmt = db->prepare(m_sql);
	}
	m_stmt->bind(1, key);
	auto res = m_stmt->fetch_table();
	for (auto& row : res)
	{
		m_damage_bonus = row["damage_bonus"].integer_value;
		m_range_multiplier = row["range_multiplier"].integer_value;
		m_damage_type = static_cast<Damage_type> (row["damage_type"].integer_value);
		m_armour_divisor = row["armour_divisor"].double_value;
	}
}