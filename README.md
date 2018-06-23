# z-logger
c++ logger , supports multiple threads ,  multiple output , outputing user types , single header only c++ logger class  

```cpp
#include "logger.h"
class Point{
    int x, y;

public:
    Point():x(1),y(1){
        
    }

    friend ostream& operator<<(ostream& os , const Point& p ){
        os << "x = " << p.x << " , " << " y = " << p.y ;
        return os;
    }


int main(int argc , char* argv[]){
	
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

}
```
