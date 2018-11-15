#include "controller.h"

void Controller::setup_events ()
{
	Dispatcher d;
	m_dispatchers["change_position"] = d;
}