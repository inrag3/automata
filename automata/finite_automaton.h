#pragma once
#ifndef FINITE_AUTIMATON_H
#define FINITE_AUTIMATON_H

#include <set>
#include <map>
#include <iostream>
#include <string>
using letter = std::string;
using state = std::string;

class finite_autimaton
{
	//M = (Q, Σ, δ, q0, F) 
	std::set<state> _states;
	std::set<letter> _alphabet;
	std::map<state, std::set<state>> _transition;
	state _start;
	std::set<state> _accepts;

public:  
	finite_autimaton(FILE* file)
	{
		//TODO сделать считывание из файла
	}

	void determinization()
	{
		//TODO сделать детерминизацию
	}


	friend std::ostream& operator<<(std::ostream& os, finite_autimaton const& a)
	{
		//TODO сделать вывод на консоль
	}
};
#endif 
