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

// ============================================================================
static void registerSignals(void);
static void cleanExit(void);
static void signalled(int signal);
// ============================================================================
static std::unique_ptr<AgentApplication> application;
// ============================================================================
// M A I N    E N T R Y
// ============================================================================
int main()
{
    application = std::unique_ptr<AgentApplication>(new AgentApplication());
    
    // Register signals we are interested to capture
    registerSignals();
    
    // Initialize CSP Embedded Managed Application Agent
    application->initialize();

    // Do the stuff
    while ( application->isRunning ) {
        sleep(1);
    }

    // We are existing now
    application->log("Exiting...");

   return 0;
}
void registerSignals(void)
{
	// Register exit signal handler
	atexit(cleanExit);

	// Trap signals for a clean exit
	signal(SIGQUIT, signalled);
	signal(SIGABRT, signalled);
	signal(SIGTERM, signalled);
	signal(SIGINT, signalled);

	// We will ignore the Broken Pipe signal otherwise our
	// reconnection logic will not work and during reconnection
	// the application will crash.
	signal(SIGPIPE, SIG_IGN);
}
void signalled(int signal)
{
	if ( application ) {
        std::cout << "Stopping Application...\n";
		application->isRunning = false;
	}
}
void cleanExit(void)
{
	if ( application ) {
        std::cout << "Stopping Application...\n";
		application->isRunning = false;
	}
}
