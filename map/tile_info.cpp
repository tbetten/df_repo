#include "stdafx.h"
#include "tile_info.h"
#include "tinyxml2.h"
#include <variant>
#include <iostream>

namespace tile_info
{
	struct Property
	{
		std::string name;
		std::string type;
		std::string value;
	};

	using Properties = std::vector<Property>;
	using value_type = std::variant <std::string, bool>;

	void fill_attrib(Tile_info& ti, value_type val, const std::string& name)
	{
		if (name == "accessible")
		{
			ti.accessible = std::get<bool>(val);
		}
		if (name == "description")
		{
			ti.description = std::get<std::string>(val);
		}
		if (name == "transparent")
		{
			ti.transparant = std::get<bool>(val);
		}
	}

	value_type get_value(const std::string& type, tinyxml2::XMLElement* elem)
	{
		value_type res;
		if (type == "bool")
		{
			res = elem->BoolAttribute("default");
		}
		if (type == "string")
		{
			res = std::string(elem->Attribute("default"));
		}
		return res;
	}

	Tile_data load_tile_data(fs::path p)
	{
		Tile_data td{};
		tinyxml2::XMLDocument doc;
		doc.LoadFile(p.string().c_str());
		auto obj_types_elem = doc.FirstChildElement("objecttypes");
		for (auto obj_type_elem = obj_types_elem->FirstChildElement("objecttype"); obj_type_elem != nullptr; obj_type_elem = obj_type_elem->NextSiblingElement("objecttype"))
		{
			Tile_info ti{};
			ti.name = obj_type_elem->Attribute("name");
			for (auto property_elem = obj_type_elem->FirstChildElement("property"); property_elem != nullptr; property_elem = property_elem->NextSiblingElement("property"))
			{
				std::string name = property_elem->Attribute("name");
				auto val = get_value(std::string(property_elem->Attribute("type")), property_elem);
				fill_attrib(ti, val, name);
			}
			td.push_back(ti);
		}
		return td;
	}
}