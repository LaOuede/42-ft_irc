#include "Cap.hpp"

/* ************************************************************************** */
/* Constructors and Destructors                                               */
/* ************************************************************************** */
Cap::Cap() : ACommand("CAP") {}

Cap::~Cap() {}


/* ************************************************************************** */
/* Getters & Setters                                                          */
/* ************************************************************************** */



/* ************************************************************************** */
/* Functions                                                                  */
/* ************************************************************************** */
string Cap::executeCommand() {
	cout << this->getCommandName() << endl;
	return ("001 user Welcome from CAP\r\n");
}


/* ************************************************************************** */
/* Exceptions                                                                 */
/* ************************************************************************** */