**Introduction**

CSP Embedded Managed Applications are application which are executing on CSP Gateway Device and can be
managed from CSP Platform Application Portal. There are two types of management operations that can be
performed on an application

    1) Container Management Operation: These are the operations which defines the life cycle of the 
       application object. Those include deploying an application on a gateway device, starting an
       application, stopping an application and so on.

    2) Application Management Operations: These are the operations of the management of the application
       itself. Example include the change of some internal parameter of the application, or performing 
       some operation by the application and so on.

Every CSP Embedded Managed Application will support "Container Management Operations" by default, such
an application will be called "Unmanaged Application" but any CSP Embedded Managed Application can 
support "Application Management Operations", such an application will be called "Managed Application". 

Supporting the Application Managed Operation will require some additional support to be added in the 
application using the CSP Embedded Managed Application Agent SDK. An application using this SDK is said
to have an CSP Application Agent. 

An ideal workflow implemented by the application for CSP Application Agent should be 


                                                Update Configuration Signal
                                                             |
                                                             |
                                                             V
    ------------> Application Start ----> Initialize Agent ----> Get Configuration ----
                                                                                      |
                                                                                      |
    <--- Do Application Work <---- Report Configuration <---- Apply Configuration <---V

CSP Application Agent SDK Provide different classes and interfaces to execute above mentioned workflow.

## Application Configuration. 

CSP Platform BE tries to ensure that the application configuration object at
the CSP Platform BE and the application itself is always in sync. To ensure this, BE uses the concept of
Current Value & Requested Value. Whenever some value of any application parameter changes from the BE
using CSP Platform Application Portal, this change will be shown as Requested Value. The Current Value
represent the currently used value by the actual application. When the user pushes this new value to the
application, the application get this new value as Requested Value. The application will apply the newly
Requested Value. Once it is applied successfully (or even not), the new value will becomes the Current Value
of the application. The application then will report this new Current Value to CSP Platform BE. Once 
reported the CSP Platform BE will move the new Requested Value to the Current Value.


## Seting Up
CSP Application Agent SDK will be installed using CSP Embedded Managed Application SDK. The SDK will
take care of making Agent SDK part of the application runtime image. 

    Includes = -I/usr/local/mgc/include
    Library  = -L/usr/local/mgc/lib -lcspeappagent -lboost_system

## Header Files 

    #include <cspeappsdk/cspeappagent/appagent.h>
    #include <cspeappsdk/cspeappagent/appconfig.h>
    #include <cspeappsdk/cspeappagent/apperror.h>
    #include <cspeappsdk/cspeappagent/appsignal.h>

## Classes 

AppAgent

This is the primary class to provide the above mentioned workflow. Important interfaces of the class are

    AppAgent::Initialize: Initializes the application agent. This should be the first step
    AppAgent::GetConfiguration: Get application configuration from the CSP Platform BE
    AppAgent::ReportConfiguration: Report application configuration to cSP Platform BE
    AppAgent::RegisterBESignalCallback: Register BE signal handler callback to handle CSP Platform BE Signals
    AppAgent::ReportRequestStatus: Report the status of BE Request received in BE signal.

AppConfig

This class represent the application configuration it has received from CSP Platform BE as response to
AppAgent::GetConfiguration API call. Important interfaces of the class are

    AppConfig::GetRequestedValue: Get the requested value
    AppConfig::GetCurrentValue: Get the current value
    AppConfig::SetCurrentValue: Set the current value

AppSignal

This class represent the incoming CSP Platform BE Signal for the application. This signal is asynchronous
in nature so to handle this signal, the application should provide a handler callback to the agent as soon
as it is initialized.

    AppSignal::GetJobId: Get the job id of the incoming signal. This job id will be used while reporting the
                         status of the operation
    AppSignal::GetRequestedOperation: Get the operation requested by CSP Platform BE in the incoming signal.

## CSP Agent Hello

CSP Agent Hello is a sample application to demonstrate the use of CSP Agent SDK. This application will 
print a "Hello World" banner on the standard output with an interval that can be managed by the 
CSP Platform Application Portal. This application is available on the CSP Application Store with following
details

    Name: Hello CSP Embedded Managed Application Agent
    Image: sample-cspagent-hello:1.0
    Type: Managed

    Managed Application Parameters:
        Greetings Message Interval: Default Value (1)

# Usage
Once application is deployed and started, following command when issued on the CSP Gateway Device 
will show the standard output of the application where you can notice the Hello banner printed by 
the application along with timestamp. Change the print interval from CSP Platform Application portal
to notice the change in the banner printing interval.

    docker logs <container_id>
