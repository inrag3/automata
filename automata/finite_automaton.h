#pragma once
#ifndef FINITE_AUTOMATON_H
#define FINITE_AUTOMATON_H

#include <set>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <regex>
#include <format>

using letter = std::string;
using state = std::string;
using line = std::vector<std::string>;

line split(const std::string& s, char delim = ' ') {
	std::vector<std::string> v;
	std::stringstream ss(s);
	std::string item;

	while (getline(ss, item, delim)) {

		if (!item.empty())
			v.push_back(item);
	}
	return v;
}

template<typename Container>
std::string join(Container const& container, char delim = ',')
{
	std::string s = "";
	for (auto i = container.begin(); i != container.end(); ++i)
	{
		s += *i;
		if (std::next(i) != container.end()) {
			s += delim;
		}
	}
	return s;
}


class finite_automaton
{
	class transition
	{
		state _state;
		letter _letter;
		std::set<state> _states;

	public:
		transition() {};
		transition(state s, letter l, std::set<state> ss) :_state(s), _letter(l), _states(ss) {};

		const std::set<state> states()
		{
			return _states;
		}

		const state state()
		{
			return _state;
		}

		const letter letter()
		{
			return _letter;
		}
	};

	//M = (Q, Σ, δ, q0, F) 
	std::set<state> _states;
	std::vector<letter> _alphabet;
	std::vector<transition> _transition;
	state _start;
	std::set<state> _accepts;

	bool accept(std::string const& s)
	{
		return s[s.size() - 1] == ')';
	}

	bool start(std::string const& s)
	{
		return s[0] == '>';
	}

	std::string decorate_state(std::string const& s)
	{
		state state = s;
		if (_start == s)
			state = "(" + state + ")";
		if (_accepts.contains(s))
			state = ">" + state;
		return state;
	}
public:
	finite_automaton(std::string const& file_name)
	{
		std::string s;
		std::ifstream file(file_name);

		std::vector<line> lines;
		while (getline(file, s))
			lines.push_back(split(s));

		//Filling alphabet
		std::for_each(lines[0].begin(), lines[0].end(), [&](letter letter)
			{
				_alphabet.push_back(letter);
			});

		//Filling states
		for (auto i = 1; i < lines.size(); ++i)
		{
			auto line = lines[i];
			auto raw_state = line[0];

			//Remove ( ) and >( )
			const std::regex r(R"((\w+))");
			std::smatch match;
			std::regex_search(raw_state, match, r);
			auto state = match[0];
			if (accept(raw_state))
				_accepts.insert(state);
			if (start(raw_state))
				_start = state;
			_states.insert(state);

			//Filling transitions
			for (auto j = 1; j < line.size(); ++j)
			{
				auto states = split(line[j], ',');
				_transition.push_back({ state, _alphabet[j - 1], { states.begin(), states.end() } });
			}
		}



		file.close();
	}

	

	void determinization()
	{
		//TODO determinization
	}


	friend std::ostream& operator<<(std::ostream& os, finite_automaton& a)
	{

		for (letter letter : a._alphabet)
		{
			os << std::format("{: >10}", letter);
		}
		std::cout << std::endl;
		for (int i{}; i < a._transition.size(); ++i )
		{
			transition transition = a._transition[i];
			auto state = a.decorate_state(transition.state());
			if (i == 0)
				os << std::format("{}", state);
			if (i > 0 && a._transition[i].state() != a._transition[i - 1].state())
				os << std::endl << std::format("{}", state);	
			os << std::format("{: >10}", join(transition.states()));
		} 
		return os;
	}
};
#endif 
