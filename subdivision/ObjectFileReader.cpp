#include "ObjectFileReader.h"

ObjectFile ObjectFileReader::parseObjFile(std::string pathToObjectFile)
{
	ObjectFile objectFile;
	std::vector<glm::vec3> _vertices;
	std::ifstream fileReader(pathToObjectFile);
	if (fileReader.is_open())
	{
		std::string lineBuffer;

		while (std::getline(fileReader, lineBuffer))
		{
			std::stringstream sstream(lineBuffer);
			std::string splitPart;

			std::getline(sstream, splitPart, ' ');

			if (fileReader.eof())
			{
				break;
			}

			if (splitPart.at(0) == '#') // we found a comment
			{
				continue;
			}
			else if (splitPart == "v")
			{
				std::vector<float> coords;
				while (std::getline(sstream, splitPart, ' '))
				{
					float coord = std::stof(splitPart);
					coords.push_back(coord);

					if (coords.size() == 3)
					{
						glm::vec3 vertex = glm::vec3(coords[0], coords[1], coords[2]);
						_vertices.push_back(vertex);
						objectFile.addRawVertex(vertex);
					}
				}
				if (coords.size() != 3)
				{
					throw "Invalid object file! Found less than 3 vertexes.";
				}
			}
			else if (splitPart == "f")
			{
				std::vector<int> indexes;
				while (std::getline(sstream, splitPart, ' '))
				{
					int coord = std::stoi(splitPart);
					indexes.push_back(coord);

					if (indexes.size() == 3)
					{
						for (int i = 0; i < indexes.size(); i++)
						{							
							objectFile.addTriangleVertex(_vertices[indexes[i]-1]);
						}

						objectFile.addFace(glm::vec3(indexes[0], indexes[1], indexes[2]));
					}
					
				}
				if (indexes.size() != 3)
				{
					throw "Invalid object file! Found less than 3 coordinates.";
				}
			}
			else
			{
				throw "Invalid object file! Encountered unknown element: " + splitPart;
			}
		}
	}

	fileReader.close();

	return objectFile;
}
