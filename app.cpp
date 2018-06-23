
#include <gtest/gtest.h>
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
#include "logger.h"

using namespace std;

class Point{
    int x, y;

public:
    Point():x(1),y(1){
        
    }

    friend ostream& operator<<(ostream& os , const Point& p ){
        os << "x = " << p.x << " , " << " y = " << p.y ;
        return os;
    }

};
using namespace Z;


int test_logger() {

	Point p;


	Z::Logger::Init();

	Z::Logger::AddFile("debug_log.txt" , Severity::DEBUG);
	Z::Logger::AddFile("info_log.txt" , Severity::INFO );
	Z::Logger::AddFile("warn_log.txt" , Severity::WARN );
	Z::Logger::AddFile("error_log.txt" , Severity::ERROR );
	Z::Logger::AddConsole(Severity::DEBUG);

	Z::ERROR() << "error " << "message" ; 
	Z::WARN()  << "warn " << " message" ;
	Z::INFO()  << "info " << " message ";
	Z::DEBUG() << "debug " << "message " << "with custom object is " << p ;


	return 0;
}

using namespace std;
int main(int argc, char* argv[]) {
	test_logger();

}
