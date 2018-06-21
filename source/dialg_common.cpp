#include "dialg_common.h"
#include "logs.h"

#include <iterator>

using namespace Dialg;
using namespace rapidjson;

//ArgsModel source
ArgsModel::ArgsModel()
{

}

ArgsModel::ArgsModel(Value & json_model)
{
	if (json_model.IsNull() || !json_model.IsArray())
	{
		LOG_ERR("can't get 'model'");
		throw "can't get 'model'";
	}

	for(SizeType i = 0; i < json_model.Size(); i++)
	{
		Value & obj = json_model[i];

		if (obj["name"].IsNull())
		{
			LOG_ERR("can't get 'model->obj->name': ", i);
			throw "can't get 'model->obj->name'";
		}

		if (obj["value"].IsNull())
		{
			LOG_ERR("can't get 'model->obj->value': ", i);
			throw "can't get 'model->obj->value'";
		}

		this->args[obj["name"].GetString()] = obj["value"].GetString();
	}
}

ArgsModel::~ArgsModel()
{

}

void ArgsModel::insert(string key, string val)
{
	this->args[key] = val;
}
