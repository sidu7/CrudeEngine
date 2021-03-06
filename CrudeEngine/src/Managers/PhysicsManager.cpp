/* Start Header -------------------------------------------------------

Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute
of Technology is prohibited.
File Name:		PhysicsManager.cpp
Purpose:		Physics Manager to handle Collisions
Language:		C++
Platform:		Visual Studio 2017 | Visual C++ 14.1 | Windows 10 Home
Project:		CS529_sidhantt_FinalProject
Author:			Sidhant Tumma | sidhant.t | 60002218
Creation date:	12/04/2018

- End Header --------------------------------------------------------*/

#include "PhysicsManager.h"
#include "../Components/Body.h"
#include "../Components/Transform.h"
#include "GameObject.h"
#include "GameObjectManager.h"
#include "Events.h"
#include "../Components/Attributes.h"
#include "../Components/Buff.h"

#include "../Defines.h"

extern GameObjectManager *gpGameObjectManager;
extern CollisionManager *gpCollisionManager;

#define GO gpGameObjectManager->mGameObjects

PhysicsManager::PhysicsManager()
{
}
PhysicsManager::~PhysicsManager()
{

}

void PhysicsManager::Update(float FrameTime)
{
	
	for (auto go : gpGameObjectManager->mGameObjects)
	{
		Body *pBody = static_cast<Body*>(go->GetComponent(BODY));
		if (pBody != nullptr)
			pBody->Integrate(0.0, FrameTime);
	}

	//Reset previous contacts
	gpCollisionManager->Reset();

	//Check for intersections

	for (int pObj1 = 0; pObj1 < (int)GO.size(); ++pObj1)
	{
		if (GO[pObj1]->mType == NO_OBJECT)
		{
			continue;
		}

		Body *pBody1 = static_cast<Body*>(GO[pObj1]->GetComponent(BODY));

		if (pBody1 == nullptr)
			continue;

		for (int pObj2 = pObj1 + 1; pObj2 < (int)GO.size(); ++pObj2)
		{
			if (GO[pObj2]->mType == NO_OBJECT)
			{
				continue;
			}

			Body *pBody2 = static_cast<Body*>(GO[pObj2]->GetComponent(BODY));

			if (pBody2 == nullptr)
				continue;

			gpCollisionManager->checkCollisionAndGenerateContact(pBody1->mpShape, pBody1->mPosition.x, pBody1->mPosition.y,
				pBody2->mpShape, pBody2->mPosition.x, pBody2->mPosition.y, gpCollisionManager->mContacts);
		}
	}

	//Super Advanced Physics here!!!
	for (auto c : gpCollisionManager->mContacts)
	{
		if ((c->mBodies[0]->mpOwner->mType == GHOUL || c->mBodies[0]->mpOwner->mType == CRAWLER) && 
			c->mBodies[1]->mpOwner->mType == BULLET)
		{
			TakeDamage *td = new TakeDamage();
			Attributes *pAttr = static_cast<Attributes*>(c->mBodies[1]->mpOwner->GetComponent(ATTRIBUTES));
			td->DamageDealt = pAttr->mDamage;
			c->mBodies[0]->mpOwner->HandleEvent(td);
			gpGameObjectManager->Destroy(c->mBodies[1]->mpOwner);

		}
		else if (c->mBodies[1]->mpOwner->mType == WALL && (c->mBodies[0]->mpOwner->mType == PLAYER || c->mBodies[0]->mpOwner->mType == GRENADE))
		{
			WallCollideEvent *wc = new WallCollideEvent();
			if (c->mBodies[1]->mPosition.y > SCREEN_HEIGHT / 2)
				wc->side = 1;
			else if (c->mBodies[1]->mPosition.y < SCREEN_HEIGHT / 2)
				wc->side = 3;
			else if (c->mBodies[1]->mPosition.x < SCREEN_WIDTH / 2)
				wc->side = 0;
			else if (c->mBodies[1]->mPosition.x > SCREEN_WIDTH / 2)
				wc->side = 2;
			c->mBodies[0]->mpOwner->HandleEvent(wc);
		}
		else if (c->mBodies[0]->mpOwner->mType == WALL && c->mBodies[1]->mpOwner->mType == BULLET)
		{
			gpGameObjectManager->Destroy(c->mBodies[1]->mpOwner);
		}
		else if (c->mBodies[0]->mpOwner->mType == TOMBSTONE && c->mBodies[1]->mpOwner->mType == BULLET)
		{
			TakeDamage *td = new TakeDamage();
			Attributes *pAttr = static_cast<Attributes*>(c->mBodies[1]->mpOwner->GetComponent(ATTRIBUTES));
			td->DamageDealt = pAttr->mDamage;
			c->mBodies[0]->mpOwner->HandleEvent(td);
			gpGameObjectManager->Destroy(c->mBodies[1]->mpOwner);
		}
		else if (c->mBodies[0]->mpOwner->mType == GRENADE && (c->mBodies[1]->mpOwner->mType == GHOUL ||
			c->mBodies[1]->mpOwner->mType == CRAWLER))
		{
			Transform *pTr = static_cast<Transform*>(c->mBodies[0]->mpOwner->GetComponent(TRANSFORM));
			if (pTr->mScale.x == 200.0f)
			{
				GrenadeHit gh;
				c->mBodies[1]->mpOwner->HandleEvent(&gh);
			}
		}
		else if ((c->mBodies[0]->mpOwner->mType == GHOUL || c->mBodies[0]->mpOwner->mType == CRAWLER) && 
			c->mBodies[1]->mpOwner->mType == GRENADE)
		{
			Transform *pTr = static_cast<Transform*>(c->mBodies[1]->mpOwner->GetComponent(TRANSFORM));
			if (pTr->mScale.x == 200.0f)
			{
				GrenadeHit gh;
				c->mBodies[0]->mpOwner->HandleEvent(&gh);
			}
		}
		else if (c->mBodies[0]->mpOwner->mType == PLAYER && (c->mBodies[1]->mpOwner->mType == GHOUL 
			|| c->mBodies[1]->mpOwner->mType == CRAWLER))
		{
			gpGameObjectManager->Destroy(c->mBodies[1]->mpOwner);
			TakeDamage *td = new TakeDamage();
			Attributes *pAtt = static_cast<Attributes*>(c->mBodies[1]->mpOwner->GetComponent(ATTRIBUTES));
			td->DamageDealt = pAtt->mDamage;
			c->mBodies[0]->mpOwner->HandleEvent(td);
		}
		else if (c->mBodies[0]->mpOwner->mType == PLAYER && c->mBodies[1]->mpOwner->mType == DROPITEM)
		{
			DropPicked *dp = new DropPicked();
			Buff *pBuff = static_cast<Buff*>(c->mBodies[1]->mpOwner->GetComponent(BUFF));
			dp->Drop = pBuff->DropType;
			c->mBodies[0]->mpOwner->HandleEvent(dp);
			gpGameObjectManager->Destroy(c->mBodies[1]->mpOwner);
		}
	}
}