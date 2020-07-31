#pragma once
#include <iostream>
#include <filesystem>
#include <type_traits>
#include <memory>
#include <map>
#include <SFML/Graphics.hpp>
#include "tileset.h"
#include "Tilesheet.h"
#include <functional>

namespace fs = std::filesystem;
namespace cache
{
	class Resource_base
	{
	public:
		virtual Resource_base* clone () const = 0;
		virtual ~Resource_base () {}
	};

	//using Pf = std::shared_ptr<Resource_base> (*)(fs::path);  // classic way
	using Pf = std::add_pointer_t<std::shared_ptr<Resource_base> (fs::path)>;  // C++ 14 way
	//using Pf = std::function<std::shared_ptr<Resource_base>(fs::path)>;

	struct Cache
	{
		std::map<std::string, Pf> loadfuncs;
		std::map<std::string, fs::path> paths;
		std::map<std::string, std::weak_ptr<Resource_base>> cache;

		Cache () { init (); }
		std::shared_ptr<Resource_base> get_obj (const std::string& key)
		{
			if (auto res = cache[key].lock ())
			{
				return res;
			}
			if (auto f = loadfuncs[key])
			{
				auto path = paths.at (key);
				auto res = f (path);
				cache[key] = res;
				return res;
			}
			return nullptr;
		}

		void init ();
	};



	template <typename T>
	struct Resource : Resource_base
	{
		T val;

		explicit Resource (fs::path);
		Resource* clone ()const override { return new Resource (*this); }
		static std::shared_ptr<Resource_base> load_resource (fs::path path)
		{
			//		auto file = c->paths[key];
			return std::make_shared<Resource> (path);
		}
	};

	template <typename T>
	T* get_val (Resource_base* p)
	{
		if (auto pp = dynamic_cast<Resource<T>*>(p)) return &pp->val;
		return nullptr;
	}

	using Texture_resource = Resource<sf::Texture>;

	using Tileset_resource = Resource<tileset::Tileset>;

	using Font_resource = Resource<sf::Font>;

	using Tilesheet_resource = Resource<tiled::Tilesheet>;
}