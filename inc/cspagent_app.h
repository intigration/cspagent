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
#include <cspeappsdk/cspeappagent/appagent.h>

class AgentApplication {
public:
    AgentApplication();
    ~AgentApplication();
    bool initialize();
    void log(const std::string &msg);

    // CSP Application Agent Callback Handlers
    CSP_VOID initializeResponse(const InitResponse &res);
    CSP_VOID getConfigResponse(const BackendResponse &res);
private:
public:
    std::unique_ptr<cspeapps::sdk::AppAgent> AGENT;
    volatile bool isRunning;
};

#endif /* _CSPBOX_CONF_APP_H_ */
