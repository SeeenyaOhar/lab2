#pragma once
#include <vector>
#include <string>
using namespace std;
class Command
{
public:
    string executable;
    vector<string> arguments = vector<string>();
    int compare(string s);
};
class CommandExtractor {
public:
    Command extract(string s);
};

