#ifndef DIALG_COMMON_H
#define DIALG_COMMON_H

#include <map>
#include <string>

#include "external/rapidjson/document.h"

using namespace std;

namespace Dialg
{

class ArgsModel
{
	public:
		map<string, string> args;

		ArgsModel();
		ArgsModel(rapidjson::Value & json_model);
		~ArgsModel();

		void insert(string key, string val);
};

}

#endif