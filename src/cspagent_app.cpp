/***********************************************************************
 *
 *            Copyright 2010 Mentor Graphics Corporation
 *                         All Rights Reserved.
 *
 * THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION WHICH IS
 * THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS LICENSORS AND IS
 * SUBJECT TO LICENSE TERMS.
 *
 ************************************************************************

 ************************************************************************
 *
 * FILE NAME
 *
 *       cspagent_app.cpp
 *
 * DESCRIPTION
 *
 *
 ***********************************************************************/
#include "cspagent_app.h"

AgentApplication::AgentApplication()
{

}

AgentApplication::~AgentApplication()
{

}

bool AgentApplication::initialize()
{
   this->AGENT = new cspeapps::sdk::AppAgent();
}

int main()
{
   AgentApplication APP;
   APP.initialize();
   APP.AGENT->initializeAgent();
   usleep(10000000);

   return 0;
}
