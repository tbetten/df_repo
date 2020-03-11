/*#include "db.h"

int main()
{
	db::Blob b{};
	void* data = std::malloc(4);
	std::memset(data, 'A', 4);
	b.value = data;
	b.size = 4;
	db::Blob b2{ b };
}*/