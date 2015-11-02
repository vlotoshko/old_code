#ifndef WORKER_H_
#define WORKER_H_

#include "container.h"
#include <zthread/Runnable.h>

//--------------------------------------------------------------------
// WorkProcess interface (abstract)
//--------------------------------------------------------------------
class WorkProcessPrivate;

class WorkProcess : public ZThread::Runnable {
public:
  WorkProcess( std::string, Container*, Container* );
  virtual ~WorkProcess();

  // Conduct
  virtual void run() = 0;
  virtual void start();
  virtual void pause();
  virtual void stop();

  // Getters
  const State state() const;
  const int processed() const;

protected:
  Container& containerPut();
  Container& containerGet();
  Condition& condition();
  Mutex& mutex();
  State& _state();
  int& count();

private:
  WorkProcessPrivate* d_ptr;
};


//--------------------------------------------------------------------
// Logger interface (abstract)
//--------------------------------------------------------------------
class LoggerI {
public:
  virtual std::string logInfo() = 0;
  virtual bool stateChanged() = 0;
};

#endif