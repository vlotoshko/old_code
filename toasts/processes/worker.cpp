#include "worker_p.h"

// temp includes TODO
#include <iostream>


//--------------------------------------------------------------------
// WorkProcessPrivate implementation
//--------------------------------------------------------------------

WorkProcessPrivate::WorkProcessPrivate( WorkProcess* w , Container* get, Container* put )
: q_ptr( w ), _containerGet( get ), _containerPut( put ), _condition( _mutex ),
  _processed( 0 ), _state( State_Paused ), _name("") {
}

void WorkProcessPrivate::start() {
//   Guard< Mutex > guard( _mutex );
  _state = State_Working;
//   std::cout << '\t' << _name << ": starting..." << std::endl;
  _condition.signal();
}

void WorkProcessPrivate::pause() {
//   Guard< Mutex > guard( _mutex );
  _state = State_Paused;
//   std::cout << '\t' << _name << ": paused" << std::endl;
}

void WorkProcessPrivate::stop() {
//   Guard< Mutex > guard( _mutex );
  _state = State_Stopping;
  _condition.signal();
}


//--------------------------------------------------------------------
// WorkProcess implementation
//--------------------------------------------------------------------
WorkProcess::WorkProcess( std::string name, Container* get = 0, Container* put = 0 )
: d_ptr( new WorkProcessPrivate( this, get, put ) ) {
  d_ptr->_name = name;
}

WorkProcess::~WorkProcess() {
  delete d_ptr;
}

// WorkProcess
void WorkProcess::start() { d_ptr->start(); }
void WorkProcess::pause() { d_ptr->pause(); }
void WorkProcess::stop() { d_ptr->stop(); }

// Getters
const State WorkProcess::state() const { return d_ptr->_state; }
const int WorkProcess::processed() const { return d_ptr->_processed; }

Container& WorkProcess::containerPut() { return *(d_ptr->_containerPut); }
Container& WorkProcess::containerGet() { return *(d_ptr->_containerGet); }
Condition& WorkProcess::condition() { return d_ptr->_condition; }
Mutex& WorkProcess::mutex() { return d_ptr->_mutex; }
State&WorkProcess:: _state() { return d_ptr->_state; }
int& WorkProcess::count() { return d_ptr->_processed; }

