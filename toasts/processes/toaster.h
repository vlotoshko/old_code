#ifndef TOASTER_H_
#define TOASTER_H_

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
  ToasterPrivate* d_ptr;
};

#endif