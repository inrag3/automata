#include <iostream>
#include "finite_automaton.h"

int main()
{
	finite_automaton automaton = { "a.txt" };
	std::cout << automaton << std::endl;
	std::cout << std::endl;
	auto x = automaton.determinization();
	std::cout << x << std::endl;
}