/* Start Header -------------------------------------------------------

Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute
of Technology is prohibited.
File Name:		Subscription.h
Purpose:		Header file for Subscription Component
Language:		C++
Platform:		Visual Studio 2017 | Visual C++ 14.1 | Windows 10 Home
Project:		CS529_sidhantt_FinalProject
Author:			Sidhant Tumma | sidhant.t | 60002218
Creation date:	12/04/2018

- End Header --------------------------------------------------------*/

#pragma once

#include "Component.h"
#include <vector>

class Subscription : public Component
{
public:
	Subscription();
	~Subscription();

	void Update();
	void Serialize(JSONObject obj);
	Component* Create();

public:
	std::vector<unsigned int> mEventSubscriptions;

};