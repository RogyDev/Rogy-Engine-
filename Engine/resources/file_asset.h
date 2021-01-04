#ifndef R_ASSET_H
#define R_ASSET_H

#include <iostream>
#include <string>

class Asset
{
public:
	Asset();
	~Asset();

	std::string name;
	std::string path;

private:

};

Asset::Asset()
{
}

Asset::~Asset()
{
}
#endif