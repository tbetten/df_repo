// df.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "attribute.h"
#include <iostream>

using namespace Attributes;
int main()
{
	Attribute_set attribs;
	attribs.buy(Attribute_set::Attrib::ST, 2, 10);
	std::cout << attribs.get_natural_value(Attribute_set::Attrib::IQ) << std::endl;
	std::cout << attribs.get_natural_value(Attribute_set::Attrib::ST) << std::endl;
    return 0;
}

