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
*       cspagent_app.h
*
* DESCRIPTION
*
***********************************************************************/
#ifndef _CSPBOX_CONF_APP_H_
#define _CSPBOX_CONF_APP_H_

#include <memory>
#include <thread>
#include <cspeappsdk/cspeappagent/appagent.h>
#include <cspeappsdk/cspeappagent/appconfig.h>

class AgentApplication {
public:
    AgentApplication();
    ~AgentApplication();
    bool initialize();
    void log(const std::string &msg);

    // CSP Application Agent Callback Handlers
    CSP_VOID initializeResponse(const INIT_RESPONSE &res);
    CSP_VOID getConfigResponse(cspeapps::sdk::AppConfig config);
    CSP_VOID beSignallingRequest(cspeapps::sdk::AppSignal signal);
private:
    CSP_VOID printBanner();
    CSP_VOID print(const CSP_STRING &msg);
public:
    std::unique_ptr<cspeapps::sdk::AppAgent> AGENT;
    std::unique_ptr<cspeapps::sdk::AppConfig> CONFIG;
    std::unique_ptr<std::thread> _bannerPrinter;
    CSP_STRING _lastJobId;
    volatile int print_interval;
    volatile int api_call_interval;
    volatile bool isRunning;
private:
    static const CSP_STRING HELLO_INTERVAL_PARAM_TAG; 
    static const CSP_STRING SUBSCRIBED_API_CALL_INTERVAL_PARAM_TAG; 
};

#endif /* _CSPBOX_CONF_APP_H_ */
