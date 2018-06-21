#ifndef DIALG_COMMON_H
#define DIALG_COMMON_H

#include <map>
#include <string>

#include "external/rapidjson/document.h"

using namespace std;

namespace Dialg
{

typedef unsigned long QGroupID;
#define QGroupID_to_idx(_val_) \
	((_val_) - 1)

typedef unsigned long QPhraseID;
#define QPhraseID_to_idx(_val_) \
	((_val_) - 1)

class ArgsModel
{
	private:
		bool invalid;
		QPhraseID qPhraseId;
		QGroupID qGroupId;
		map<string, string> args;
	public:
		ArgsModel();
		ArgsModel(QPhraseID qPhraseId, QGroupID qGroupId, map<string, string> & args);
		ArgsModel(QGroupID qGroupId, rapidjson::Value & json_model);
		~ArgsModel();

		bool IsValid();
		QGroupID GetQGroupID();
		QPhraseID GetQPhraseID();
		map<string, string> & GetArguments();

		string & operator[](string key);
};

}

#endif