#include "worker.h"

//--------------------------------------------------------------------
// JammerPrivate interface
//--------------------------------------------------------------------
class JammerPrivate;

class Jammer : public WorkProcess {
public:
  Jammer( Container& );
  ~Jammer();
  void run();

  bool stateChanged();
  std::string logInfo();
private:
  JammerPrivate* d_ptr;
};


//--------------------------------------------------------------------
// JammerPrivate interface
//--------------------------------------------------------------------
class JammerPrivate{
private:
  State _oldState;
public:
  JammerPrivate( Jammer*, State );
  Jammer* q_ptr;
  bool stateChanged( State );
  std::string logInfo( State, int, int );
};