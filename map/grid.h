#pragma once

#include "SFML/Graphics.hpp"
#include <unordered_map>
#include "tilemap.h"

namespace grid
{
	class Grid : public sf::Drawable
	{
	public:
		Grid (const Layout& l);
		~Grid () = default;
		void draw (sf::RenderTarget& target, sf::RenderStates states) const override;
	private:
		sf::VertexArray m_model;
	};

	sf::Vector2f grid_to_pixel (const Layout& l, const sf::Vector2u gridcoords);
}