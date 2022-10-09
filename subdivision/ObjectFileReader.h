#pragma once
#include "ObjectFile.h"
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>

class ObjectFileReader
{
	ObjectFile _objectFile;

public:
	ObjectFile parseObjFile(std::string);
};

