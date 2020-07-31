#pragma once
#include "system.h"
#include "attributes.h"

struct Attributes;
struct Character;

namespace ecs
{
	template <class T>
	struct Component;
	class Entity_manager;
}

enum class Attribute : unsigned int;

namespace systems
{
	class Attribute_system : public ecs::S_base
	{
	public:
		Attribute_system(ecs::System_type type, ecs::System_manager* mgr);
		void update(sf::Int64 dt) override; 
		void setup_events() override;
//		Dispatcher& get_event(const std::string& event) override;

		void add_transaction(attributes::Transaction transaction, ecs::Entity_id entity);
		void remove_transaction(unsigned int transaction_id, ecs::Entity_id entity);

		void buy_attribute(::Attribute a, ecs::Entity_id entity, int cp);
		void buy_units (Attribute a, ecs::Entity_id entity, int units);
		bool check_cp(int cp, ecs::Entity_id entity);
		void pay_cp(int cp, ecs::Entity_id entity);
		int get_attribute(Attribute a);
	private:
		void init();
		ecs::Entity_manager* m_em;
		ecs::Component<Attributes>* m_attribs;
		ecs::Component<Character>* m_char;
	};
}