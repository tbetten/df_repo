// df.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "attribute.h"
#include <iostream>

using namespace attrib;
int main()
{
	Attribute_set attribs;
	int budget = 100;
	attribs.buy(Attribute_type::ST, 2, budget);
	std::cout << attribs.get_value(Attribute_type::ST, Attribute_set::Value_type::Effective) << std::endl;
	std::cout << attribs.get_value(Attribute_type::ST, Attribute_set::Value_type::Natural) << std::endl;
    return 0;
}

