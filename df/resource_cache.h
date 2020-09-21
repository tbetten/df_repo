#pragma once
#include <iostream>
#include <filesystem>
#include <type_traits>
#include <array>
#include <memory>
#include <map>
#include <variant>
#include <cassert>
#include <SFML/Graphics.hpp>
#include "Tilesheet.h"
#include <functional>

namespace fs = std::filesystem;
namespace cache
{
	using Resource = std::variant < std::monostate, std::weak_ptr<sf::Texture>, std::weak_ptr<sf::Font>, std::weak_ptr<tiled::Tilesheet>>;

	struct Cache_entry
	{
		Cache_entry (std::string a_key, std::string a_path) : key { std::move (a_key) }, path { std::move (a_path) }, data { std::monostate{} }{}
		std::string key;
		std::string path;
		Resource data;
	};

	struct Load_visitor
	{
		void operator()(std::monostate m) {}
		void operator()(std::weak_ptr<sf::Font> f) { f.lock ()->loadFromFile (m_path); }
		void operator()(std::weak_ptr<sf::Texture> t) { t.lock ()->loadFromFile (m_path); }
		void operator()(std::weak_ptr<tiled::Tilesheet> t) { t.lock ()->load (m_path, false); }

		std::string m_path;
	};

	struct Cache
	{
		Cache ();
		template <typename T>
		std::shared_ptr<T> get (const std::string& key)
		{
			auto itr = std::ranges::find_if (m_cache, [&key] (const Cache_entry& ce){return ce.key == key; });
			assert (itr != std::end (m_cache));

			Cache_entry& ce = *itr;
			auto& var = ce.data;
			if (std::holds_alternative<std::monostate> (var))
			{
				var = std::weak_ptr<T> ();
			}
			auto& wp = std::get<std::weak_ptr<T>> (var);
			if (auto sp = wp.lock ()) return sp;
			auto temp = std::make_shared<T> ();
			var = temp;
			m_lv.m_path = ce.path;
			std::visit (m_lv, var);
			return temp;
		}
	private:
		bool load_cache_entry (const std::string& key);

		std::vector<Cache_entry> m_cache;
		Load_visitor m_lv;
	};
}