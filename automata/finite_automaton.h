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
std::string join(Container const& container, std::string delim = " ")
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


std::string pad_left(std::string const& s, int padSize, char symbol = ' ')
{
	std::string string = s;
	while (string.length() < padSize)
		string = symbol + string;
	return string;
}

class finite_automaton
{
	class transition
	{
		std::set<state> _start;
		letter _letter;
		std::set<state> _states;

	public:
		transition() {};
		transition(std::set<state> s, letter l, std::set<state> ss) :_start(s), _letter(l), _states(ss) {};


		const std::set<state> states()
		{
			return _states;
		}

		const std::set<state> start()
		{
			return _start;
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
		return s.find('(') != std::string::npos;
	}

	bool start(std::string const& s)
	{
		return s.find('>') != std::string::npos;
	}

	std::string decorate(std::set<state> const& s)
	{
		auto string = join(s, ",");
		state state = string;
		state = brackets(state);
		for (auto accept : _accepts)
			if (string == accept)
			{
				state = "(" + state + ")";
			}
		if (string == _start)
			state = ">" + state;
		return state;
	}

	std::string brackets(std::string& s)
	{
		return '{' + s + '}';
	}

	transition merge(transition& t)
	{
		std::set<state> states;
		for (auto state : t.states())
		{
			auto it = std::find_if(_transition.begin(), _transition.end(), [&](transition x)
				{
					return x.letter() == t.letter();
				});
			auto moving_states = (*it).states();
			states.insert(moving_states.begin(), moving_states.end());
		}
		return { t.start(), t.letter(), { join(states, "")} };
	}

public:
	finite_automaton() {};
	finite_automaton(std::string const& file_name) : finite_automaton()
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
				_transition.push_back({ {state}, _alphabet[j - 1], { states.begin(), states.end() } });
			}
		}
		file.close();
	}



	finite_automaton determinization()
	{
		finite_automaton automaton{};
		std::vector<transition> transitions{};
		automaton._alphabet = _alphabet;
		auto start = *_states.find(_start);
		std::vector<state> states{ start };

		automaton._start = start;
		std::set<state> accepts{};
		if (std::find(states.begin(), states.end(), start) != states.end())
			accepts.insert(start);

		for (auto j = 0; j < states.size(); j++)
		{
			for (int i = 0; i < _alphabet.size(); i++)
			{
				letter letter = _alphabet[i];
				std::set<state> transition_states{};

				for (auto x : split(states[j], ','))
				{
					auto transition = std::find_if(_transition.begin(), _transition.end(), [&](auto tr) {
						return letter == tr.letter() && x == join(tr.start(), "");
						});
					if (transition == _transition.end())
						continue;
					auto states = (*transition).states();
					transition_states.insert(states.begin(), states.end());
				}

				if (transition_states.size() != 1 && transition_states.find("-") != transition_states.end())
					transition_states.erase("-");
				std::set<state> s{ states[j] };
				transitions.push_back( { s , letter, transition_states });
				auto state = join(transitions[_alphabet.size() * j + i].states(), ",");
				if (state != "-" && std::find(states.begin(), states.end(), state) == states.end())
				{
					states.push_back(state);
					for (auto accept : _accepts)
					{
						if (state.find(accept) != std::string::npos)
							accepts.insert(state);
					}
				}
			}
		}
		automaton._states.insert(states.begin(), states.end());
		automaton._transition = transitions;
		automaton._accepts = accepts;
		return automaton;
	}



	friend std::ostream& operator<<(std::ostream& os, finite_automaton& a)
	{
		for (letter letter : a._alphabet)
		{
			os << pad_left(letter, 17);
		}
		std::cout << std::endl;
		for (int i = 0,j = 1; i < a._transition.size(); ++i)
		{
			transition transition = a._transition[i];
			auto state = a.decorate(transition.start());
			if (i == 0)
				os << state;
			if (i > 0 && a._transition[i].start() != a._transition[i - 1].start())
			{
				os << std::endl << state;
				j = 1;
			}
			if (i > 0 && a._transition[i].start() == a._transition[i - 1].start())
				j++;
			auto s = join(transition.states(), ",");
			os << pad_left(a.brackets(s), 17 - (j == 1 ? state.length() : 0));
			
		}
		return os;
	}
};
#endif 
