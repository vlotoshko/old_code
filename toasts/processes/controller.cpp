#include "controller_p.h"

// temp includes TODO
#include <iostream>
using std::cout;
using std::cin;
using std::endl;
using std::string;

//--------------------------------------------------------------------
// ControllerPrivate implementation
//--------------------------------------------------------------------
ControllerPrivate::ControllerPrivate(
    Controller* c,
    Container& basket,
    Container& conveyor,
    CountedPtr< Toaster >& toaster,
    CountedPtr< Butterer >& butterer,
    CountedPtr< Jammer >& jammer
): _ptr( c ), _basket( basket ), _conveyor( conveyor ),
   _toaster( toaster ), _butterer( butterer ), _jammer( jammer ),
   _state( State_Paused ), _condition(_mutex ), _beginTime( time( 0 ) ),
   _lastLogTime( _beginTime ), _settings()
{
  _displayer = new Displayer();
  _displayer->clear();
}

ControllerPrivate::~ControllerPrivate() {
  delete _displayer;
}

void ControllerPrivate::run() {
  std::string logStr;
  try {
    logStr = 
      " --- CONTROLLER STARTED ---\nmax toasts: " + std::to_string( _settings.maxCookedToasts ) + '\n' +
      "max toasts in basket: " + std::to_string( _settings.maxToastsInBasket )  + '\n' +
      "max toasts on conveyor: " + std::to_string( _settings.maxToastsOnConveyor ) + '\n' +
      "log interval: " + std::to_string( _settings.logInterval ) + '\n';
    log( logStr );
    cout << logStr << "Press <Enter> to start." << endl;


    while( _state != State_Stopped ) {
      Guard< Mutex > guard( _mutex );

      _condition.wait( ( rand()/( RAND_MAX/5 )*g::sleep ) * 20 );

      if( _state == State_Paused )
        _condition.wait();

      processesCheck();
      log();

      if( rand() % 1000 == 0 )
          cout << "toasts ready: " << _jammer->processed() << endl;

      if( _state == State_Stopping)
        stopping();
    }
  }
  catch( ZThread::Interrupted_Exception& e ) { cout << "Controller exc-off " << endl; }

  logStr = "toasted: " + std::to_string( _toaster->processed() ) +
  "; butteded: " + std::to_string( _butterer->processed() ) +
  "; jammed: " + std::to_string( _jammer->processed() ) + '\n' +
  "duration: " + g::timeToStr( difftime( time( 0 ), _beginTime ), false ) + '\n' +
  " --- CONTROLLER STOPPED ---\n";
  log( logStr );
  cout << logStr;


//   cout << " --- CONTROLLER STOPPED --- " << endl;
}


void ControllerPrivate::start() {
//   Guard< Mutex > guard( _mutex );
  _state = State_Working;
//   cout << "CONTROLLER STARTED" << endl;
  _condition.signal();
  _toaster->start();
  _butterer->start();
  _jammer->start();
}

void ControllerPrivate::pause() {
//   Guard< Mutex > guard( _mutex );
  _state = State_Paused;
//   cout << "CONTROLLER PAUSED" << endl;
  _toaster->pause();
  _butterer->pause();
  _jammer->pause();
  _condition.wait();
}

void ControllerPrivate::stop() {

  if( _state != State_Stopped ) {
    _state = State_Stopping;
//     cout << "CONTROLLER STOPPING" << endl;
  }
}

void ControllerPrivate::stopping() {
//   Guard< Mutex > guard( _mutex );
  if(  _toaster->state() == State_Working )
    _toaster->stop();

  if( _toaster->state() == State_Stopped && _butterer->state() == State_Working )
    _butterer->stop();

  if( _butterer->state() == State_Stopped && _jammer->state() == State_Working )
    _jammer->stop();

  if(_toaster->state() == State_Stopped &&
     _butterer->state() == State_Stopped &&
     _jammer->state() == State_Stopped ) {
    _state = State_Stopped;
  }
}


