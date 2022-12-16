#include <iostream>
#include "finite_automaton.h"

int main()
{
	finite_automaton automaton = { "a.txt" };
	auto x = automaton.determinization();
	std::cout << x << std::endl;
}