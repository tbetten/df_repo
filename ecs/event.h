#pragma once
#include <queue>
#include <optional>

struct Event
{

};

class Event_queue
{
public:
	void add_event(const Event& event) noexcept { m_queue.push(event); }
	std::optional<Event> get_event() noexcept;
	void clear() noexcept;
private:
	std::queue<Event> m_queue;
};