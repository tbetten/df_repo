#include "stdafx.h"
#include "hexmap.h"

namespace map
{
	Map::Map(Orientation_enum orientation, int width, int heigth, sf::Vector2<double> hexsize, sf::Vector2<double> origin)
	{
		Orientation orient = get_orientation(orientation);
		Layout layout{ orient, Point{ hexsize.x, hexsize.y }, Point{ origin.x, origin.y } };
		Layout lay2 = layout;
		_layout = layout;
		_map = hexlib::create_map(width, heigth, orient);
	}

	HexGrid Map::make_grid()
	{
		return HexGrid(_map, _layout);
	}

	coords Map::get_coords()
	{
		coords res;
		for (const auto& hex : _map)
		{
			sf::Vector3i hexcoords{ hex.q, hex.r, hex.s };
			auto center = hexlib::hex_to_pixel(_layout, hex);
			res.push_back(std::make_pair(hexcoords, sf::Vector2f{ static_cast<float>(center.x), static_cast<float>(center.y) }));
		}
		return res;
	}

	HexGrid::HexGrid(const HexMap& map, const Layout layout)
	{
		_vertices.setPrimitiveType(sf::Lines);
		_vertices.resize(map.size() * 6 * 2);
		int size = map.size() * 6 * 2;

		int i{ 0 };
		for (const auto& hex : map)
		{
			auto corners = hexlib::polygon_corners(layout, hex);
			int vertexindex = i * 6 * 2;
			for (int j = 0; j < 6; ++j)
			{
				_vertices[vertexindex].position = sf::Vector2f(corners[j].x, corners[j].y);
				_vertices[vertexindex + 1].position = sf::Vector2f(static_cast<float>(corners[(j + 1) % 6].x), static_cast<float>(corners[(j + 1) % 6].y));
				vertexindex += 2;
			}
			++i;
		}
	}

	void HexGrid::draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		states.transform *= getTransform();
		target.draw(_vertices, states);
	}
}