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

#include <cspeappsdk/cspeappagent/apperror.h>
#include "cspagent_app.h"

// hello_interval tag is the managed application parameter tag given while creating
// the application metadata by the Application Developer using the CSP Platform Application Portal
const CSP_STRING AgentApplication::HELLO_INTERVAL_PARAM_TAG = "control_settings.hello_interval"; 
const CSP_STRING AgentApplication::SUBSCRIBED_API_CALL_INTERVAL_PARAM_TAG = "app_subscribed_parameters.api_call_interval"; 

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
    // AppAgent object is the primary interface to all the workflow operations 
    // performed by the CSP Agent
    this->AGENT = std::unique_ptr<cspeapps::sdk::AppAgent>(new cspeapps::sdk::AppAgent());

    // Before even initializing the Agent, we will register the Signalling Callback because
    // the application will start to receive BE signals as soon as the Agent is initialized
    // to make sure we do not miss any BE signal, we will register our handler now.
    this->AGENT->RegisterBESignalCallback(std::bind(&AgentApplication::beSignallingRequest, this, std::placeholders::_1));

    // Initialize the agent. 
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

        // Once we are initialized successfully, our first task is to get the configuration of the application
        // from the BE so that we can start our application. 
        this->AGENT->GetConfiguration(std::bind(&AgentApplication::getConfigResponse, this, std::placeholders::_1));
    } else {
        log("Initialization Failed");
    }
}
CSP_VOID AgentApplication::getConfigResponse(cspeapps::sdk::AppConfig config)
{
    log("Received configuration from CSP Platform BE");
    // We will keep a copy of the Configuration object because we may need this while running the application.
    CONFIG.reset();
    CONFIG = std::unique_ptr<cspeapps::sdk::AppConfig>(new cspeapps::sdk::AppConfig(config));
    // Apply the new value
    log("Applying requested configuration");
    CSP_STRING helloIntervalReqVal = CONFIG->GetRequestedValue(HELLO_INTERVAL_PARAM_TAG);
    CSP_STRING helloIntervalCurVal = CONFIG->GetCurrentValue(HELLO_INTERVAL_PARAM_TAG);

    CSP_STRING apiCallIntervalReqVal = CONFIG->GetRequestedValue(SUBSCRIBED_API_CALL_INTERVAL_PARAM_TAG);
    CSP_STRING apiCallIntervalCurVal = CONFIG->GetCurrentValue(SUBSCRIBED_API_CALL_INTERVAL_PARAM_TAG);

    log("Requested Value of [" + HELLO_INTERVAL_PARAM_TAG + "] = [" + helloIntervalReqVal + "]");
    log("Current Value of [" + HELLO_INTERVAL_PARAM_TAG + "] = [" + helloIntervalCurVal + "]");

    log("Requested Value of [" + SUBSCRIBED_API_CALL_INTERVAL_PARAM_TAG + "] = [" + apiCallIntervalReqVal + "]");
    log("Current Value of [" + SUBSCRIBED_API_CALL_INTERVAL_PARAM_TAG + "] = [" + apiCallIntervalCurVal + "]");

    // If requested value is changed, there will be a value, otherwise we will
    // use the current value.
    bool report_config = false;
    if ( helloIntervalReqVal.length() > 0 ) {
        print_interval = atoi(helloIntervalReqVal.c_str());
        log("New Current Value of [" + HELLO_INTERVAL_PARAM_TAG + "] = " + std::to_string(print_interval));
        CONFIG->SetCurrentValue(HELLO_INTERVAL_PARAM_TAG, std::to_string(print_interval), "new value applied");
        report_config = true;
    } else {
        print_interval = atoi(helloIntervalCurVal.c_str());
    }

    if ( apiCallIntervalReqVal.length() > 0 ) {
        api_call_interval = atoi(apiCallIntervalReqVal.c_str());
        log("New Current Value of [" + SUBSCRIBED_API_CALL_INTERVAL_PARAM_TAG + "] = " + std::to_string(api_call_interval));
        CONFIG->SetCurrentValue(SUBSCRIBED_API_CALL_INTERVAL_PARAM_TAG, std::to_string(api_call_interval), "new value applied");
        report_config = true;
    } else {
        api_call_interval = atoi(apiCallIntervalCurVal.c_str());
    }

    // Application specific logic. Start our worker thread here.
    if ( !_bannerPrinter ) {
        _bannerPrinter = std::unique_ptr<std::thread>(new std::thread(std::bind(&AgentApplication::printBanner, this)));
    }

    // Now set the new current value. We will ensure to read the current value of any given parameter
    // from its original source (instead of just using the RequestedValue) to ensure the exact value
    // being used by the application. This will make sure correct reporting of current value at the BE

    // Report back newly applied value
    if ( report_config ) {
        log("We have received new values, reporting back newly applied configuration");
        this->AGENT->ReportConfiguration(*CONFIG, nullptr);
    }

    // Check if we have updated our configuration as part of BE Signal we will report
    // back the status of the update_configuration signal.
    if ( _lastJobId.length() > 0 ) {
        BE_REQUEST_STATUS reqStatus;
        reqStatus.jobid = _lastJobId;
        reqStatus.job_status = REQ_STATUS_CODE::SUCCESS;
        _lastJobId = "";

        if ( this->AGENT->ReportRequestStatus(reqStatus) ) {
            log("Request [" + reqStatus.jobid + "] status reported successfully");
        } else {
            log("Request [" + reqStatus.jobid + "] status failed to report");
        }
    }
}
CSP_VOID AgentApplication::beSignallingRequest(cspeapps::sdk::AppSignal signal)
{
    // CSP Platform BE Signal handler
    log("Received a signal from BE");
    _lastJobId = signal.GetJobId();

    // Currently we only have one operation as implemented below.
    std::string operation = signal.GetRequestedOperation();
    if ( operation == "update_configuration" ) {
        // This operation does not have any parameters, so we are just taking
        // appropriate action to service this request
        // Since the signal is asking us to update the configuration, so we will 
        // just call the GetConfiguration API again.
        this->AGENT->GetConfiguration(std::bind(&AgentApplication::getConfigResponse, this, std::placeholders::_1));
    } else if ( operation == "parameter_changed" ) {
        log("Some subscribed parameter has been changed. Take appropriate action");
        cspeapps::sdk::AppSignal::SIG_OP_PARAMS sig_params = signal.GetOperationParams();
        log("Parameter Changed = " + sig_params["parameter_name_1"]);
        this->AGENT->GetConfiguration(std::bind(&AgentApplication::getConfigResponse, this, std::placeholders::_1));
    } else if ( operation == "parameter_changed_aapp_with_payload" ) {
        log("P2P Signal received for subscribed parameters");
        cspeapps::sdk::AppSignal::SIG_PAYLOAD_PARAMS payload_params = signal.GetSignalParametersData();
        bool report_config = false;

        // We are expecting api_call_interval value to be changed
        if ( payload_params.find(SUBSCRIBED_API_CALL_INTERVAL_PARAM_TAG) != payload_params.end() ) {
            // Ensure that requested value is different that our current value for this parameter.
            api_call_interval = atoi(payload_params[SUBSCRIBED_API_CALL_INTERVAL_PARAM_TAG].reqValue.c_str());
            if ( CONFIG == nullptr ) {
                JsonParser::TreeMap tree;
                CONFIG = std::unique_ptr<cspeapps::sdk::AppConfig>(new cspeapps::sdk::AppConfig(tree));
            }
            CONFIG->SetCurrentValue(SUBSCRIBED_API_CALL_INTERVAL_PARAM_TAG, std::to_string(api_call_interval), "new value applied");
            report_config = true;
        }

        // We need to make sure we report the comitted value of parameters at once to 
        // avoid lazy reporting.
        if ( report_config ) {
            log("Reporting back newly applied configuration received in P2P Signal");
            this->AGENT->ReportConfiguration(*CONFIG, nullptr);
        }
    } 
    log("Signal handling completed");
}
CSP_VOID AgentApplication::printBanner()
{
    log("Starting Banner Printing");
    log("Printing Interval = " + std::to_string(print_interval));
    while ( isRunning ) {
        print("Hello World from CSP Agent Application. Subscribed Parameter Value = [" + std::to_string(api_call_interval) + "]");
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
