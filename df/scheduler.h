#pragma once

#include <functional>
#include <optional>

using Action = std::function<std::optional<int> ()>;

struct Turn
{
	int prio;
	Action action;
};

bool operator< (const Turn& left, const Turn& right)
{
	return left.prio < right.prio;
}

class Scheduler
{
public:
	void schedule (Action action, int initial_prio = 0);
	void unschedule ();
	int pass_time ();
	int async_response (int duration);
private:
	int process_delay (Turn& current_turn, int delay);

	std::vector<Turn> m_schedule;
	bool m_waiting{ false };
};