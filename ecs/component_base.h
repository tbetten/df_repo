#pragma once
#include <memory>

#include "ecs_types.h"

class Component_base
{
public:
	using Ptr = std::shared_ptr<Component_base>;
	Component_base(const Component type) : m_type{ type } {}
	virtual ~Component_base() = default;

	Component get_type() { return m_type; }

protected:
	Component m_type;
};