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
//		m_name = std::get<std::string>(row["name"]);
//		m_description = std::get<std::string>(row["description"]);
//		m_price = std::get<int>(row["price"]);
//		m_weight = static_cast<float> (std::get<double>(row["weight"]));
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
		m_damage_bonus = std::get<int>(row["damage_bonus"]);
		m_range_multiplier = std::get<int>(row["range_multiplier"]);
		m_damage_type = static_cast<Damage_type> (std::get<int>(row["damage_type"]));
		m_armour_divisor = static_cast<float> (std::get<double>(row["armour_divisor"]));
	}
}