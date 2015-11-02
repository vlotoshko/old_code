#include "butterer_p.h"

// // temp includes TODO
// #include <iostream>

//--------------------------------------------------------------------
// ButtererPrivate implementation
//--------------------------------------------------------------------
ButtererPrivate::ButtererPrivate( Butterer* butterer, State state )
: q_ptr( butterer ), _oldState( state ) {
}


bool ButtererPrivate::stateChanged( State state ) {
  return _oldState != state;
}


std::string ButtererPrivate::logInfo( State state, int buttered, int putCount, int getCount ) {

  std::string out = "butterer\t";

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
  out += " buttered: " + std::to_string( buttered ) + ";";

  // Additional log
  out += " in basket: " + std::to_string( getCount ) + ";";
  out += " on conveyor: " + std::to_string( putCount ) + ";";

  return out;
}

//--------------------------------------------------------------------
// Butterer implementation
//--------------------------------------------------------------------
Butterer::Butterer( Container& get, Container& put )
: WorkProcess( "butterer", &get, &put ), d_ptr( new ButtererPrivate( this, state() ) ) {
}
Butterer::~Butterer() {
 delete d_ptr;
}

void Butterer::run() {
  try {
    while( _state() != State_Stopped ) {
      Guard< Mutex > guard( mutex() );
//       std::cout << "Butterer::run(), state = " << state_show( _state() ) << std::endl;

      if( _state() == State_Paused )
        condition().wait();

      if( containerGet()->get( _state() ) == containerGet()->zero() )
        continue;

      condition().wait( ( rand()/( RAND_MAX/5 )*g::sleep ) * 2 );

      containerPut()->put( 1 );
      ++count();
//       std::cout << "butterer: buttered toast #" << ++count()
//             << ", put on conveyor" << std::endl;

    }

  }
  catch( ZThread::Interrupted_Exception& e ) {
    _state() = State_Failed;
  }
//   std::cout << " --- BUTTERED STOPPED --- " << std::endl;
}

bool Butterer::stateChanged() {
  Guard< Mutex > guard( mutex() );
  return d_ptr->stateChanged( state() );
}

std::string Butterer::logInfo() {
  Guard< Mutex > guard( mutex() );
  return d_ptr->logInfo( state(), count(), containerPut()->count(), containerGet()->count() );
}