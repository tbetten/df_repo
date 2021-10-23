#include "drawable.h"
#include "component.h"
#include "components.h"
#include "resource_cache.h"
#include "facing_payload.h"
#include "Tilesheet.h"
#pragma warning (push)
#pragma warning (disable : 26812)
#include "tinyxml2.h"
#pragma warning (pop)

Drawable::Drawable() : locations{"base_icon", "torso", "beard", "feet", "facing_indicator"}
{
	composed_icon = std::make_unique<sf::RenderTexture>();
	composed_icon->create(32, 32);
	for (auto location : locations)
	{
		icon_parts[location] = Icon{};
	}
}
void Drawable::reset () {}

void Vertex_array::add_tile(int tile_index, sf::FloatRect coords, sf::FloatRect texture_coords)
{
	m_index[tile_index] = Vertex_coords{ coords, texture_coords };
}

void Vertex_array::remove_tile(int tile_index)
{
	m_index.erase(tile_index);
}

void Vertex_array::rebuild()
{
	m_array.resize(m_index.size() * 4);
	std::for_each(std::cbegin(m_index), std::cend(m_index), [this](std::pair<int, Vertex_coords> c_pair)
		{
			Vertex_coords& c = c_pair.second;
			m_array.append(sf::Vertex{ sf::Vector2f{c.coords.left, c.coords.top}, sf::Vector2f{c.texture_coords.left, c.texture_coords.top} });
			m_array.append(sf::Vertex{ sf::Vector2f{c.coords.left + c.coords.width, c.coords.top}, sf::Vector2f{c.texture_coords.left + c.texture_coords.width, c.texture_coords.top} });
			m_array.append(sf::Vertex{ sf::Vector2f{c.coords.left + c.coords.width, c.coords.top + c.coords.height}, sf::Vector2f{c.texture_coords.left + c.texture_coords.width, c.texture_coords.top + c.texture_coords.height} });
			m_array.append(sf::Vertex{ sf::Vector2f{c.coords.left, c.coords.top + c.coords.height}, sf::Vector2f{c.texture_coords.left, c.texture_coords.top + c.texture_coords.height} });
		});
}

void Vertex_array::add_texture (std::shared_ptr<tiled::Tilesheet> sheet)
{
	//m_res = res;
	m_sheet = sheet;
}

void Vertex_array::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	states.texture = &m_sheet->m_texture;
	target.draw(m_array, states);
}

void Drawable::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	switch (type)
	{
	case Type::Entity:
		target.draw(sprite);
		break;
	case Type::Vertex_array:
		target.draw(m_array);
		break;
	}
}

void fill_icon(Icon& icon, std::string key, cache::Cache* cache, const sf::IntRect& area)
{
	icon.key = std::move(key);
	icon.tilesheet = cache->get<tiled::Tilesheet> (icon.key);
	//icon.resource_ptr = cache->get_obj(icon.key);
	//icon.texture = cache::get_val<sf::Texture>(icon.resource_ptr.get());
	//icon.tilesheet = cache::get<tiled::Tilesheet>(icon.resource_ptr);
	icon.texture = std::make_shared<sf::Texture>(icon.tilesheet->m_texture);
	icon.sprite.setTexture(icon.tilesheet->m_texture);
	icon.sprite.setTextureRect(area);
}

void fill_icon(Icon& icon, std::string key, cache::Cache* cache, const int tile_index)
{
	icon.key = std::move(key);
	icon.tilesheet = cache->get<tiled::Tilesheet> (icon.key);
//	icon.resource_ptr = cache->get_obj(icon.key);
//	icon.tilesheet = cache::get<tiled::Tilesheet>(icon.resource_ptr);
	icon.texture = std::make_shared<sf::Texture> (icon.tilesheet->m_texture);
	icon.sprite.setTexture(icon.tilesheet->m_texture);
	icon.sprite.setTextureRect(icon.tilesheet->get_rect(tile_index));
}

