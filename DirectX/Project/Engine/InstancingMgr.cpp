#pragma once

#include "CSingleton.h"

#include "ptr.h"

class InstancingMgr 
	: public CSingleton<InstancingMgr>
{
	SINGLE(InstancingMgr);
private:



};