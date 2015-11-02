#ifndef BUTTERER_H_
#define BUTTERER_H_

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

#endif