#ifndef FILE_LOADER_H
#define FILE_LOADER_H

#include <filesystem>
#include <string>

class FileLoader {
public:
	FileLoader();
	~FileLoader();
	std::string loadAsString();
};

#endif // !FILE_LOADER_H
