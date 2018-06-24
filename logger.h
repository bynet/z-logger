#pragma  once
#include <iostream>
#include <memory>
#include <fstream>
#include <chrono>
#include <ctime>
#include <iostream>
#include <time.h>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <sstream>
#include <iomanip>


using namespace std;

enum class Severity {
	DEBUG,
	INFO,
	WARN,
	ERROR
};

namespace Z {

	using P_Stream = std::shared_ptr<std::ostream>;
	using StreamList = vector< shared_ptr<ostream> >;

	using Target = pair< Severity, P_Stream >;
	using TargetMap = unordered_map<string, Target>;
	using SystemClock = std::chrono::system_clock;


	class AtomicWriter {

	private:
		std::ostringstream st;
		StreamList m_StreamList;
		mutex&     m_Mutex;
		Severity   m_Severity;
		string     m_DateTimeFormat;
	public:

		AtomicWriter(StreamList& streamList, mutex& mtx , Severity& sev )
			:m_StreamList(streamList)
			, m_Mutex(mtx)
			, m_Severity( sev )
			, m_DateTimeFormat("%Y:%m:%d %H:%M:%S")
		{

		}

		AtomicWriter(const AtomicWriter& s)
			:m_StreamList(s.m_StreamList)
			,m_Mutex(s.m_Mutex)
			,m_Severity( s.m_Severity )
			,m_DateTimeFormat( s.m_DateTimeFormat )
		{
			st.clear();
			st << s.st.str();
		}

		template <typename T>
		AtomicWriter& operator<<(T const& t) {

			if (m_StreamList.size() > 0) {
				st << t;
			}
			return *this;

		}

		~AtomicWriter() {

			if (st.tellp() > 0) {

				std::lock_guard<std::mutex> safe(m_Mutex);
				std::ostringstream ss;

				ss << "[ " << GetLogTimeAsString(ReadSystemTime()) << " " << SeverityLevel( m_Severity ) << " ] " << " {" << st.str() << " }"; 

				for (auto s : m_StreamList) {
					*s << ss.str() << '\n';
				}

			}

		}
		
	private:
		std::tm ReadSystemTime() const {

			SystemClock::time_point now = SystemClock::now();
			std::time_t now_c = SystemClock::to_time_t(now);
			std::tm tm_snapshot;

#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
			localtime_s(&tm_snapshot, &now_c);
#else
			localtime_r(&time, &tm_snapshot); // POSIX
#endif
			return tm_snapshot;
		}

		const char* SeverityLevel(Severity level) {

			const unordered_map<Severity, const char*> LevelStrings 
			{
				{ Severity::ERROR , "ERROR" },
				{ Severity::WARN  , "WARN " },
				{ Severity::INFO  , "INFO " },
				{ Severity::DEBUG , "DEBUG" }
			};

			auto it = LevelStrings.find(level);
			return it == LevelStrings.end() ? "UNKNOWN" : it->second;
		}

		string GetLogTimeAsString(std::tm& tm) const {

			std::ostringstream ss;
			ss << std::put_time(&tm, m_DateTimeFormat.c_str());
			return ss.str();
		}



	};


	class Logger {
		Severity  m_Severity;
		mutex     m_LoggerMutex;
		TargetMap m_TargetMap;

	public:

		Logger():m_Severity(Severity::DEBUG) {
		}

		static Logger& GetInstance() {
			static Logger instance;
			return instance;
		}

		static void Init(Severity s = Severity::DEBUG) {
			GetInstance().SetSeverity(s);
		}

		static TargetMap GetTargets() {
			return GetInstance().m_TargetMap;
		}

		void SetSeverity(Severity s) {
			m_Severity = s;
		}

		static Target AddFile(const char* filePathName, Severity sev = Severity::DEBUG) {
			return AddFile(string(filePathName), sev);
		}

		static Target AddFile(string& filePathName, Severity sev = Severity::DEBUG) {

			if (GetInstance().m_TargetMap.size() > 0) {
				auto search = GetInstance().m_TargetMap.find(filePathName);
				if (search != GetInstance().m_TargetMap.end()) {
					return search->second;
				}
			}

			auto Ptr = std::shared_ptr< std::ostream >(new std::ofstream(filePathName, std::ios_base::trunc));
			Target newTarget = make_pair(sev, Ptr);
			GetInstance().m_TargetMap[filePathName] = newTarget;
			return newTarget;

		}


		static Target AddConsole(Severity sev = Severity::DEBUG) {

			if (GetInstance().m_TargetMap.size() > 0) {
				auto search = GetInstance().m_TargetMap.find("console");
				if (search != GetInstance().m_TargetMap.end()) {
					return search->second;
				}
			}

			auto Ptr = std::shared_ptr< std::ostream >(&std::cout, [](void*) {});
			Target newTarget = make_pair(sev, Ptr);
			GetInstance().m_TargetMap["console"] = newTarget;
			return newTarget;

		}

		AtomicWriter log(Severity s) {

			StreamList streams;

			if (m_Severity > s) {
				return AtomicWriter(streams, m_LoggerMutex , s );
			}

			for (auto t : m_TargetMap) {
				if (t.second.first <= s)
					streams.push_back(t.second.second);
			}

			AtomicWriter b(streams, m_LoggerMutex , s );

			return b;

		}

		AtomicWriter debug() {
			return log(Severity::DEBUG);
		}

		AtomicWriter info() {
			return log(Severity::INFO);
		}

		AtomicWriter warn() {
			return log(Severity::WARN);
		}

		AtomicWriter error() {
			return log(Severity::ERROR);
		}

	};

#define DEBUG()  Logger::GetInstance().debug()
#define INFO()   Logger::GetInstance().info()
#define WARN()   Logger::GetInstance().warn()
#define ERROR()  Logger::GetInstance().error()
	

};
