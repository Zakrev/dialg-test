#include "dialg_common.h"
#include "logs.h"

#include <iterator>

using namespace Dialg;
using namespace rapidjson;

//ArgsModel source
ArgsModel::ArgsModel()
{
	this->invalid = 1;
}

ArgsModel::ArgsModel(QPhraseID qPhraseId, QGroupID qGroupId, map<string, string> & args)
{
	this->invalid = 0;
	this->qGroupId = qGroupId;
	this->qPhraseId = qPhraseId;
	this->args = args;
}

ArgsModel::ArgsModel(QGroupID qGroupId, Value & json_model)
{
	this->invalid = 0;
	this->qGroupId = qGroupId;

	if (json_model.IsNull())
	{
		return;
	}

	if (!json_model.IsArray())
	{
		LOG_ERR("can't read 'model': not array");
		throw "can't read 'model': not array";
	}

	for(SizeType i = 0; i < json_model.Size(); i++)
	{
		Value & obj = json_model[i];

		if (obj["name"].IsNull())
		{
			LOG_ERR("can't read 'model->obj->name': ", i);
			throw "can't read 'model->obj->name'";
		}

		if (obj["value"].IsNull())
		{
			LOG_ERR("can't read 'model->obj->value': ", i);
			throw "can't read 'model->obj->value'";
		}

		this->args[obj["name"].GetString()] = obj["value"].GetString();
	}
}

ArgsModel::~ArgsModel()
{

}

bool ArgsModel::IsValid()
{
	return this->invalid == 0;
}

QGroupID ArgsModel::GetQGroupID()
{
	if (!this->IsValid())
	{
		LOG_ERR("this model is invalid can't get QGroupID");
		throw "this model is invalid can't get QGroupID";
	}
	return this->qGroupId;
}

QPhraseID ArgsModel::GetQPhraseID()
{
	if (!this->IsValid())
	{
		LOG_ERR("this model is invalid can't get QPhraseID");
		throw "this model is invalid can't get QPhraseID";
	}
	return this->qPhraseId;
}

map<string, string> & ArgsModel::GetArguments()
{
	if (!this->IsValid())
	{
		LOG_ERR("this model is invalid can't get arguments");
		throw "this model is invalid can't get arguments";
	}
	return this->args;
}

string & ArgsModel::operator[](string key)
{
	auto it = this->args.find(key);

	if (it == this->args.end())
	{
		LOG_DBG("find 'key not found': ", key);
		throw "key not found";
	}

	return (*it).second;
}
