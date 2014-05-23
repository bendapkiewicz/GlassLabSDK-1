//
//  glsdk_core.h
//  Glasslab SDK
//
//  Authors:
//      Joseph Sutton
//      Ben Dapkiewicz
//
//  Copyright (c) 2014 Glasslab. All rights reserved.
//

/* ---------------------------- */
#ifndef GLASSLABSDK_CORE
#define GLASSLABSDK_CORE
/* ---------------------------- */


// The classes below are not exported
#pragma GCC visibility push(hidden)

using namespace std;

#include "glsdk_const.h"
#include "glsdk_data_sync.h"


namespace nsGlasslabSDK {

    class  Core;


    typedef struct _p_glSDKInfo p_glSDKInfo;

    typedef void(*CoreCallback_Func)(p_glSDKInfo);

    typedef struct _coreCallbackStructure {
        CoreCallback_Func coreCB;
        bool cancel;
    } coreCallbackStructure;

    typedef struct _glConfig {
        int eventsPeriodSecs;
        int eventsMinSize;
        int eventsMaxSize;
        int eventsDetailLevel;
    } glConfig;

    typedef struct _p_glSDKInfo {
        GlasslabSDK*        sdk;
        Core*               core;
        string              data;
        ClientCallback_Func clientCB;
        void*               userData;
    } p_glSDKInfo;

    typedef struct _glHttpRequest {
        GlasslabSDK*                sdk;
        Core*                       core;
        struct event_base*          base;
        struct evhttp_connection*   conn;
        struct evhttp_request*      req;
        string                      coreCBKey;
        ClientCallback_Func         clientCB;
        int                         msgQRowId;
    } p_glHttpRequest;


    // used for client connection (get config), login, start/end session
    //   - future feature: set/get client data (cloud saves)
    // TODO: write simple c++ wrapper libevent
    // TODO: check for de-allocate pointers!!
    class Core {
        public:
            Core( GlasslabSDK* client, const char* dataPath, const char* gameId, const char* deviceId = NULL, const char* uri = NULL );
            ~Core();

            // Message stack functions
            void pushMessageStack( Const::Message msg, const char* data = "{}" );
            Const::Response* popMessageStack();
        
            // API functions
            int connect( const char* gameId, const char* uri = NULL );
            int setUserInfo( const char* json );
            void deviceUpdate();
            void authStatus();
            void registerStudent( const char* username, const char* password, const char* firstName, const char* lastInitial, string cb = "" );
            void registerInstructor( const char* name, const char* email, const char* password, bool newsletter = true, string cb = "" );
            void login( const char* username, const char* password, const char* type = NULL, string cb = "" );
            void enroll( const char* courseCode, string cb = "" );
            void unenroll( const char* courseId, string cb = "" );
            void getCourses( string cb = "" );
            void logout( string cb = "" );
            void startSession( string cb = "" );
            void endSession( string cb = "" );
            void saveGame(const char* gameData, string cb = "");
            void sendTelemEvents( string clientCB = "", string coreCB = "" );
            void mf_httpGetRequest( string path, string coreCB, string clientCB = "", string postdata = "", const char* contentType = NULL, int rowId = -1 );

            void cancelRequest( const char* requestKey );

            // Callback map functions
            CoreCallback_Func getCoreCallback( string key );
            bool getCoreCallbackCancelState( string key );
            void setCoreCallbackCancelState( string key, bool state );
            ClientCallback_Func getClientCallback( string key );

            // SQLite message queue functions
            void mf_addMessageToDataQueue( string path, string coreCB, string clientCB = "", string postdata = "", const char* contentType = NULL );
            void mf_updateMessageStatusInDataQueue( int rowId, string status );
        
            // Telemetry event values
            void addTelemEventValue( const char* key, const char* value );
            void addTelemEventValue( const char* key, int8_t value );
            void addTelemEventValue( const char* key, int16_t value );
            void addTelemEventValue( const char* key, int32_t value );
            void addTelemEventValue( const char* key, uint8_t value );
            void addTelemEventValue( const char* key, uint16_t value );
            void addTelemEventValue( const char* key, uint32_t value );
            void addTelemEventValue( const char* key, float value );
            void addTelemEventValue( const char* key, double value );

            // Telemetry event helpers
            void clearTelemEventValues();
            void clearTelemEvents();
            void clearAchievementEventValues();
            void saveTelemEvent( const char* name );
            void saveAchievementEvent( const char* item, const char* group, const char* subGroup );

            // Setters
            void setConnectUri( const char* uri );
            void setName( const char* name );
            void setVersion( const char* version );
            void setGameLevel( const char* gameLevel );
            void setUserId( int userId );
            void setPlayerHandle( const char* handle );
            void removePlayerHandle( const char* handle );
            void setCookie( const char* cookie );
            void setSessionId( const char* sessionId );
        
            // Getters
            const char* getId();
            const char* getCookie();
            const char* getSessionId();
            Const::Status getLastStatus();

            // Config object
            glConfig config;

            // Helper function for displaying warnings and errors
            void displayWarning( string location, string warning );
            void displayError( string location, string error );
            bool mf_checkForJSONErrors( json_t* root );


        private:
            // SDK object
            GlasslabSDK* m_sdk;

            // SQLite Message Queue
            DataSync* m_dataSync;

            // General members
            string m_connectUri;
            string m_cookie;
            string m_gameId;
            string m_deviceId;
            string m_baseDeviceId;
            string m_sessionId;
            int    m_userId;
            string m_gameLevel;
            string m_playerHandle;
            string m_clientName;
            string m_clientVersion;
        
            // JSON members
            time_t m_currentTime;
            time_t m_startTime;
            json_error_t m_jsonError;
            json_t* m_userInfo;
            json_t* m_telemEvents;
            json_t* m_telemEventValues;
            json_t* m_achievementEventValues;
        
            // Status members
            Const::Status m_lastStatus;
            std::queue<Const::Response*> m_msgQueue;

            // Helper function for callback setup
            void mf_setupCallbacks();
            // Callback function maps
            map<string, coreCallbackStructure> m_coreCallbackMap;
            map<string, ClientCallback_Func> m_clientCallbackMap;


            int tempCounter;    // TEMPORARY
    };
};
#pragma GCC visibility pop


/* ---------------------------- */
#endif /* defined(GLASSLABSDK_CORE) */
/* ---------------------------- */