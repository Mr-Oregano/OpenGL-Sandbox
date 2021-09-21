
#include "glpch.h"

#include "Instrumentor.h"

namespace GLCore { namespace Performance {


    Instrumentor::Instrumentor() 
        : m_CurrentSession(nullptr)
    {}

    void Instrumentor::Init()
    {

        this->m_Running = true;
        this->m_ProfilerRunnable = std::make_unique<std::thread>(&Instrumentor::Run, this);

    }
    void Instrumentor::DeInit()
    {

        this->m_Running = false;
        m_ProfilerRunnable->join();

    }

    void Instrumentor::Run()
    {
        while (this->m_Running)
        {

            this->EmptyQueue();

            if (this->m_EndSession)
                this->InternalEndSession();

            if (this->m_BeginSession)
            {
                this->InternalBeginSession();
                this->m_BeginSession = false;
            }

        }

    }
    void Instrumentor::EmptyQueue()
    {
        while (!this->m_ProfileQueue.empty())
        {

            this->m_Mutex.lock();
            this->WriteProfile(this->m_ProfileQueue.back());
            this->m_ProfileQueue.pop_back();
            this->m_Mutex.unlock();

        }
    }

    void Instrumentor::WriteProfile(const ProfileResult& result)
    {
        std::stringstream json;

        // TODO: Occasional Null ptr exception (probably has to do with the threading).
        std::string name = result.Name;
        //

        std::replace(name.begin(), name.end(), '"', '\'');

        json << std::setprecision(3) << std::fixed;
        json << ",{";
        json << "\"cat\":\"function\",";
        json << "\"dur\":" << (result.ElapsedTime.count()) << ',';
        json << "\"name\":\"" << name << "\",";
        json << "\"ph\":\"X\",";
        json << "\"pid\":0,";
        json << "\"tid\":" << result.ThreadID << ",";
        json << "\"ts\":" << result.Start.count();
        json << "}";

        if (m_CurrentSession) {

            m_OutputStream << json.str();
            m_OutputStream.flush();
        
        }

    }
    void Instrumentor::WriteHeader()
    {
        m_OutputStream << "{\"otherData\": {},\"traceEvents\":[{}";
        m_OutputStream.flush();
    }
    void Instrumentor::WriteFooter()
    {
        m_OutputStream << "]}";
        m_OutputStream.flush();
    }

    void Instrumentor::InternalBeginSession()
    {
        
        if (m_CurrentSession) {
            // If there is already a current session, then close it before beginning new one.
            // Subsequent profiling output meant for the original session will end up in the
            // newly opened session instead.  That's better than having badly formatted
            // profiling output.
            if (Log::GetLogger()) // Edge case: BeginSession() might be before Log::Init()
                LOG_WARN("Instrumentor::BeginSession('{0}') when session '{1}' already open.", m_SessionData.name, m_CurrentSession->Name);

            InternalEndSession();
        }

        m_OutputStream.open(m_SessionData.filepath);

        if (m_OutputStream.is_open()) {

            m_CurrentSession = new InstrumentationSession({ m_SessionData.name });
            WriteHeader();
            return;

        }

        if (Log::GetLogger()) // Edge case: BeginSession() might be before Log::Init()
            LOG_ERROR("Instrumentor could not open results file '{0}'.", m_SessionData.filepath);

        this->m_BeginSession = false;

    }
    void Instrumentor::InternalEndSession() {

        if (m_CurrentSession) {
            
            WriteFooter();
            m_OutputStream.close();
            delete m_CurrentSession;
            m_CurrentSession = nullptr;

        }

        this->m_EndSession = false;

    }

    ////////////////////////////////// TIMER //////////////////////////////////

    InstrumentationTimer::InstrumentationTimer(const char *name)
        : m_Name(name), m_Stopped(false)
    {
        m_StartTimepoint = std::chrono::steady_clock::now();
    }
    InstrumentationTimer::~InstrumentationTimer()
    {
        if (!m_Stopped)
            Stop();
    }

    void InstrumentationTimer::Stop()
    {
        auto endTimepoint = std::chrono::steady_clock::now();
        auto highResStart = FloatingPointMicroseconds{ m_StartTimepoint.time_since_epoch() };
        auto elapsedTime = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch()
                         - std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch();

        Instrumentor::Get().SubmitProfile({ m_Name, highResStart, elapsedTime, std::this_thread::get_id() });

        m_Stopped = true;
    }
} }