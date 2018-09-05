#pragma once
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include "state.h"
#include "shared_context.h"
/*#include "state_intro.h"
#include "state_main_menu.h"
#include "state_game.h"
#include "state_paused.h"*/

/*enum class State_type
{
All_states, Intro, Main_menu, Game, Paused, Game_over, Credits
};*/

using State_ptr = std::unique_ptr<State>;
//using State_container = std::vector<std::pair<State_type, State_ptr>>;
using State_container = std::unordered_map<int, State_ptr>;
//using State_elem = std::pair<State_type, State_ptr>;
using Type_container = std::vector<int>;
//using State_factory = std::unordered_map<State_type, std::function<State_ptr (void)>>;

class State_manager
{
public:
	explicit State_manager(Shared_context* shared);
	~State_manager();

	void insert_state(const int id, State_ptr state);
	void update(const sf::Time time);
	void draw();

	void process_requests();

	Shared_context* get_context() const { return m_shared; }
	bool has_state(const int id) const;
	void switch_to(const int id);
	void remove(const int id);
private:
	void create_state(const int id);
	void remove_state(const int id);

	/*template <class T>
	void register_state(const State_type type)
	{
	m_state_factory[type] = [this]()->State_ptr {return std::make_unique<T>(this); };//new T(this); }
	}*/

	//void register_state(const State_type type, const State& state);

	Shared_context * m_shared;
	State_container m_states;
	std::vector<int> m_state_stack;
	Type_container m_to_remove;
	//State_factory m_state_factory;
};
