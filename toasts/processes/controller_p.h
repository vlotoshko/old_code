#ifndef CONTROLLER_P_H_
#define CONTROLLER_P_H_

#include <zthread/Runnable.h>
#include <zthread/Thread.h>
#include "container.h"
#include "toaster.h"
#include "butterer.h"
#include "jammer.h"
#include "settings.h"

#include <fstream>
#include <sstream>
#include <memory>


//--------------------------------------------------------------------
// Controller interface
//--------------------------------------------------------------------
class ControllerPrivate;
class Displayer;
// class Logger;

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


//--------------------------------------------------------------------
// ControllerPrivate interface
//--------------------------------------------------------------------
class ControllerPrivate {
public:
  ControllerPrivate(
    Controller*,
    Container&,
    Container&,
    CountedPtr< Toaster >&,
    CountedPtr< Butterer >&,
    CountedPtr< Jammer >&
  );
  ~ControllerPrivate();

  // Inerface pointer
  Controller* _ptr;

  // Critical section instruments
  Condition _condition;
  Mutex _mutex;

  // Queues
  Container& _basket;
  Container& _conveyor;

  // Threads
  CountedPtr< Toaster >& _toaster;
  CountedPtr< Butterer >& _butterer;
  CountedPtr< Jammer >& _jammer;

  // Settings
  Settings _settings;

  // Other
  State _state;
  int _processed;
  Displayer* _displayer;
  time_t _beginTime;
  time_t _lastLogTime;

  // Methods
  void processesCheck();
  void run();
  void start();
  void pause();
  void stop();
  void stopping();
  void log( std::string = "" );
};

//--------------------------------------------------------------------
// Displayer
//--------------------------------------------------------------------
class Displayer {
  Mutex _mutex;
  std::string _fileName;
public:
  Displayer( std::string name = "/home/arete/test.txt" ) : _fileName( name ) { }
  ~Displayer() {}
  void clear();
  friend void operator<< ( Displayer& d, std::stringstream& out );
};

#endif