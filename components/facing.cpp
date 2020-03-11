#include "facing.h"
#include "resource_cache.h"
#include <iostream>

void Facing::reset ()
{
	facing = Compass::North;
	m_texture_resource = nullptr;
}