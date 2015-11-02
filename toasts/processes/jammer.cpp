#include "jammer_p.h"



//--------------------------------------------------------------------
// JammerPrivate implementation
//--------------------------------------------------------------------
JammerPrivate::JammerPrivate( Jammer* jammer, State state )
: q_ptr( jammer ), _oldState( state ) {
}

bool JammerPrivate::stateChanged( State state ) {
  return _oldState != state;
}

std::string JammerPrivate::logInfo( State state, int jammed, int getCount ) {

  std::string out = "jammer\t";

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
  out += " jammed: " + std::to_string( jammed ) + ";";

  // Additional log
  out += " on conveyor: " + std::to_string( getCount ) + ";";

  return out;
}


//--------------------------------------------------------------------
// Jammer implementation
//--------------------------------------------------------------------
Jammer::Jammer( Container& get )
: WorkProcess( "jammer", &get, 0 ), d_ptr( new JammerPrivate( this, state()  ) ) {
}

Jammer::~Jammer() {
 delete d_ptr;
}

void Jammer::run() {
  try {
    while( _state() != State_Stopped ) {
      Guard< Mutex > guard( mutex() );
//       std::cout << "Jammer::run(), state = " << state_show( _state() ) << std::endl;

      if( _state() == State_Paused )
        condition().wait();

      if( containerGet()->get( _state() )  == containerGet()->zero() )
        continue;

      ++count();
      condition().wait( ( rand()/( RAND_MAX/5 )*g::sleep ) * 3 );

    }
  }
  catch( ZThread::Interrupted_Exception& e ) {
    _state() = State_Failed;
  }
//   std::cout << " --- JAMMER STOPPED --- " << std::endl;
}

bool Jammer::stateChanged() {
  Guard< Mutex > guard( mutex() );
  return d_ptr->stateChanged( state() );
}

std::string Jammer::logInfo() {
  Guard< Mutex > guard( mutex() );
  return d_ptr->logInfo( state(), count(), containerGet()->count() );
}

