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
#include <ctime>
#include <functional>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>

#include <ctype.h>
#include <signal.h>
#include <unistd.h>

#include "cspagent_app.h"

const CSP_STRING AgentApplication::HELLO_INTERVAL_PARAM_TAG = "hello_interval"; 

AgentApplication::AgentApplication() : AGENT(nullptr), 
    _bannerPrinter(nullptr), _lastJobId(""), print_interval(10), isRunning(true) 
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
        log("Agent initialized successfully");
        log("Getting Application Configuration from CSP Platform BE");
        this->AGENT->GetConfiguration(std::bind(&AgentApplication::getConfigResponse, this, std::placeholders::_1));
    } else {
        log("Initialization Failed");
    }
}
CSP_VOID AgentApplication::getConfigResponse(cspeapps::sdk::AppConfig config)
{
    log("Received configuration from CSP Platform BE");
    CONFIG.reset();
    CONFIG = std::unique_ptr<cspeapps::sdk::AppConfig>(new cspeapps::sdk::AppConfig(config));
    // Apply the new value
    log("Applying requested configuration");
    CSP_STRING reqVal = CONFIG->GetRequestedValue(HELLO_INTERVAL_PARAM_TAG);
    CSP_STRING curVal = CONFIG->GetCurrentValue(HELLO_INTERVAL_PARAM_TAG);

    // If requested value is changed, there will be a value, otherwise we will
    // use the current value.
    if ( reqVal.length() > 0 ) {
        print_interval = atoi(reqVal.c_str());
    } else {
        print_interval = atoi(curVal.c_str());
    }

    // Start our worker thread here.
    if ( !_bannerPrinter ) {
        _bannerPrinter = std::unique_ptr<std::thread>(new std::thread(std::bind(&AgentApplication::printBanner, this)));
    }

    // Now set the new current value
    log("Setting new current value");
    CONFIG->SetCurrentValue(HELLO_INTERVAL_PARAM_TAG, std::to_string(print_interval), "new value applied");

    // Report back newly applied value
    log("Reporting back newly applied configuration");
    this->AGENT->ReportConfiguration(*CONFIG, nullptr);

    // Check if we have updated our configuration as part of BE Signal
    if ( _lastJobId.length() > 0 ) {
        BE_REQUEST_STATUS reqStatus;
        reqStatus.jobid = _lastJobId;
        reqStatus.job_status = REQ_STATUS_CODE::SUCCESS;
        _lastJobId = "";

        if ( this->AGENT->ReportRequestStatus(reqStatus).status ) {
            log("Request [" + reqStatus.jobid + "] status reported successfully");
        } else {
            log("Request [" + reqStatus.jobid + "] status failed to report");
        }
    }
}
CSP_VOID AgentApplication::beSignallingRequest(cspeapps::sdk::AppSignal signal)
{
    log("Received a signal from BE");
    _lastJobId = signal.GetJobId();
    if ( signal.GetRequestedOperation() == "update_configuration" ) {
        // This operation does not have any parameters, so we are just taking
        // appropriate action to service this request
        this->AGENT->GetConfiguration(std::bind(&AgentApplication::getConfigResponse, this, std::placeholders::_1));
    }
    log("Signal handling completed");
}
CSP_VOID AgentApplication::printBanner()
{
    log("Starting Banner Printing");
    log("Printing Interval = " + std::to_string(print_interval));
    while ( isRunning ) {
        print("Hello World from CSP Agent Application");
        std::this_thread::sleep_for(std::chrono::seconds(print_interval));
    }
    log("Exiting Banner Printing");
}
CSP_VOID AgentApplication::print(const CSP_STRING &msg)
{
    std::time_t now = std::time(nullptr);
    std::stringstream _time_ss;
    _time_ss << "[" << std::put_time(std::localtime(&now), "%c") << "]";
    CSP_STRING final_message = _time_ss.str() + " " + msg;
    log(final_message);
}
