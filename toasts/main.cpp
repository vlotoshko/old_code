#include <iostream>
#include <zthread/ThreadedExecutor.h>
#include <zthread/PoolExecutor.h>
#include "processes/controller.h"


using std::cout;
using std::cin;
using std::endl;
using std::string;

using ZThread::ThreadedExecutor;
using ZThread::Runnable;


void initInputData( int argc, char **argv ) {
  srand( time( 0 ) );

  if( argc > 1 ) 
    g::countThreads = atoi( argv[ 1 ] );
  else
    g::countThreads = 5;

  if (argc > 2)
    g::sleep = atoi( argv[ 2 ] );
}




void cookingToast() {
//   cout << "Press <Enter> to exit." << endl;

  try {
    // queues
    CountedPtr< TQueue< int, 0 > > basket( new TQueue< int, 0 > );
    CountedPtr< TQueue< int, 0 > > conveyor( new TQueue< int, 0 > );

    // processes
    CountedPtr< Jammer > jammer( new Jammer( conveyor ) );
    CountedPtr< Butterer > butterer( new Butterer( basket, conveyor ) );
    CountedPtr< Toaster > toaster( new Toaster( basket ) );
    CountedPtr< Controller > controller( new Controller( basket, conveyor, toaster, butterer, jammer ) );
//     Controller* controller = new Controller( basket, conveyor, toaster, butterer, jammer );

//     Logger( State*, std::string, std::string, const int ( *count )(), std::string, const int ( *addit )() );
//     Logger lg( &toaster->state, "toaster", "cooked", &toaster->processed, "basket has", &basket->count );
    controller->settings().maxCookedToasts = 10000000; // ten millions
    controller->settings().maxToastsInBasket = 150000;
    controller->settings().maxToastsOnConveyor = 90000;
    controller->settings().logInterval = 30;

    // executor
    ThreadedExecutor executor;
    executor.execute( controller );
    executor.execute( toaster );
    executor.execute( butterer );
    executor.execute( jammer );

    cin.ignore();
    controller->start();
    cout << "Press <Enter> to exit." << endl;

    cin.ignore();
    controller->stop();
    executor.wait();

//     cout << "toasted: " << toaster->processed() << endl;
//     cout << "butteded: " << butterer->processed() << endl;
//     cout << "jammed: " << jammer->processed() << endl;
    cout << " --- MAIN THREAD STOPPED --- " << endl;
  }
  catch( ZThread::Synchronization_Exception& e ) {
    cout << e.what() << endl;
  }
}


class SharedObject {
private:
  int _count;
  Mutex _mutex;
  Condition _condition;
  bool _threadsStoped;
public:
  SharedObject() : _count( 0 ), _condition( _mutex ), _threadsStoped( false ) {}
  void slowIncrement( int );
  void manageThreads( int );
  void broadcast();
};

void SharedObject::slowIncrement( int idThread ) {
  Guard< Mutex > guard( _mutex );
  for( int i = 0; i < 3; ++i ) {
    ZThread::Thread::sleep( g::sleep );
    if( idThread != 0 ) {
      ++_count;
      cout << idThread << ": " << _count << endl;
    }
  }
}

void SharedObject::manageThreads( int idThread ) {
  if( idThread != 0 ) {
    Guard< Mutex > guard( _mutex );
    ZThread::Thread::sleep( g::sleep );
    cout << idThread << ": waiting..." << endl;
    _threadsStoped = true;
    _condition.wait();
  }

  if( idThread == 0 && _threadsStoped ) {
    cout << idThread << ": releasing one thread..." << endl;
    ZThread::Thread::sleep( g::sleep * 3 );
    cout << idThread << ": GO!" << endl;
    _threadsStoped = false;
    _condition.signal();
  }
}

void SharedObject::broadcast() {
  Guard< Mutex > guard( _mutex );
  _condition.broadcast();
}


class SimpleThread : public Runnable {
private:
  int _idThread;
  CountedPtr< SharedObject > _object;
public:
  SimpleThread( int, CountedPtr< SharedObject >& );
  void run();
};

SimpleThread::SimpleThread( int i, CountedPtr< SharedObject >& o ) : _idThread( i ), _object( o ) {
}

void SimpleThread::run() {
  try {
    while( !ZThread::Thread::interrupted() ) {
      _object->slowIncrement( _idThread );
      _object->manageThreads( _idThread );
    }
  }
  catch( ZThread::Interrupted_Exception& e ) {
    cout << _idThread << ": exiting..." << endl;
  }
}

void simpleThreads() {
  cout << "Press <Enter> to exit." << endl;

  CountedPtr< SharedObject > object( new SharedObject );

  ZThread::PoolExecutor executor( g::countThreads );
  for( int i = 0; i < g::countThreads; ++i ) {
    executor.execute( new SimpleThread( i, object ) );
  }

  cin.ignore();
//   t.interrupt();
  object->broadcast();
  executor.interrupt();
}