void ControllerPrivate::processesCheck() {
//   Guard< Mutex > guard( _mutex );
  // basket filling
  if( _toaster->state() == State_Working &&
      _basket->count() >= _settings.maxToastsInBasket ) {
    _toaster->pause();
  }

  if( _toaster->state() == State_Paused &&
      _basket->count() < (_settings.maxToastsInBasket / 2) )
    _toaster->start();

  // conveyor filling
  if( _butterer->state() == State_Working &&
      _conveyor->count() >= _settings.maxToastsOnConveyor )
    _butterer->pause();

  if( _butterer->state() == State_Paused &&
    _conveyor->count() < (_settings.maxToastsOnConveyor / 2) )
    _butterer->start();

  // toasts cooked
  if( _toaster->state() == State_Working &&
    _toaster->processed() >= _settings.maxCookedToasts ) {
    stop();
  }
}


void ControllerPrivate::log( std::string message ) {
  std::stringstream ss;

  ss << g::now() << endl;

  string event_time  = "event time: ";
  string event_state = "event state: ";

  time_t now = time( 0 );
  bool timeToLog = ( now - _lastLogTime ) > _settings.logInterval;
  bool stateChanged = false;

  if ( message == "" ) {
    if ( _toaster->stateChanged() ) {
      ss << event_state << _toaster->logInfo() << endl;
      if (_toaster->state() == State_Stopped )
        ss << "TOASTER STOPPED" << endl;
      stateChanged = true;
    }

    if ( _butterer->stateChanged() ) {
      ss << event_state << _butterer->logInfo() << endl;
      if (_butterer->state() == State_Stopped )
        ss << "BUTTERER STOPPED" << endl;
      stateChanged = true;
    }

    if ( _jammer->stateChanged() ) {
      ss << event_state << _jammer->logInfo() << endl;
      if (_jammer->state() == State_Stopped )
        ss << "JAMMER STOPPED" << endl;
      stateChanged = true;
    }

   if ( timeToLog ) {
    if ( stateChanged )
      ss << endl;

     if (_toaster->state() != State_Stopped )
       ss << event_time << _toaster->logInfo() << endl;

     if (_butterer->state() != State_Stopped )
       ss << event_time << _butterer->logInfo() << endl;

     if (_jammer->state() != State_Stopped )
       ss << event_time << _jammer->logInfo() << endl;

     _lastLogTime = now;
    }
  }
  else
    ss << message << endl;

  if( message != "" || timeToLog || stateChanged ) {
    ss << endl;
    *_displayer << ss;
  }
}

//--------------------------------------------------------------------
// ControllerPrivate implementation
//--------------------------------------------------------------------
Controller::Controller(
  Container& basket,
  Container& conveyor,
  CountedPtr< Toaster >& toaster,
  CountedPtr< Butterer >& butterer,
  CountedPtr< Jammer >& jammer
) : d_ptr( new ControllerPrivate( this, basket, conveyor, toaster, butterer, jammer ) ) {
}


Controller::~Controller() {
  delete d_ptr;
}

  // Conduct
void Controller::run() { d_ptr->run(); }
void Controller::start() { d_ptr->start(); }
void Controller::pause() { d_ptr->pause(); }
void Controller::stop() { d_ptr->stop(); }

// Getters
Settings& Controller::settings() { return d_ptr->_settings; }


//--------------------------------------------------------------------
// Displayer implementation
//--------------------------------------------------------------------
void operator<< ( Displayer& d, std::stringstream& out ) {
  Guard< Mutex > guard( d._mutex );
  std::auto_ptr< std::fstream > file(new std::fstream() );
  (*file).open(  d._fileName.c_str() , std::ios_base::out | std::ios_base::app );
  (*file) << out.str();
}

void Displayer::clear() {
  Guard< Mutex > guard( _mutex );
  std::ofstream file;
  file.open(_fileName.c_str(), std::ofstream::out | std::ofstream::trunc);
  file.close();
}
