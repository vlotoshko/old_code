#ifndef JAMMER_H_
#define JAMMER_H_

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

#endif