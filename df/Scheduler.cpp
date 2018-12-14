#include "stdafx.h"
#include "scheduler.h"
#include <algorithm>
#include <iostream>

void Scheduler::schedule (Action action, int initial_prio)
{
	m_schedule.emplace_back (Turn{ initial_prio, action });
	std::push_heap (m_schedule.begin (), m_schedule.end ());
}

void Scheduler::unschedule ()
{
	std::pop_heap (m_schedule.begin (), m_schedule.end ());
	m_schedule.pop_back ();
}

int Scheduler::pass_time ()
{
	if (m_waiting) return 0;
	Turn& current = m_schedule.front ();
	auto delay = current.action ();
	if (delay)
	{
		return process_delay (current, delay.value ());
	}
	else
	{
		m_waiting = true;
		return 0;
	}
}

int Scheduler::async_response (int duration)
{
	if (!m_waiting)
	{
		std::cout << "dit mag niet gebeuren!!" << std::endl;
	}
	m_waiting = false;
	return process_delay (m_schedule.front (), duration);
}

int Scheduler::process_delay (Turn& current, int duration)
{
	int t1 = current.prio;
	current.prio -= duration;
	std::make_heap (m_schedule.begin (), m_schedule.end ());
	int delay = t1 - m_schedule.front ().prio;
	for (auto t : m_schedule)
	{
		t.prio += delay;
	}
	return delay;
}