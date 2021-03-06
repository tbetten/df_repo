#include <iostream>
#include <filesystem>
#include <type_traits>
#include <memory>
#include <map>
#include <SFML/Graphics.hpp>

namespace fs = std::filesystem;
class Resource_base
{
public:
	virtual Resource_base* clone() const = 0;
	virtual ~Resource_base() {}
};

//using Pf = std::shared_ptr<Resource_base> (*)(fs::path);  // classic way
using Pf = std::add_pointer_t<std::shared_ptr<Resource_base> (fs::path)>;  // C++ 14 way


struct Cache
{
	std::map<std::string, Pf> loadfuncs;
	std::map<std::string, fs::path> paths;
	std::map<std::string, std::weak_ptr<Resource_base>> cache;

	std::shared_ptr<Resource_base> get_obj(const std::string& key)
	{
		if (auto res = cache[key].lock())
		{
			return res;
		}
		if (auto f = loadfuncs[key])
		{
			auto path = paths.at(key);
			auto res = f(path);
			cache[key] = res;
			return res;
		}
		return nullptr;
	}
};



template <typename T>
struct Resource : Resource_base
{
	T val;

	explicit Resource(fs::path);
	Resource* clone()const override { return new Resource(*this); }
	static std::shared_ptr<Resource_base> load_resource(fs::path path) 
	{ 
//		auto file = c->paths[key];
		return std::make_shared<Resource>(path);
	}
};

template <typename T>
T* get_val(Resource_base* p)
{
	if (auto pp = dynamic_cast<Resource<T>*>(p)) return &pp->val;
	return nullptr;
}

using Int_resource = Resource<int>;
using Image_resource = Resource<sf::Image>;

Int_resource::Resource(fs::path file)
{
	val = 42;  // dummy, most resources should load from file
}

Image_resource::Resource(fs::path file)
{
	val.loadFromFile(file.string());
}

int main()
{
	Cache c;
	c.loadfuncs["bla"] = &Int_resource::load_resource; 
	c.loadfuncs["chest"] = &Image_resource::load_resource;
	c.paths["bla"] = fs::path("d:/temp");
	c.paths["chest"] = fs::path("D:/stone_soup_tiles/Dungeon Crawl Stone Soup Full/dungeon/chest.png");
	auto p{ c.get_obj("bla") };
	if (auto sp = get_val<int>(p.get()))
	{
		std::cout << *sp << std::endl;
	}

	auto q{ c.get_obj("bla") };
	if (auto sp2 = get_val<int>(q.get()))
	{
		std::cout << *sp2 << std::endl;
	}
	
	auto i{ c.get_obj("chest") };
	if (auto chest_img = get_val<sf::Image>(i.get()))
	{
		auto size = chest_img->getSize();
		std::cout << size.x << "\t" << size.y << std::endl;
	}
}

