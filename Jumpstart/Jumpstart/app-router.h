#pragma once
#include "router.h"
#include "init-controller.h"

class AppRouter : public Router
{
public:
	void setRoutes() override;

	//-----------Define Controllers---------//
	InitController initController;
};