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
		list<string> args_model;

		void compile_pattern(string pattern);
	public:
		string help;
		unsigned long id;
		unsigned long group_id;

		QPhrase(string pattern_str, string help, list<string> args_model);
		QPhrase(rapidjson::Value & phrase);
		~QPhrase();

		char parse(const string str, ArgsModel * model);
		char is_partial(const string str);
		void clear();
};

class QGroup
{
	private:
		list<QPhrase> phrases;
		ArgsModel model_def;
	public:
		unsigned long id;

		QGroup(unsigned long id, ArgsModel model_def);
		QGroup(string json_path);
		~QGroup();

		char add_phrase(QPhrase phrase);
		list<QPhrase *> get_phrases();
		ArgsModel create_model(const string text);

		void clear();
};

}

#endif