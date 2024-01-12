#ifndef USER_HPP
#define USER_HPP

#pragma once

# include "ACommand.hpp"
# include <iostream>
# include <string>

using std::cout;
using std::endl;
using std::string;

class User : public ACommand {
	public:
		// Construtors & Destructors
		User();
		virtual ~User();

		// Methods
		string executeCommand();

	private:
		// Attributes
		string _name;

};

#endif