#include "state_character_generator.h"
#include "shared_context.h"
#include "window.h"
#include "statemanager.h"



void State_character_generator::on_create()
{
	m_gui_window = sfg::Window::Create();
	m_gui_window->SetRequisition(sf::Vector2f(m_context->m_wind->get_renderwindow()->getSize()));
}

void State_character_generator::on_destroy()
{

}

void State_character_generator::update(const sf::Time& time)
{

}

void State_character_generator::draw()
{

}

void State_character_generator::to_game()
{
	m_context->m_state_manager->switch_to(Game_state::Game);
}