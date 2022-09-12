#pragma once
#include <filesystem>
using namespace std::filesystem;
class FileExplorer
{
public:
	path path = current_path();
	void goTo(std::filesystem::path path); 
};

