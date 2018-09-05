#pragma once
#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Widgets.hpp>
#include <SFML/Graphics.hpp>

#include <unordered_map>
#include <memory>
#include <vector>
#include "pages.h"
#include "db.h"

enum class Component_type { None = -1, Position, Attribute_set, Skill_set, Item_shared, Projectile, Ranged };
struct Pagedata
{
	Pagedata(bool active, std::string title, Component_type type, Page::Ptr page) : m_active{ active }, m_title{ title }, m_type{ type }, m_page{ std::move(page) } {}
	bool m_active;
	std::string m_title;
	Component_type m_type;
	int m_index = -1;  // index of page in notebook
	std::unique_ptr<Page> m_page;
};

enum class Page_type { Entity, Shared, Projectile, Ranged };
using Page_container = std::vector<Pagedata>;

class Builder
{
public:
	Builder();
	void run();
	void set_tab_status(Page_type type, bool status);
	void update_window();
private:
	void setup_window();
	void on_create();

	
	sfg::SFGUI m_gui;
	sf::RenderWindow m_window;
	sfg::Window::Ptr m_gui_window;
	sfg::Notebook::Ptr m_notebook;
	sfg::Button::Ptr m_create_button;

	Page_container m_pages;
	db::db_connection_ptr m_database;
//	Entity_page m_entity_page;
};