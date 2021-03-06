/* Start Header -------------------------------------------------------

Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute
of Technology is prohibited.
File Name:		GameObject.cpp
Purpose:		Component-based GameObject template
Language:		C++
Platform:		Visual Studio 2017 | Visual C++ 14.1 | Windows 10 Home
Project:		CS529_sidhantt_FinalProject
Author:			Sidhant Tumma | sidhant.t | 60002218
Creation date:	12/04/2018

- End Header --------------------------------------------------------*/

#include "GameObject.h"
#include "../Components/Sprite.h"
#include "../Components/Transform.h"
#include "../Components/Controller.h"
#include "../Components/Body.h"
#include "../Components/Animator.h"
#include "ResourceManager.h"
#include "GameObjectManager.h"
#include "EventManager.h"

extern GameObjectManager *gpGameObjectManager;
extern ResourceManager *gpResourceManager;

GameObject::GameObject(GameObjectTypes type) : mType(type), mDeathDelay(0.0f), Destroyed(false)
{
	Vector2DZero(&mTempScale);
}


GameObject::~GameObject()
{
	for(auto c: mComponents)
		delete c;
	mComponents.clear();
}

bool GameObject::Update()
{	
	for (unsigned int i = 0; i < mComponents.size(); ++i)
		mComponents[i]->Update();
	return Destroyed;
}

Component* GameObject::AddComponent(unsigned int Type)
{
	Component *pNewComponent = nullptr;
	pNewComponent = gpGameObjectManager->mComponentMap[Type]->Create();
	
	if(pNewComponent != nullptr)
	{
		mComponents.push_back(pNewComponent);
		pNewComponent->mpOwner = this;
	}
	return pNewComponent;
}

Component* GameObject::GetComponent(unsigned int Type)
{
	for(auto c : mComponents)
		if(c->GetType() == Type)
			return c;
		
	return nullptr;
}

void GameObject::RemoveComponent(unsigned int Type)
{
	for (unsigned int i = 0; i < mComponents.size(); ++i)
		if (mComponents[i]->GetType() == Type)
		{
			mComponents.erase(mComponents.begin() + i);
			break;
		}
}

void GameObject::SetTransform(float posX, float posY, float scaleX, float scaleY, float angle)
{
	Transform* pTr = static_cast<Transform*>(GetComponent(TRANSFORM));
	
	pTr->mPosition.x = posX;
	pTr->mPosition.y = posY;
	pTr->mScale.x = scaleX;
	pTr->mScale.y = scaleY;
	pTr->mAngle = angle;
}

void GameObject::SetSprite(const char* filePath)
{
	Sprite* pSp = static_cast<Sprite*>(GetComponent(SPRITE));

	pSp->mpTexture = gpResourceManager->LoadSurface(filePath);
}

void GameObject::HandleEvent(Event * pEvent)
{
	for (unsigned int i = 0; i < mComponents.size(); ++i)
	{
		mComponents[i]->HandleEvent(pEvent);
	}
}