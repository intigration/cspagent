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
#include <functional>
#include <iostream>

#include <ctype.h>
#include <signal.h>
#include <unistd.h>

#include "cspagent_app.h"

AgentApplication::AgentApplication() : AGENT(nullptr), isRunning(true)
{

}

AgentApplication::~AgentApplication()
{
    isRunning = false;
}

bool AgentApplication::initialize()
{
   this->AGENT = std::unique_ptr<cspeapps::sdk::AppAgent>(new cspeapps::sdk::AppAgent());
   this->AGENT->Initialize(std::bind(&AgentApplication::initializeResponse, this, std::placeholders::_1));
}

void AgentApplication::log(const std::string &msg)
{
    std::cout << "[CSPAGENT-HELLO] : " << msg << std::endl;
}

CSP_VOID AgentApplication::initializeResponse(const InitResponse &res)
{
    if ( res.status ) {
        log("Initialized Successfully");
        log("Getting Application Configuration from BE");
        this->AGENT->GetConfiguration(std::bind(&AgentApplication::getConfigResponse, this, std::placeholders::_1));
    } else {
        log("Initialization Failed");
    }
}
CSP_VOID AgentApplication::getConfigResponse(const BackendResponse &res)
{
    log("GetConfiguration Response from CSP Platform BE");
    log("Response = [" + res.response + "]");
    log("GetConfiguration Response End");
}
