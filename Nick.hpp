#ifndef NICK_HPP
#define NICK_HPP

#pragma once

# include "ACommand.hpp"
# include <iostream>
# include <string>

using std::cout;
using std::endl;
using std::string;

class Nick : public ACommand {
	public:
		// Construtors & Destructors
		Nick();
		virtual ~Nick();

		// Methods
		string executeCommand();

	private:
		// Attributes
		string _name;

};

#endif