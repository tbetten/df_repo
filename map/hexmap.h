#pragma once
#include "hexlib.h"
#include <SFML\Graphics.hpp>
#include <vector>

using namespace hexlib;

namespace map
{
	class HexGrid;
	using coords = std::vector <std::pair<sf::Vector3i, sf::Vector2f>>;
	class Map
	{
	public:
		Map(Orientation_enum orientation, int width, int height, sf::Vector2<double> hexsize, sf::Vector2<double> origin);
		Map() : Map(Orientation_enum::Flat, 0, 0, sf::Vector2<double>{ 0.0, 0.0 }, sf::Vector2<double>{ 0.0, 0.0 }) {}
		HexGrid make_grid();
		coords get_coords();
	private:
		HexMap _map;
		Layout _layout;
	};

	class HexGrid : public sf::Drawable, sf::Transformable
	{
	public:
		HexGrid(const HexMap& map, const Layout layout);
	private:
		sf::VertexArray _vertices;
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	};
}