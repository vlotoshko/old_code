#ifndef CONTAINER_H_
#define CONTAINER_H_

#include <deque>
#include <string>
#include <zthread/Guard.h>
#include <zthread/Mutex.h>
#include <zthread/Condition.h>
#include <zthread/CountedPtr.h>

using ZThread::Condition;
using ZThread::Mutex;
using ZThread::CountedPtr;
using ZThread::Guard;


namespace Glogal {
  extern int sleep;
  extern int countThreads;
  extern const double PI;
  extern const double E;
  extern int MAX_TOASTS;
  extern int QUEUE_LIMIT;
  extern std::string now();
  extern std::string timeToStr( time_t, bool = true );
};
namespace g = Glogal;

enum State {
  State_Invalid = 0,
  State_First = 1,
  State_Working = 2,
  State_Paused = 3,
  State_Stopping = 4,
  State_Stopped = 5,
  State_Failed = 6,
  State_Last = 6
};

std::string state_show( State );

//--------------------------------------------------------------------
// TQueue
//--------------------------------------------------------------------
template< typename T, T Zero > class TQueue {
private:
  Mutex _mutex;
  Condition _condition;
  std::deque< T > _data;
  T _zero;
public:
  TQueue();
  void put( T );
  T get( State& );
  int count();
  T zero();
};

typedef CountedPtr< TQueue< int, 0 > > Container;


#endif