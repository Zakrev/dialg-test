#ifndef QGROUP_H
#define QGROUP_H

#include <list>
#include <vector>
#include <string>

#include "dialg_common.h"

#include "external/pcre/pcre.h"
#include "external/rapidjson/document.h"

using namespace std;

namespace Dialg
{

class QPhrase
{
	private:
		pcre * expr;
		list<string> model;
		string help;
		QPhraseID id;
		QGroupID group_id;

		void CompilePattern(string pattern);
	public:
		QPhrase();
		QPhrase(rapidjson::Value & phrase);
		~QPhrase();

		void Initialize(rapidjson::Value & phrase);

		QGroupID GetQGroupID();
		QPhraseID GetQPhraseID();
		string & GetHelp();

		ArgsModel FillModel(const string str, ArgsModel & model);
		bool IsPartial(const string str);
};

class QGroup
{
	private:
		vector<QPhrase> phrases;
		ArgsModel model;
		QGroupID id;
	public:
		QGroup(string json_path);
		~QGroup();

		QGroupID GetQGroupID();
		size_t GetPhrasesCount();

		QPhrase & operator[](QPhraseID key);
};

}

#endif