void clear_icon(Icon& icon)
{
	icon.key = "";
	icon.tilesheet.reset();
	icon.texture = nullptr;
}

void init_drawable(ecs::Entity_manager* mgr, ecs::Entity_id entity, const std::string& key, cache::Cache* cache, const sf::IntRect& rect)
{
	auto data = mgr->get_data<ecs::Component<Drawable>> (ecs::Component_type::Drawable, entity);
	auto& base_icon = data->icon_parts.at("base_icon");
	fill_icon(base_icon, key, cache, rect);
	mgr->get_messenger()->notify("icon_changed", entity);
	auto& tex = data->composed_icon->getTexture();
	data->sprite.setTexture(tex);
	data->sprite.setOrigin(static_cast<sf::Vector2f> (tex.getSize() / 2u));
	//data->sprite.setTextureRect(rect);
}

void init_drawable(ecs::Entity_manager* mgr, ecs::Entity_id entity, const std::string& tilesheet, cache::Cache* cache, int tile_index)
{
	auto data = mgr->get_data<ecs::Component<Drawable>>(ecs::Component_type::Drawable, entity);
	auto& base_icon = data->icon_parts.at("base_icon");
	fill_icon(base_icon, tilesheet, cache, tile_index);
	mgr->get_messenger()->notify("icon_changed", entity);
	auto& tex = data->composed_icon->getTexture();
	data->sprite.setTexture(tex);
	data->sprite.setOrigin(static_cast<sf::Vector2f> (tex.getSize() / 2u));
}

void change_icon_index(ecs::Entity_manager* mgr, const std::string& location, ecs::Entity_id entity, int tile_index)
{
	auto data = mgr->get_data<ecs::Component<Drawable>>(ecs::Component_type::Drawable, entity);
	auto& icon_part = data->icon_parts.at(location);
	
	icon_part.sprite.setTextureRect(icon_part.tilesheet->get_rect(tile_index));
	mgr->get_messenger()->notify("icon_changed", entity);
}

void change_icon_area(ecs::Entity_manager* mgr, const std::string& location, ecs::Entity_id entity, sf::IntRect area)
{
	auto data = mgr->get_data<ecs::Component<Drawable>>(ecs::Component_type::Drawable, entity);
	auto& icon_part = data->icon_parts.at(location);

	icon_part.sprite.setTextureRect(area);
	mgr->get_messenger()->notify("icon_changed", entity);
}

void fill_icon_part(ecs::Entity_manager* mgr, cache::Cache* cache, std::string tilesheet, int tile_index, const std::string& location, ecs::Entity_id entity)
{
	auto data = mgr->get_data<ecs::Component<Drawable>>(ecs::Component_type::Drawable, entity);
	auto& icon_part = data->icon_parts.at(location);
	fill_icon(icon_part, tilesheet, cache, tile_index);
	mgr->get_messenger()->notify("icon_changed", entity);
}

void fill_icon_part(ecs::Entity_manager* mgr, std::shared_ptr<sf::Texture> texture, sf::IntRect area, const std::string& location, ecs::Entity_id entity)
{
	auto data = mgr->get_data<ecs::Component<Drawable>>(ecs::Component_type::Drawable, entity);
	auto& icon_part = data->icon_parts.at(location);
	//icon_part.resource_ptr = texture_resource;
	icon_part.texture = texture;
	icon_part.sprite.setTexture(*icon_part.texture);
	icon_part.sprite.setTextureRect(area);
	mgr->get_messenger()->notify("icon_changed", entity);
}

void clear_icon_part(ecs::Entity_manager* mgr, const std::string& location, ecs::Entity_id entity)
{
	auto data = mgr->get_data<ecs::Component<Drawable>>(ecs::Component_type::Drawable, entity);
	auto& icon_part = data->icon_parts.at(location);
	clear_icon(icon_part);
	mgr->get_messenger()->notify("icon_changed", entity);
}