#include "stdafx.h"
#include "renderer.h"
#include "ecs_base.h"
#include "position_comp.h"
#include "drawable_comp.h"
#include "window.h"

void Renderer::setup_events ()
{
	Dispatcher d;
	m_dispatchers["dummy"] = d;
}

Dispatcher& Renderer::get_event (std::string event)
{
	return m_dispatchers[event];
}

void Renderer::update (int dt)
{
	auto entities = m_system_manager->get_entity_mgr ();
	for (auto& entity : m_entities)
	{
		auto pos = entities->get_component<Position_comp> (entity, Component::Position);
		auto drawable = entities->get_component<Drawable_comp> (entity, Component::Drawable);
		drawable->update_position (pos->coords);
	}
}

void Renderer::render (Window* win)
{
	auto entities = m_system_manager->get_entity_mgr ();
	for (auto& entity : m_entities)
	{
		auto drawable = entities->get_component<Drawable_comp> (entity, Component::Drawable);
		drawable->draw (win->get_renderwindow ());
	}
}