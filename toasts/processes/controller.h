#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <zthread/Runnable.h>
#include "container.h"
#include "toaster.h"
#include "butterer.h"
#include "jammer.h"
#include "settings.h"


//--------------------------------------------------------------------
// Controller interface
//--------------------------------------------------------------------
class ControllerPrivate;

class Controller : public ZThread::Runnable {
public:
  Controller(
    Container&,
    Container&,
    CountedPtr< Toaster >&,
    CountedPtr< Butterer >&,
    CountedPtr< Jammer >&
  );
  ~Controller();

  // Conduct
  void run();
  void start();
  void pause();
  void stop();

  Settings& settings();

private:
  ControllerPrivate* d_ptr;
};

#endif