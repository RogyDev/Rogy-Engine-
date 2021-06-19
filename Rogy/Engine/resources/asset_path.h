#ifndef RES_ASSET_PATH_H
#define RES_ASSET_PATH_H
	
#include <iostream>
#include <string>

class AssetPath
{
public:
	/*AssetPath(std::string atype, std::string apath)
	{
		SetPath(apath);
		__type = atype;
	}
	AssetPath(std::string atype)
	{
		SetPath("");
		__type = atype;
	}*/
	AssetPath()
	{
		SetPath("");
	}
	void SetPath(std::string apath)
	{
		path = apath;
	}

	std::string& GetPath()
	{
		return path;
	}
	std::string& GetType()
	{
		return __type;
	}
	std::string __type = "asset";
private:
	std::string path;
	
};

#endif