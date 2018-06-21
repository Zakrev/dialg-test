#include "qgroup.h"
#include "logs.h"

#include <iterator>
#include <fstream>

#include "external/rapidjson/document.h"
#include "external/rapidjson/istreamwrapper.h"

using namespace Dialg;
using namespace rapidjson;

//QPhrase source
void QPhrase::CompilePattern(string pattern)
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

QPhrase::QPhrase()
{
	this->expr = NULL;
}

QPhrase::QPhrase(rapidjson::Value & phrase)
{
	this->expr = NULL;
	this->Initialize(phrase);
}

QPhrase::~QPhrase()
{
	if (this->expr)
		free(this->expr);
}

void QPhrase::Initialize(rapidjson::Value & phrase)
{
	if (this->expr)
	{
		LOG_ERR("expr is not NULL");
		throw "expr is not NULL";
	}

	Value & val = phrase["id"];
	if (val.IsNull())
	{
		LOG_ERR("can't get 'id'");
		throw "can't get 'id'";
	}
	this->id = val.GetInt();

	val = phrase["model"];
	if (!val.IsNull())
	{
		if (!val.IsArray())
		{
			LOG_ERR("can't get 'model'");
			throw "can't get 'model'";
		}
		for(SizeType i = 0; i < val.Size(); i++)
		{
			this->model.push_back(val[i].GetString());
		}
	}

	val = phrase["pattern"];
	if (val.IsNull())
	{
		LOG_ERR("can't get 'pattern'");
		throw "can't get 'pattern'";
	}
	this->CompilePattern(val.GetString());

	val = phrase["help"];
	if (val.IsNull())
	{
		LOG_ERR("can't get 'help'");
		throw "can't get 'help'";
	}
	this->help = val.GetString();
}

QGroupID QPhrase::GetQGroupID()
{
	return this->group_id;
}

QPhraseID QPhrase::GetQPhraseID()
{
	return this->id;
}

string & QPhrase::GetHelp()
{
	return this->help;
}

ArgsModel QPhrase::FillModel(const string str, ArgsModel & model)
{
	int * vector = NULL;
	int vector_size = (this->model.size() + 1) * 2;

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
		return ArgsModel();
	}
	else if (count != vector_size / 2)
	{
		delete[] vector;
		LOG_DBG("pcre_exec 'size not equal': ", count, "/", vector_size / 2);
		return ArgsModel();
	}

	ArgsModel new_model = ArgsModel(this->id, this->group_id, model.GetArguments());
	auto it = this->model.begin();
	for (int i = 1; i < count && it != this->model.end(); i++, it++)
	{
		new_model[*it] = str.substr(vector[i * 2], vector[i * 2 + 1]);
	}

	delete[] vector;
	return new_model;
}

bool QPhrase::IsPartial(const string str)
{
	if (!this->expr)
	{
		LOG_ERR("expr is NULL");
		throw "expr is NULL";
	}
	int res = pcre_exec(this->expr, NULL, str.c_str(), str.length(), 0, PCRE_PARTIAL, NULL, 0);

	return (res == 0 || res == PCRE_ERROR_PARTIAL);
}

//QGroup source
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
	this->model = ArgsModel(this->id, val);

	val = json_doc["phrases"];
	if (val.IsNull() || !val.IsArray())
	{
		LOG_ERR("can't get 'phrases'");
		throw "can't get 'phrases'";
	}

	if (val.Size() > 0)
	{
		this->phrases.resize(val.Size());

		for(SizeType i = 0; i < val.Size(); i++)
		{
			Value & val_id = val[i]["id"];
			if (val_id.IsNull() || !val_id.IsInt())
			{
				LOG_ERR("can't get phrase 'id'");
				throw "can't get phrase 'id'";
			}

			QPhraseID id = val_id.GetInt();
			(*this)[id].Initialize(val[i]);
		}
	}
}

QGroup::~QGroup()
{
}

QGroupID QGroup::GetQGroupID()
{
	return this->id;
}

size_t QGroup::GetPhrasesCount()
{
	return this->phrases.size();
}

QPhrase & QGroup::operator[](QPhraseID key)
{
	key = QPhraseID_to_idx(key);
	if (key >= this->phrases.size())
	{
		LOG_ERR("can't get phrase by id: ", key);
		throw "can't get phrase by id";
	}

	return this->phrases[key];
}
