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
   this->AGENT->RegisterBESignalCallback(std::bind(&AgentApplication::beSignallingRequest, this, std::placeholders::_1));
   this->AGENT->Initialize(std::bind(&AgentApplication::initializeResponse, this, std::placeholders::_1));
}

void AgentApplication::log(const std::string &msg)
{
    std::cout << "[CSPAGENT-HELLO] : " << msg << std::endl;
}

CSP_VOID AgentApplication::initializeResponse(const INIT_RESPONSE &res)
{
    if ( res.status ) {
        log("Initialized Successfully");
        log("Getting Application Configuration from BE");
        this->AGENT->GetConfiguration(std::bind(&AgentApplication::getConfigResponse, this, std::placeholders::_1));
    } else {
        log("Initialization Failed");
    }
}
CSP_VOID AgentApplication::getConfigResponse(cspeapps::sdk::AppConfig &config)
{
    log("GetConfiguration Response from CSP Platform BE");
    CSP_STRING config_str;
    config.ToString(config_str);
    log("Response = [" + config_str + "]");
    log("GetConfiguration Response End");
}
CSP_VOID AgentApplication::beSignallingRequest(cspeapps::sdk::AppSignal &signal)
{
    log("Received a signal from BE");
    log("Signal Job Id = [" + signal.GetJobId() + "]");
    log("Signal Operation = [" + signal.GetRequestedOperation() + "]");
    cspeapps::sdk::AppSignal::SIG_OP_PARAMS op_params = signal.GetOperationParams();
    for ( auto param : op_params ) {
        log("Operation Param Key = " + param.first + " Value = " + param.second);
    }
    log("Operation Parameters Complete");
}
