#include "renderer.h"
#include "components.h"
#include "position.h"
#include "drawable.h"
#include "facing.h"
#include "ecs.h"
#include "grid.h"
#include "shared_context.h"

namespace systems
{
	Renderer::Renderer (ecs::System_type type, ecs::System_manager* mgr) : S_base (type, mgr)
	{
		ecs::Bitmask b;
		b.set (static_cast<int>(ecs::Component_type::Position));
		b.set (static_cast<int>(ecs::Component_type::Drawable));
		m_requirements.push_back (b);
		m_position = m_system_manager->get_entity_mgr ()->get_component<ecs::Component<Position>> (ecs::Component_type::Position);
		m_drawable = m_system_manager->get_entity_mgr ()->get_component<ecs::Component<Drawable>> (ecs::Component_type::Drawable);
	}

	void Renderer::update (float dt)
	{
		auto mgr = m_system_manager->get_entity_mgr ();
		for (auto& entity : m_entities)
		{
			auto index = mgr->get_index(ecs::Component_type::Drawable, entity);
			auto drawable = &m_drawable->m_data[index];
			//auto drawable = mgr->get_data<ecs::Component<Drawable>> (ecs::Component_type::Drawable, entity);
			auto position = &m_position->m_data[mgr->get_index(ecs::Component_type::Position, entity)];
		//	auto position = mgr->get_data<ecs::Component<Position>> (ecs::Component_type::Position, entity);
			auto pixel_coords = position->current_map->grid_to_pixel (position->coords);// hexlib::hex_to_pixel (drawable->layout, position->coords);
			auto size = drawable->texture.getSize ();
			drawable->screen_coords.x = pixel_coords.x;// -(size.x / 2.0f);
			drawable->screen_coords.y = pixel_coords.y;// -(size.y / 2.0f);
			drawable->sprite.setPosition ({ static_cast<float>(pixel_coords.x), static_cast<float>(pixel_coords.y) });
		}
	}

	void Renderer::setup_events ()
	{
		auto system = m_system_manager->find_event("switched_current_entity");
		m_system_manager->get_event(system, "switched_current_entity").bind([this](std::any val) { m_current_entity = std::any_cast<ecs::Entity_id>(val); });
	}

	Dispatcher& Renderer::get_event (const std::string& event)
	{
		throw "don't have any events";
	}

	void Renderer::render (sf::RenderWindow* win)
	{
		auto mgr = m_system_manager->get_entity_mgr ();
		auto map = m_system_manager->get_context ()->m_current_map;
		auto view = sf::View{ sf::FloatRect{0.0f, 0.0f, 1000.0f, 800.0f} };
		auto index_current_entity = mgr->get_index(ecs::Component_type::Drawable, m_current_entity);
		auto current = &m_drawable->m_data[index_current_entity];
		view.setCenter(current->screen_coords);
		//view.setViewport (sf::FloatRect{ 0.1f , 0.1f, 0.5f, 0.5f });
		win->setView (view);
		win->draw (*map);
		//auto target = win->get_renderwindow ();
		for (auto& entity : m_entities)
		{
			//auto drawable = mgr->get_data<ecs::Component<Drawable>> (ecs::Component_type::Drawable, entity);
			auto drawable = &m_drawable->m_data[mgr->get_index(ecs::Component_type::Drawable, entity)];
			drawable->sprite.setTexture (drawable->texture);
			win->draw(drawable->sprite);
			if (mgr->has_component (entity, ecs::Component_type::Facing))
			{
				auto facing = mgr->get_data<ecs::Component<Facing>> (ecs::Component_type::Facing, entity);
				facing->facing_indicator.setTexture (facing->facing_texture);
				win->draw (facing->facing_indicator);
			}
		}
	}
}