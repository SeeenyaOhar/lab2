#include "FileExplorer.h"
void FileExplorer::goTo(std::filesystem::path p) {
	if (p.string().compare(".")) {
		if (p.string() == "..") {
			this->path = this->path.parent_path();
		}
		else {
			this->path = p;
		}
		
	}
	
}