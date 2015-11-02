#ifndef WORKER_P_H_
#define WORKER_P_H_

#include <string>
#include <zthread/Runnable.h>
#include "container.h"

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
// WorkProcessPrivate interface
//--------------------------------------------------------------------
class WorkProcessPrivate {
public:
  WorkProcessPrivate( WorkProcess*, Container*, Container* );

  WorkProcess* q_ptr;
  Container* _containerGet;
  Container* _containerPut;
  Condition _condition;
  Mutex _mutex;
  int _processed;
  State _state;
  std::string _name;

  virtual void start();
  virtual void pause();
  virtual void stop();
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