/*#pragma once
#include <string>
#include <vector>

#include "ecs_base.h"

namespace item
{
	using namespace std::string_literals;
	enum class Damage_type { cr, cut, imp, pi_, pi, spec, ranged };
	enum class Attack_type { Swing, Thrust };

	struct Item_shared_data
	{
		std::string m_name;
		std::string m_description;
		int m_price;
		float m_weight;
	};

	struct Item_shared : public Component_base
	{
		using Ptr = std::shared_ptr<Item_shared>;
		static Ptr create () { return std::make_shared<Item_shared> (); }
		//static Ptr create(std::string name = ""s, std::string description = ""s, int price = 0, float weight = 0.0f) { return std::make_shared<Item_shared>(name, description, price, weight); }
		//Item_shared(std::string name = ""s, std::string description = ""s, int price = 0, float weight = 0.0f) noexcept : m_name{ name }, m_description{ description }, m_price { price }, m_weight{ weight } {}
		void load_from_db(db::db_connection* db, const std::string& key) override;
		void add_entity (Entity_id entity) override { m_data.emplace (entity, Item_shared_data ()); }
		bool has_entity (Entity_id entity) override { return m_data.count (entity);	}
		Item_shared_data& get_data (Entity_id entity) { return m_data[entity]; }
		//void load_from_db(const std::string& key) override {}

		std::unordered_map<Entity_id, Item_shared_data> m_data;
//		std::string m_name;
//		std::string m_description;
//		int m_price;  // in farthing, the smallest unit of currency
//		float m_weight;
		const std::string m_sql = "select name, description, price, weight from item_shared where entity_key = ?";
		db::prepared_statement_ptr m_stmt = nullptr;
	};

	struct Attack
	{
		Damage_type m_damage_type;
		Attack_type m_attack_type;
		int m_bonus;
	};

	struct Reach
	{
		Reach(int min = 0, int max = 0, bool close = false, bool must_ready = false) noexcept : m_min{ min }, m_max{ max }, m_close{ close }, m_must_ready{ must_ready } {}
		int m_min;
		int m_max;
		bool m_close;
		bool m_must_ready;
	};

	struct Range
	{
		int m_min;
		int m_max;
		bool m_multiple;
	};

	struct Shot
	{
		int m_shots;
		bool m_thrown;
		int m_num_readies;
	};

	struct Projectile : public Component_base
	{
		enum class Point { None, Default, Bodkin, Cutting, Flaming };
		using Ptr = std::shared_ptr<Projectile>;
		static Ptr create(Point point_type = Point::Default, int damage_bonus = 0, int range_multiplier = 0) { return std::make_shared<Projectile>(point_type, damage_bonus, range_multiplier); }
		Projectile(Point point_type = Point::Default, int damage_bonus = 0, int range_multiplier = 0) noexcept : m_point_type{ point_type }, m_damage_bonus{ damage_bonus }, m_range_multiplier{ range_multiplier } {}
		void load_from_db(db::db_connection* db, const std::string& key) override;
		void add_entity (Entity_id entity) override;
		bool has_entity (Entity_id entity) override;
		//void load_from_db(const std::string& key) override {}

		Point m_point_type;
		Damage_type m_damage_type;
		float m_armour_divisor;
		int m_damage_bonus;      // lead bullets give +1 damage over stones
		int m_range_multiplier;  // lead bullets give double range over stones
		const std::string m_sql = "select point_type, damage_bonus, range_multiplier, damage_type, armour_divisor from projectile where entity_key = ?";
		db::prepared_statement_ptr m_stmt = nullptr;
	};

	struct Melee_weapon
	{
		enum class Handed { One, Two, Two_unreadies };
		std::string m_name;
		std::string m_skill_id;
		std::vector<Attack> m_attacks;
		Reach m_reach;
		int m_parry_bonus;
		bool m_parry_unreadies;
		int m_min_strength;
		Handed m_handed;
		bool m_pick = false;
	};

	struct Ranged_weapon
	{
		std::string m_name;
		std::string m_skill_id;
		std::vector<Projectile> m_projectiles;
		Attack m_attack;
		int m_accuracy;
		Range m_range;
		int m_shot_weight;
		Shot m_shot;
		int m_min_strength;
		int m_bulk;
	};

	struct Shield
	{
		std::string m_name;
		std::string m_skill_id;
		int m_defence_bonus;
	};

	enum class Hit_location { Skull, Eyes, Face, Neck, Torso, Arms, Hands, Groin, Legs, Feet };

	struct Armour
	{
		std::vector<Hit_location> m_hit_location;
	};
}
*/