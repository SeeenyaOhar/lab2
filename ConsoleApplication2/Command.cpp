#include "Command.h"
#include <string>
using namespace std;
Command CommandExtractor::extract(string s)
{
	Command command = Command();
	size_t upperBound = s.find(" ");
	string next_argument = s.substr(0, upperBound);

	if (s.find_first_not_of(' ') != string::npos) {
		command.executable = next_argument;
		s.erase(0, upperBound + 1);
	}
	while (s.find(" ") != std::string::npos) {
		upperBound = s.find(" ");
		if (upperBound != std::string::npos){
			string next_argument = s.substr(0, upperBound);
			command.arguments.push_back(next_argument);
			s.erase(0, upperBound + 1);
		}
	}
	if (s != "") {
		command.arguments.push_back(s);
	}
	return command;
}
int Command::compare(string s) {
	return this->executable.compare(s);
}
