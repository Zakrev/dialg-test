#include "qgroup.h"
#include "logs.h"

#include <iterator>
#include <fstream>

#include "external/rapidjson/document.h"
#include "external/rapidjson/istreamwrapper.h"

using namespace Dialg;
using namespace rapidjson;

//QPhrase source
void QPhrase::compile_pattern(string pattern)
{
	const char * error;
	int error_offset;

	pattern.insert(0, "^");

	this->expr = pcre_compile(pattern.c_str(), PCRE_CASELESS, &error, &error_offset, NULL);
	if (!this->expr)
	{
		LOG_ERR("pcre_compile failed: for '", pattern, "': ", error);
		throw "pcre_compile failed";
	}
}

QPhrase::QPhrase(string pattern_str, string help, list<string> args_model)
{
	this->compile_pattern(pattern_str);
	this->help = help;
	this->args_model = args_model;
}

QPhrase::QPhrase(rapidjson::Value & phrase)
{

	Value & val = phrase["id"];
	if (val.IsNull())
	{
		LOG_ERR("can't get 'id'");
		throw "can't get 'id'";
	}
	this->id = val.GetInt();

	val = phrase["model"];
	if (val.IsNull() || !val.IsArray())
	{
		LOG_ERR("can't get 'model'");
		throw "can't get 'model'";
	}
	for(SizeType i = 0; i < val.Size(); i++)
	{
		this->args_model.push_back(val[i].GetString());
	}

	val = phrase["pattern"];
	if (val.IsNull())
	{
		LOG_ERR("can't get 'pattern'");
		throw "can't get 'pattern'";
	}
	this->compile_pattern(val.GetString());

	val = phrase["help"];
	if (val.IsNull())
	{
		LOG_ERR("can't get 'help'");
		throw "can't get 'help'";
	}
	this->help = val.GetString();
}

QPhrase::~QPhrase()
{
}

char QPhrase::parse(const string str, ArgsModel * model)
{
	int * vector = NULL;
	int vector_size = (this->args_model.size() + 1) * 2;

	vector = new int [vector_size];
	if (!vector)
	{
		LOG_ERR("new failed for int [", vector_size, "]");
		throw "new failed";
	}

	int count = pcre_exec(this->expr, NULL, str.c_str(), str.length(), 0, 0, vector, vector_size);

	if (count < 0)
	{
		delete[] vector;
		LOG_DBG("pcre_exec 'not found', result: ", count);
		return 0;
	}
	else if (count != vector_size / 2)
	{
		delete[] vector;
		LOG_DBG("pcre_exec 'size not equal': ", count, "/", vector_size / 2);
		return -1;
	}

	auto it = this->args_model.begin();
	for (int i = 1; i < count && it != this->args_model.end(); i++, it++)
	{
		auto find_it = model->args.find(*it);
		if (find_it == model->args.end())
		{
			delete[] vector;
			LOG_DBG("find 'key not found': ", *it);
			return -1;
		}

		model->args[*it] = str.substr(vector[i * 2], vector[i * 2 + 1]);
	}

	delete[] vector;
	return 1;
}

char QPhrase::is_partial(const string str)
{
	int res = pcre_exec(this->expr, NULL, str.c_str(), str.length(), 0, PCRE_PARTIAL, NULL, 0);

	return (res == 0 || res == PCRE_ERROR_PARTIAL);
}

void QPhrase::clear()
{
	if (this->expr)
	{
		free(this->expr);
		this->expr = NULL;
	}
}

//QGroup source
QGroup::QGroup(unsigned long id, ArgsModel model_def)
{
	this->id = id;
	this->model_def = model_def;
}

QGroup::QGroup(string json_path)
{
	ifstream json_file(json_path);

	if (!json_file.is_open())
	{
		LOG_ERR("failed open json file: ", json_path);
		throw "failed open json file";
	}

	IStreamWrapper isw(json_file);
	Document json_doc;

	json_doc.ParseStream(isw);

	if (json_doc.IsNull())
	{
		LOG_ERR("can't' parse json file: ", json_path);
		throw "can't' parse json file";
	}

	Value & val = json_doc["id"];
	if (val.IsNull())
	{
		LOG_ERR("can't get 'id'");
		throw "can't get 'id'";
	}
	this->id = val.GetInt();

	val = json_doc["model"];
	this->model_def = ArgsModel(val);

	val = json_doc["phrases"];
	if (val.IsNull() || !val.IsArray())
	{
		LOG_ERR("can't get 'phrases'");
		throw "can't get 'phrases'";
	}

	for(SizeType i = 0; i < val.Size(); i++)
	{
		this->phrases.push_back(QPhrase(val[i]));
	}
}

QGroup::~QGroup()
{
}

char QGroup::add_phrase(QPhrase phrase)
{
	phrase.group_id = this->id;
	this->phrases.push_back(phrase);

	return 0;
}

list<QPhrase *> QGroup::get_phrases()
{
	list<QPhrase *> phrases;

	for (auto it = this->phrases.begin(); it != this->phrases.end(); it++)
	{
		QPhrase * phrase = &(*it);

		phrases.push_back(phrase);
	}

	return phrases;
}

ArgsModel QGroup::create_model(const string text)
{
	ArgsModel model = this->model_def;

	for (auto it = this->phrases.begin(); it != this->phrases.end(); it++)
	{
		QPhrase * phrase = &(*it);

		if (1 == phrase->parse(text, &model))
		{
			return model;
		}
	}

	return model;
}

void QGroup::clear()
{
	for (auto it = this->phrases.begin(); it != this->phrases.end(); it++)
	{
		QPhrase * phrase = &(*it);

		phrase->clear();
	}
}
