#include "renderer.h"
#include "components.h"
#include "position.h"
#include "drawable.h"
#include "facing.h"
#include "ecs.h"
//#include "grid.h"
#include "shared_context.h"
#include "map_data.h"

namespace systems
{
	using namespace std::string_literals;

	Renderer::Renderer(ecs::System_type type, ecs::System_manager* mgr, messaging::Messenger* m) : S_base(type, mgr), messaging::Sender{ m }
	{
		ecs::Bitmask b;
		b.set (static_cast<int>(ecs::Component_type::Position));
		b.set (static_cast<int>(ecs::Component_type::Drawable));
		m_requirements.push_back (b);
		m_position = m_system_manager->get_entity_mgr ()->get_component<ecs::Component<Position>> (ecs::Component_type::Position);
		m_drawable = m_system_manager->get_entity_mgr ()->get_component<ecs::Component<Drawable>> (ecs::Component_type::Drawable);
	}

	sf::Vector2i grid_to_pixel(Shared_context* context, std::string map, sf::Vector2i grid_coords)
	{
		return context->m_maps->maps[map].topology->center(grid_coords);
	}

	void Renderer::update (sf::Int64 dt)
	{
		auto mgr = m_system_manager->get_entity_mgr ();
		for (auto& entity : m_entities)
		{
			auto index = mgr->get_index(ecs::Component_type::Drawable, entity);
			auto drawable = &m_drawable->m_data[*index];
			auto position = &m_position->m_data[*mgr->get_index(ecs::Component_type::Position, entity)];
			if (position->moved)
			{
				//auto drawable = mgr->get_data<ecs::Component<Drawable>> (ecs::Component_type::Drawable, entity);
				
				//	auto position = mgr->get_data<ecs::Component<Position>> (ecs::Component_type::Position, entity);
					//auto pixel_coords = position->current_map->grid_to_pixel (position->coords);// hexlib::hex_to_pixel (drawable->layout, position->coords);
				auto pixel_coords = grid_to_pixel(m_system_manager->get_context(), position->current_map, position->coords);
				auto& tex = drawable->composed_icon->getTexture();
				auto size = tex.getSize();
				//auto size = drawable->texture->getSize ();
				drawable->screen_coords.x = pixel_coords.x -(size.x / 2.0f);
				drawable->screen_coords.y = pixel_coords.y -(size.y / 2.0f);
				drawable->sprite.setPosition({ static_cast<float>(pixel_coords.x), static_cast<float>(pixel_coords.y) });
				position->moved = false;
			}
		}
	}

	void Renderer::setup_events ()
	{
		m_messenger->bind("switched_current_entity"s, [this](std::any val) { m_current_entity = std::any_cast<ecs::Entity_id>(val); });
	}

	struct 
	{
		bool operator() (ecs::Entity_id e1, ecs::Entity_id e2)
		{

		}
	} Entity_compare;

	bool Renderer::compare_entities(ecs::Entity_id e1, ecs::Entity_id e2)
	{
		if (e1 == e2) return false;
		auto entity_mgr = m_system_manager->get_entity_mgr();
		auto pos_index_1 = entity_mgr->get_index(ecs::Component_type::Position, e1);
		auto pos_index_2 = entity_mgr->get_index(ecs::Component_type::Position, e2);
		if (!pos_index_1 || !pos_index_2) return false;
		auto pos1 = m_position->m_data[*pos_index_1];
		auto pos2 = m_position->m_data[*pos_index_2];
		return pos1 < pos2;
	}

	void Renderer::order_entities()
	{
		
		std::sort(std::begin(m_entities), std::end(m_entities), [this](ecs::Entity_id e1, ecs::Entity_id e2) {return compare_entities(e1, e2); });

/*		auto entity_mgr = m_system_manager->get_entity_mgr();
		for (auto layer : m_maps)
		{
			layer.second.clear();
		}
		m_maps.clear();
		for (auto entity : m_entities)
		{
			auto pos_index = entity_mgr->get_index(ecs::Component_type::Position, entity);
			if (!pos_index) continue;
			auto position = m_position->m_data[*pos_index];
			auto map = position.current_map;
			auto layer = position.layer;
			if (!m_maps.contains(map))
			{
				m_maps.emplace(map, Layers{});
			}
		}*/
	}

	void Renderer::render (sf::RenderWindow* win)
	{
		auto mgr = m_system_manager->get_entity_mgr ();
		auto map = m_system_manager->get_context ()->m_current_map;
		//auto index_current_entity = *mgr->get_index(ecs::Component_type::Drawable, m_current_entity);
		//auto current = &m_drawable->m_data[index_current_entity];
		//auto view = win->getView();
		//view.setCenter(current->screen_coords);
		//win->setView(view);

		//auto drawable_comp = mgr->get_component<ecs::Component<Drawable>>(ecs::Component_type::Drawable);
/*		auto& layers = m_system_manager->get_context()->m_maps->maps[map].layers;
		for (auto& layer : layers)
		{
			for (auto& cell : layer)
			{
				for (auto& entity : cell)
				{
					if (entity)
					{
						auto index = mgr->get_index(ecs::Component_type::Drawable, *entity);
						if (!index) continue;
						auto& drawable = m_drawable->m_data[*index];
						win->draw(drawable.sprite);
						//win->draw(sf::Sprite{ drawable.composed_icon->getTexture() });
					}
					else
					{
						break;
					}
				}
			}
		}*/

		//win->draw (*map);
		//auto target = win->get_renderwindow ();
		for (auto& entity : m_entities)
		{
			//auto drawable = mgr->get_data<ecs::Component<Drawable>> (ecs::Component_type::Drawable, entity);
			auto drawable = &m_drawable->m_data[*mgr->get_index(ecs::Component_type::Drawable, entity)];
			//drawable->sprite.setTexture (*drawable->texture);
			win->draw(drawable->sprite);
	/*		if (mgr->has_component (entity, ecs::Component_type::Facing))
			{
				auto facing = mgr->get_data<ecs::Component<Facing>> (ecs::Component_type::Facing, entity);
				facing->facing_indicator.setTexture (facing->facing_texture);
				win->draw (facing->facing_indicator);
			}*/
		}
	}
}