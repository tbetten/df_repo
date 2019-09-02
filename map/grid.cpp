/*#include "stdafx.h"
#include "grid.h"

namespace grid
{
	Grid::Grid (const Layout& l)
	{
		unsigned int nr_vertices = (l.gridsize.x + 1) * 2 + (l.gridsize.y + 1) * 2;
		sf::VertexArray vertices{ sf::Lines, nr_vertices };
		for (unsigned int i = 0; i <= l.gridsize.x; ++i)
		{
			vertices[i * 2].position = sf::Vector2f{ l.origin.x + i * l.tilesize.x, l.origin.y };
			vertices[i * 2 + 1].position = sf::Vector2f{ l.origin.x + i * l.tilesize.x, l.origin.y + l.gridsize.y * l.tilesize.y };
		}
		unsigned int offset = (l.gridsize.x + 1) * 2;
		for (unsigned int i = 0; i <= l.gridsize.y; ++i)
		{
			vertices[offset + (i * 2)].position = sf::Vector2f{ l.origin.x, l.origin.y + i * l.tilesize.y };
			vertices[offset + (i * 2 + 1)].position = sf::Vector2f{ l.origin.x + l.tilesize.x * l.gridsize.x, l.origin.y + i * l.tilesize.y };
		}
		m_model = vertices;
	}

	void Grid::draw (sf::RenderTarget& target, sf::RenderStates states) const
	{
		target.draw (m_model, states);
	}

	sf::Vector2f grid_to_pixel (const Layout& l, const sf::Vector2u gridcoords)
	{
		return sf::Vector2f{ l.origin.x + (gridcoords.x * l.tilesize.x) + (l.tilesize.x / 2.0f), l.origin.y + (gridcoords.y * l.tilesize.y) + (l.tilesize.y / 2.0f) };
	}
}*/