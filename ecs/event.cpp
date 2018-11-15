#include "event.h"

std::optional<Event> Event_queue::get_event() noexcept
{
	if (m_queue.empty()) return std::nullopt;
	auto first = m_queue.front();
	m_queue.pop();
	return first;
}

void Event_queue::clear() noexcept
{
	while (!m_queue.empty())
	{
		m_queue.pop();
	}
}