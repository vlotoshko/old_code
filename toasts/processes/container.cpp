#include "container.h"

// temp includes
#include <iostream>

template< typename T, T Zero >
TQueue< T, Zero >::TQueue() : _condition( _mutex ), _zero( Zero ) {
}

template< typename T, T Zero >
void TQueue< T, Zero >::put( T item ) {
  Guard< Mutex > guard( _mutex );

  _data.push_back( item );
  _condition.signal();
}

template< typename T, T Zero >
T TQueue< T, Zero >::get( State& state ) {
  Guard< Mutex > guard( _mutex );

  while( _data.empty() ) {
    if( state == State_Stopping ) {
      state = State_Stopped;
      break;
    }
    else {
//       std::cout << message << std::endl;
      _condition.wait();
    }
  }

  T returnVal = zero();
  if( !_data.empty() ) {
    returnVal = _data.front();
    _data.pop_front();
  }
  return returnVal;
}

template< typename T, T Zero >
int TQueue< T, Zero >::count() {
  return _data.size();
}

template< typename T, T Zero >
T TQueue< T, Zero >::zero() {
  return _zero;
}

std::string state_show( State state) {
  switch( state ) {
    case State_Working: return "working";
    case State_Paused: return "paused";
    case State_Stopping: return "stopping";
    case State_Stopped: return "stopped";
    default: return "invalid";
  }
}

namespace Glogal {
  int sleep = 500;
  int countThreads = 5;
  const double PI = 3.1415965358979323846;
  const double E = 2.718281824590452354;
  int MAX_TOASTS = 1000000;
  int QUEUE_LIMIT = 10000;
  std::string now();
  std::string timeToStr( time_t, bool );

  std::string now() {
    return timeToStr( time( 0 ) );
  }

  std::string timeToStr( time_t rawtime, bool local ) {
    struct tm * timeinfo;
    if ( local )
      timeinfo = localtime( &rawtime );
    else
      timeinfo = gmtime( &rawtime );
    return
      (timeinfo->tm_hour < 10 ? "0" : "") + std::to_string( timeinfo->tm_hour ) + ":" +
      (timeinfo->tm_min < 10 ? "0" : "") + std::to_string( timeinfo->tm_min ) + ":" +
      (timeinfo->tm_sec < 10 ? "0" : "") + std::to_string( timeinfo->tm_sec );
  }

};

template class TQueue< int, 0 >;