enum st {
  st_working = 1,
  st_paused = 2,
  st_stopped = 3
};

string st_string( st state) {
  switch( state ) {
    case st_working: return "working";
    case st_paused: return "paused";
    case st_stopped: return "stopped";
    default: return "invalid";
  }
}


class PausedThread : public ZThread::Runnable {
private:
  ZThread::Condition _condition;
  ZThread::Mutex _mutex;
  st _state;
public:
  PausedThread();
  void run();
  void work();
  void stop();
  void pause();
  st state() { return _state; }
};

void PausedThread::work() {
  _state = st_working;
  _condition.signal();
  cout << "working..." << endl;
}

void PausedThread::pause() {
  _state = st_paused;
  cout << "paused..." << endl;
}

void PausedThread::stop() {
  _state = st_stopped;
  _condition.signal();
  cout << "stopped..." << endl;
}

PausedThread::PausedThread() : _condition( _mutex ), _state( st_paused ) {
}

void PausedThread::run() {
  try {
    int counter = 0;
    while( _state != st_stopped ) {
      Guard< Mutex > guard( _mutex );
      ZThread::Thread::sleep( g::sleep / 2 );
      cout << "PausedThread::run()" << endl;
      cout << "state = " << st_string( _state ) << endl;

      if( _state == st_paused )
        _condition.wait();
    }
  }
  catch( ZThread::Interrupted_Exception& e ) {
    cout << e.what() << endl;
  }
  cout << "Exiting PausedThread::run()" << endl;
}


class TestPausedClass : public ZThread::Runnable {
private:
  PausedThread& _thread;
  ZThread::Condition _condition;
  ZThread::Mutex _mutex;
public:
  TestPausedClass( PausedThread& );
  void run();
};


TestPausedClass::TestPausedClass( PausedThread& p )
: _thread( p ), _condition( _mutex ) {
}

void TestPausedClass::run() {
  try {
    int counter = 0;
    while( counter < 5 ) {
      Guard< Mutex > guard( _mutex );
      ZThread::Thread::sleep( g::sleep * 2 );

      cout << "counter = " << ++counter << endl;

      if( _thread.state() == st_working )
        _thread.pause();
      else
      if( _thread.state() == st_paused )
        _thread.work();
    }
    _thread.stop();
  }
  catch( ZThread::Interrupted_Exception& e ) {
    cout << e.what() << endl;
  }
  cout << "Exiting TestPausedClass::run()" << endl;
}



void testPause() {
  try {
    CountedPtr< PausedThread > pthread( new PausedThread() );
    CountedPtr< TestPausedClass > tpc( new TestPausedClass( *pthread ) );
    ThreadedExecutor executor;
    executor.execute( pthread );
    executor.execute( tpc );

    cout << "Press <Enter> to exit." << endl;
    cin.ignore();
  }
  catch( ZThread::Synchronization_Exception& e ) {
    cout << e.what() << endl;
  }
}


class Class1: public ZThread::Runnable {
    ZThread::Mutex lock;
    ZThread::Condition condition;
public:
    Class1(): condition(lock) {}

    void run() {
      while ( true ) {
        ZThread::Guard<ZThread::Mutex> g(lock);
        std::cout << "Class1::run()\n";
        condition.wait();
//         std::cout << "Message\n";
      }
      std::cout << "Exiting Class2::run()\n";
    }

    void continue1() {
        ZThread::Guard<ZThread::Mutex> g(lock);
        std::cout << "Class1::continue1()\n";
        condition.signal();
    }
};

class Class2: public ZThread::Runnable {
    ZThread::CountedPtr<Class1>& c;
public:
    Class2(ZThread::CountedPtr<Class1>& c1): c(c1) {}

    void run() {
      while ( true ) {
        std::cout << "Class2::run()\n";
        ZThread::Thread::sleep( g::sleep );
        std::cout << "Calling continue1()\n";
        c->continue1();
      }
      std::cout << "Exiting Class2::run()\n";
    }
};


void test2() {
    ZThread::CountedPtr<Class1> c(new Class1);
//     cout << "Starting thread 1\n";
//     ZThread::Thread t1(c);
//     cout << "Starting thread 2\n";
//     ZThread::Thread t2(new Class2(c));
    ThreadedExecutor executor;
    executor.execute( c );
    executor.execute( new Class2(c) );

    cout << "Press <Enter> to exit." << endl;
    cin.ignore();
    cout << "exit\n";
}


int main(int argc, char **argv) {
//   cout << "Press <Enter> to exit." << endl;
  initInputData( argc, argv );

//   simpleThreads();
  try {
  cookingToast();
  }
  catch( std::exception& e ) {
    cout << e.what() << endl;
  }
  return 0;
//   testPause();
//   test2();

  return 0;
}
