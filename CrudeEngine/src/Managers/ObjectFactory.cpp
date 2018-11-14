#include "ObjectFactory.h"
#include "GameObject.h"
#include "Components/Transform.h"
#include "GameObjectManager.h"
#include "Components/Body.h"
#include "Components/Follow.h"

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include "../Defines.h"

#include "../vendor/simplejson/JSON.h"

extern GameObjectManager *gpGameObjectManager;

ObjectFactory::ObjectFactory()
{

}

ObjectFactory::~ObjectFactory()
{

}

void ObjectFactory::LoadLevel(const char *pFileName)
{

	std::string fullPath = "res/data/";
	fullPath += pFileName;
	std::ifstream file(fullPath);
	std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	JSONValue *value = JSON::Parse(contents.c_str());
	if (value == NULL)
	{
		printf("value is null");
	}
	JSONObject root = value->AsObject();
	JSONArray objects = root[L"Objects"]->AsArray();
	int type;
	std::string FileName;
	for (unsigned int i = 0; i < objects.size(); ++i)
	{
		JSONObject obj = objects[i]->AsObject();
		if (obj.find(L"File") != obj.end())
		{
			std::wstring wt = obj[L"File"]->AsString();
			FileName = std::string(wt.begin(), wt.end());
		}
		if (obj.find(L"Type") != obj.end())
		{
			std::wstring wt = obj[L"Type"]->AsString();
			std::string tt(wt.begin(), wt.end());
			if ("Player" == tt)
				type = PLAYER;
			else if ("Enemy" == tt)
				type = CRAWLER;
			else
				type = NO_OBJECT;

		}
		GameObject *pGameObject = LoadObject(FileName.c_str(), type);

		if (obj[L"Transform"]->IsObject()) {
			Transform *pTr = static_cast<Transform*>(pGameObject->GetComponent(TRANSFORM));
			pTr->Serialize(obj[L"Transform"]->AsObject());

			Body *pBody = static_cast<Body*>(pGameObject->GetComponent(BODY));
			pBody->Initialize();
		}
	}


}

GameObject* ObjectFactory::LoadObject(const char *pFileName, int type)
{
	GameObject *pNewGameObject = nullptr;

	std::string fullPath = "res/data/";
	fullPath += pFileName;
	std::ifstream file(fullPath);
	std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	JSONValue *value = JSON::Parse(contents.c_str());

	if (value->IsObject()) {

		pNewGameObject = new GameObject(type);
		Component *pNewComponent = nullptr;

		JSONObject root = value->AsObject();
		if (root.find(L"Transform") != root.end())
		{
			pNewComponent = pNewGameObject->AddComponent(TRANSFORM);
			pNewComponent->Serialize(root[L"Transform"]->AsObject());
		}
		if (root.find(L"Controller") != root.end())
		{
			pNewComponent = pNewGameObject->AddComponent(CONTROLLER);
		}
		if (root.find(L"Body") != root.end())
		{
			pNewComponent = pNewGameObject->AddComponent(BODY);
			pNewComponent->Serialize(root[L"Body"]->AsObject());
		}
		if (root.find(L"Animator") != root.end())
		{
			pNewComponent = pNewGameObject->AddComponent(ANIMATOR);
			pNewComponent->Serialize(root[L"Animator"]->AsObject());
		}
		if (root.find(L"Sprite") != root.end())
		{
			pNewComponent = pNewGameObject->AddComponent(SPRITE);
			pNewComponent->Serialize(root[L"Sprite"]->AsObject());
		}
		if (root.find(L"Follow") != root.end())
		{
			pNewComponent = pNewGameObject->AddComponent(FOLLOW);
			Follow *pFollow = static_cast<Follow*>(pNewComponent);
			pFollow->Initialize();
		}
		if (root.find(L"FallExplode") != root.end())
		{
			pNewComponent = pNewGameObject->AddComponent(FALLEXPLODE);
			pNewComponent->Serialize(root[L"FallExplode"]->AsObject());
		}
		gpGameObjectManager->mGameObjects.push_back(pNewGameObject);
	}
	
	return pNewGameObject;
}