#include "stdafx.h"
#include "builder.h"
#include <algorithm>
#include <filesystem>
#include <bitset>

namespace fs = std::filesystem;

Builder::Builder() : m_window { sf::VideoMode{ 600, 600 }, "Entity builder" }
{
	fs::path dbfile;
	try
	{
		dbfile = fs::canonical(fs::current_path() / "../db/dfdata.db");
	}
	catch (fs::filesystem_error& e)
	{
		std::cout << e.what() << std::endl;
	}
	std::cout << dbfile << std::endl;
	m_database = db::db_connection::create(dbfile.string());

	m_window.resetGLStates();
	m_gui_window = sfg::Window::Create();
	auto winsize = m_window.getSize();
	m_gui_window->SetRequisition(sf::Vector2f(m_window.getSize()));
	Page::set_builder(this);
	m_pages.emplace_back(true, "Entity", Component_type::None , Entity_page::create());
	m_pages.emplace_back(false, "Shared data", Component_type::Item_shared, Shared_page::create());
	m_pages.emplace_back(false, "Projectile", Component_type::Projectile, Projectile_page::create());
	m_pages.emplace_back(false, "Ranged Weapon", Component_type::Ranged, Ranged_page::create());
	m_pages[0].m_index = 0;
	setup_window();
}

void Builder::run()
{
	sf::Clock clock;
	m_gui_window->Update(0.0f);
	while (m_window.isOpen())
	{
		sf::Event event;
		while (m_window.pollEvent(event))
		{
			m_gui_window->HandleEvent(event);
			if (event.type == sf::Event::Closed)
			{
				m_window.close();
			}
		}
		if (clock.getElapsedTime().asMicroseconds() > 5000)
		{
			update_window();
			m_gui_window->Update(static_cast<float>(clock.getElapsedTime().asMicroseconds()) / 1000000.0f);
			clock.restart();
		}
		m_window.clear();
		m_gui.Display(m_window);
		m_window.display();
	}
}

void Builder::setup_window()
{
	m_notebook = sfg::Notebook::Create();
	m_notebook->AppendPage(m_pages[0].m_page->get_page(), sfg::Label::Create("Entity"));

	auto box = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
	m_create_button = sfg::Button::Create("Create");
	m_create_button->GetSignal(sfg::Button::OnLeftClick).Connect([this]() {on_create(); });
	box->Pack(m_notebook);
	box->Pack(m_create_button, false, false);
	m_gui_window->Add(box);
}

void Builder::set_tab_status(Page_type type, bool status)
{
	auto index = static_cast<int>(type);
	m_pages[index].m_active = status;
}

void Builder::update_window()
{
	for (auto& data : m_pages)
	{
		if (data.m_active && data.m_index == -1)
		{
			auto i = m_notebook->AppendPage(data.m_page->get_page(), sfg::Label::Create(data.m_title));
			data.m_index = i;
		}
		if (!data.m_active && data.m_index != -1)
		{
			m_notebook->RemovePage(data.m_index);
			data.m_index = -1;
		}
	}

}

void Builder::on_create()
{
	std::bitset<32> index;
	for (auto& p : m_pages)
	{
		if (p.m_active)
		{
			if (!p.m_page->validate())
			{
				std::cout << "page " << p.m_title << " is not filled out correctly" << std::endl;
				return;
			}
			if (p.m_type != Component_type::None)
			{
				index.set(static_cast<int>(p.m_type));
			}
		}
	}

	// all pages validated successfully
//	auto& x = m_pages[0];
//	auto& y = x.m_page;
//	auto z = *y;
	auto key = m_pages[0].m_page->get_key();
	for (auto& p : m_pages)
	{
		if (p.m_active && p.m_type != Component_type::None)
		{
			p.m_page->insert_into_database(m_database, key);
		}
	}
	std::string entity_sql = "insert into entity (key, c_index) values (?, ?)";
	auto stmt = m_database->prepare(entity_sql);
	stmt->bind(1, key);
	stmt->bind(2, index.to_string());
	int rc = stmt->execute_row();
	//int max_shared = m_pages[1].m_page->insert_into_database(m_database);
	//std::cout << "max index of page shared is " << max_shared << std::endl;
}
