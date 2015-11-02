#include "toaster_p.h"

// // temp includes TODO
// #include <iostream>

//--------------------------------------------------------------------
// ToasterPrivate implementation
//--------------------------------------------------------------------
ToasterPrivate::ToasterPrivate( Toaster* toaster, State state )
: q_ptr( toaster ), _oldState( state ) {
}

bool ToasterPrivate::stateChanged( State state ) {
  return _oldState != state;
}

std::string ToasterPrivate::logInfo( State state, int toasted, int containerCount ) {

  std::string out = "toaster\t";

    // State changed log
  if ( stateChanged( state ) ) {
    out += "state: " + state_show( _oldState ) + " -> " + state_show( state ) + ";";
    _oldState = state;
  }
  else
    // State log
    if( state != State_Invalid )
      out += "state: " + state_show( state ) + ";";

  // Processed log
  out += " cooked: " + std::to_string( toasted ) + ";";

  // Additional log
  out += " in basket: " + std::to_string( containerCount ) + ";";

  return out;
}


//--------------------------------------------------------------------
// Toaster implementation
//--------------------------------------------------------------------
Toaster::Toaster( Container& put )
: WorkProcess( "toaster", 0, &put ), d_ptr( new ToasterPrivate( this, state() ) ) {
}

Toaster::~Toaster() {
  delete d_ptr;
}


void Toaster::run() {
  try {
    while( _state() != State_Stopped ) {
      Guard< Mutex > guard( mutex() );
//       std::cout << "Toaster::run(), state = " << state_show( _state() ) << std::endl;

      if( _state() == State_Paused )
        condition().wait();


//       std::cout << "toaster: cooking toast " << ++count() << std::endl;
//       ZThread::Thread::sleep( rand()/( RAND_MAX/5 )*1000/2 );
      condition().wait( ( rand()/( RAND_MAX/5 )*g::sleep ) );
      /*
         Изготовление нового теста...
      */
      containerPut()->put( 1 );
      ++count();
//       std::cout << "toaster: cooked toast #" << ++count()
//             << ", put into basket" << std::endl;
    }
  }
  catch( ZThread::Interrupted_Exception& e ) {
    _state();
  }
//   std::cout << " --- TOASTER STOPPED --- " << std::endl;
//     try {
//     int counter = 0;
//     while( _state() != State_Stopped ) {
//       Guard< Mutex > guard( mutex() );
//       ZThread::Thread::sleep( 1000 / 2 );
//       std::cout << "Toaster::run()" << std::endl;
//       std::cout << "state = " << state_show( _state() ) << std::endl;
// 
//       if( _state() == State_Paused )
//         condition().wait();
//     }
//   }
//   catch( ZThread::Interrupted_Exception& e ) {
//     std::cout << e.what() << std::endl;
//   }
//   std::cout << "Exiting Toaster::run()" << std::endl;
}

bool Toaster::stateChanged() {
  Guard< Mutex > guard( mutex() );
  return d_ptr->stateChanged( state() );
}

std::string Toaster::logInfo() {
  Guard< Mutex > guard( mutex() );
  return d_ptr->logInfo( state(), count(), containerPut()->count() );
}

void Toaster::stop() {
  WorkProcess::stop();
  _state() = State_Stopped;
}


