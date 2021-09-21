//
// Basic instrumentation profiler by Cherno

// Usage: include this header file somewhere in your code (eg. precompiled header), and then use like:
//
// Instrumentor::Get().BeginSession("Session Name");        // Begin session 
// {
//     InstrumentationTimer timer("Profiled Scope Name");   // Place code like this in scopes you'd like to include in profiling
//     // Code
// }
// Instrumentor::Get().EndSession();                        // End Session
//
// You will probably want to macro-fy this, to switch on/off easily and use things like __FUNCSIG__ for the profile name.
//
#pragma once

#include "Core.h"

#include <string>
#include <chrono>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <deque>

#include <thread>

#if PROFILING
    #define PROFILER_INIT() ::GLCore::Performance::Instrumentor::Get().Init();
    #define PROFILER_DEINIT() ::GLCore::Performance::Instrumentor::Get().DeInit();

    #define PROFILE_SESSION_BEGIN(name) ::GLCore::Performance::Instrumentor::Get().BeginSession(name, name##".json")
    #define PROFILE_SESSION_END() ::GLCore::Performance::Instrumentor::Get().EndSession()

    #define PROFILE_SCOPE(name) ::GLCore::Performance::InstrumentationTimer timer##__LINE__(name)
    #define PROFILE_FUNCTION() PROFILE_SCOPE(__FUNCSIG__)
#else
    #define PROFILER_DEINIT()
    #define PROFILER_INIT()

    #define PROFILE_SESSION_BEGIN(name)
    #define PROFILE_SESSION_END()

    #define PROFILE_SCOPE(name)
    #define PROFILE_FUNCTION()
#endif

namespace GLCore { namespace Performance {

    using FloatingPointMicroseconds = std::chrono::duration<double, std::micro>;

    struct ProfileResult
    {
        std::string Name;
        
        FloatingPointMicroseconds Start;
        std::chrono::microseconds ElapsedTime;

        std::thread::id ThreadID;
    };

    struct InstrumentationSession
    {
        std::string Name;
    };

    class Instrumentor
    {
    private:
        struct InstrumentationSessionData
        {

            std::string name;
            std::string filepath;

        };

    // Internal members
    private:
        InstrumentationSession *m_CurrentSession;
        std::unique_ptr<std::thread> m_ProfilerRunnable = nullptr;
        std::ofstream m_OutputStream;

    // Exposed members
    private:
        std::mutex m_Mutex;
        InstrumentationSessionData m_SessionData;
        std::deque<ProfileResult> m_ProfileQueue;

        bool m_BeginSession = false;
        bool m_EndSession = false;

        bool m_Running = false;

    public:
        void Init();
        void DeInit();

        inline void BeginSession(const std::string &name, const std::string &filepath) 
        { 
            std::lock_guard lock(this->m_Mutex); 
            m_SessionData = { name, filepath }; 
            this->m_BeginSession = true; 
        
        }
        inline void EndSession() {
        
            std::lock_guard lock(this->m_Mutex);
            this->m_EndSession = true; 
        
        }
        inline void SubmitProfile(const ProfileResult &result) { 
        
            std::lock_guard lock(this->m_Mutex);
            this->m_ProfileQueue.emplace_front(result); 
        
        }

    public:
        inline static Instrumentor &Get() { static Instrumentor instance; return instance; }

    private:
        Instrumentor();

        void Run();
        void EmptyQueue();

        void WriteProfile(const ProfileResult &result);
        void InternalBeginSession();
        void InternalEndSession();

        void WriteHeader();
        void WriteFooter();

    };

    class InstrumentationTimer
    {
    public:
        InstrumentationTimer(const char *name);
        ~InstrumentationTimer();

        void Stop();

    private:
        const char *m_Name;
        std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
        bool m_Stopped;
    };

} }