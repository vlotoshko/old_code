#ifndef TOASTER_P_H_
#define TOASTER_P_H_

#include <zthread/Thread.h>
#include "worker.h"


//--------------------------------------------------------------------
// Toaster interface
//--------------------------------------------------------------------
class ToasterPrivate;

class Toaster : public WorkProcess, public LoggerI {
public:
  Toaster( Container& );
  ~Toaster();
  void run();
  void stop();

  bool stateChanged();
  std::string logInfo();
private:
  friend ToasterPrivate;
  ToasterPrivate* d_ptr;
};


//--------------------------------------------------------------------
// ToasterPrivate interface
//--------------------------------------------------------------------
class ToasterPrivate{
private:
  State _oldState;
public:
  ToasterPrivate( Toaster*, State );
  Toaster* q_ptr;
  bool stateChanged( State );
  std::string logInfo( State, int, int );
};

#endif