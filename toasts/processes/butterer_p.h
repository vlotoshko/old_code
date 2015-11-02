#ifndef BUTTERER_P_H_
#define BUTTERER_P_H_

#include "worker.h"

//--------------------------------------------------------------------
// Butterer interface
//--------------------------------------------------------------------
class ButtererPrivate;

class Butterer : public WorkProcess, public LoggerI {
public:
  Butterer( Container&, Container& );
  ~Butterer();
  void run();

  bool stateChanged();
  std::string logInfo();
private:
  ButtererPrivate* d_ptr;
};


//--------------------------------------------------------------------
// ButtererPrivate interface
//--------------------------------------------------------------------
class ButtererPrivate{
private:
  State _oldState;
public:
  ButtererPrivate( Butterer*, State );
  Butterer* q_ptr;
  bool stateChanged( State );
  std::string logInfo( State, int, int, int );
};

#endif