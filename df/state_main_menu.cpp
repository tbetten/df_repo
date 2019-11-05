#include "state_main_menu.h"
//#include "shared_context.h"
#include "window.h"
#include "statemanager.h"
#include <iostream>

void State_main_menu::on_create()
{
	m_context->m_wind->get_renderwindow()->resetGLStates();
	m_gui_window = sfg::Window::Create(sfg::Window::Style::NO_STYLE);
	m_gui_window->SetRequisition(sf::Vector2f(m_context->m_wind->get_renderwindow()->getSize()));
	auto vbox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 5.0f);
	auto label = sfg::Label::Create("hello world");
	auto button_load = sfg::Button::Create("Load game");
	auto button_save = sfg::Button::Create("Save game");
	auto button_new = sfg::Button::Create("New game");
	button_new->GetSignal(sfg::Widget::OnLeftClick).Connect([this]() {to_game(); });
	vbox->Pack(label, false);
	vbox->Pack(button_load, false, true);
	vbox->Pack(button_save, false, true);
	vbox->Pack(button_new, false, true);
	m_root_widget = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL);
	m_root_widget->Pack(vbox, false);
	m_gui_window->Add(m_root_widget);
	m_desktop.Add(m_gui_window);
	m_desktop.SetProperty<sf::Color>("Button:Prelight", "Color", sf::Color::Magenta);
}

void State_main_menu::to_game()
{
	auto sm = m_context->m_state_manager;
	sm->switch_to(Game_state::Character_generator);
	m_done = true;
}

void State_main_menu::on_click()
{
	std::cout << "clicked\n";
}

void State_main_menu::on_destroy()
{

}

void State_main_menu::update(const sf::Time& time)
{
	m_desktop.Update(time.asSeconds());
}

void State_main_menu::handle_sfml_event(sf::Event& e)
{
	m_desktop.HandleEvent(e);
}

void State_main_menu::draw()
{
	m_sfgui.Display(*m_context->m_wind->get_renderwindow());

}

void State_main_menu::activate()
{
	m_root_widget->Show(true);
}

void State_main_menu::deactivate()
{
	m_root_widget->Show(false);
}