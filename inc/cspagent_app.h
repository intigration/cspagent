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

#include <cspeappsdk/cspeappagent/appagent.h>

class AgentApplication {
public:
    AgentApplication();
    ~AgentApplication();
    bool initialize();
public:
    cspeapps::sdk::AppAgent *AGENT;
};

#endif /* _CSPBOX_CONF_APP_H_ */
