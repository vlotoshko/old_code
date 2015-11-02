#include <iostream>
#include <string>
#include <stdlib.h> 
#include <iomanip>

//********************************************************************
// DIRECTION ENTITY
//
// Direction and poritions functions
//********************************************************************

// Podition type
enum Position { CENTER, LEFT_WALL, BOTTOM_WALL, TOP_WALL, RIGHT_WALL, OUT_OF_TERRITORY };
// Direction type
enum Direction { DOWN, UP, LEFT, RIGHT, DOWN_LEFT, DOWN_RIGTH, UP_LEFT, UP_RIGHT, NONE };

//--------------------------------------------------------------------
// Reverse direction
//--------------------------------------------------------------------
Direction Back (Direction direction) {
  switch ( direction ) {
    case DOWN: return UP;
    case UP: return DOWN;

    case LEFT: return RIGHT;
    case RIGHT: return LEFT;

    case DOWN_LEFT: return UP_RIGHT;
    case UP_RIGHT: return DOWN_LEFT;

    case UP_LEFT: return DOWN_RIGTH;
    case DOWN_RIGTH: return UP_LEFT;

    default: return NONE;
  }
}

//--------------------------------------------------------------------
// Returns direction away from wall. If no any wall returs NONE direction
//--------------------------------------------------------------------
Direction GoAwayFromWall(Position position) {
  switch ( position ) {
    case LEFT_WALL:
    case BOTTOM_WALL:
      return UP_RIGHT;
    case TOP_WALL:
    case RIGHT_WALL:
      return DOWN_LEFT;
    default:
      return NONE;
  }
}

//--------------------------------------------------------------------
// Returns direction along the wall. If no any wall returs NONE direction
//--------------------------------------------------------------------
Direction GoAlongWall(Position position) {
  switch ( position ) {
    case LEFT_WALL:
    case RIGHT_WALL:
      return DOWN;
    case TOP_WALL:
    case BOTTOM_WALL:
      return RIGHT;
    default:
      return NONE;
  }
}

// Simple coordinates
struct Coordinates {
  int x;
  int y;
  Coordinates() : x(0), y(0){}
};

//--------------------------------------------------------------------
// Returns position type
//--------------------------------------------------------------------
Position WhereAmI ( const Coordinates& point, const int& height, const int& width ) {

  if ( ( width - point.x < 1 ) || ( height - point.y < 1 ) )
    return OUT_OF_TERRITORY;

  if ( width - point.x == 1 )
    return RIGHT_WALL;

  if ( point.y == 0 && point.x != 0 )
    return TOP_WALL;

  if ( height - point.y == 1 )
    return BOTTOM_WALL;

  if ( point.x == 0 )
    return LEFT_WALL;

  return CENTER;
}

// Step
struct Step {
  Coordinates point; // end point
  Direction direction;
  Step() : point(), direction( NONE ){}
};


// Forward declaration
Step DoStep(const Coordinates& coordinates, const Direction& direction);

//--------------------------------------------------------------------
// Indicates whether was previous step along the wall
//--------------------------------------------------------------------
bool AlongWall(const Step& step, const int& height, const int& width) {
  Step prev_step = DoStep ( step.point, Back( step.direction ) );
  Position position = WhereAmI(prev_step.point, height, width);

  return 
  ( ( ( position == LEFT_WALL ) || ( position == RIGHT_WALL ) ) && ( step.direction == DOWN ) )
  ||
  ( ( ( position == BOTTOM_WALL ) || ( position == TOP_WALL ) ) && ( step.direction == RIGHT ) )
    ;
}

class out_of_territory{};

//--------------------------------------------------------------------
// Returns direction to next step
//--------------------------------------------------------------------
Direction Desicion( const Step& step, const int& height, const int& width ) {
  Position position = WhereAmI( step.point, height, width );

  if ( position == OUT_OF_TERRITORY )
    throw out_of_territory();

  if ( step.direction != NONE ) {
    if ( position == CENTER )
      return step.direction;
    else {
      if ( AlongWall( step, height, width ) )
	return GoAwayFromWall( position );
      else
	return GoAlongWall( position );
    }
  }
  else {
    if ( position == CENTER )
      return UP_LEFT;
    else
      return GoAlongWall( position );
  }
}

//--------------------------------------------------------------------
// Changes coordinates according direction. Returns new step
//--------------------------------------------------------------------
Step DoStep(const Coordinates& coordinates, const Direction& direction) {
  Coordinates coor = coordinates;
  switch ( direction ) {
    case DOWN: { 
      ++coor.y;
      break;
    }
    case UP:{ 
      --coor.y;
      break;
    }
    case LEFT: {
      --coor.x;
      break;
    }
    case RIGHT: {
      ++coor.x;
      break;
    }
    case DOWN_LEFT: {
      --coor.x;
      ++coor.y;
      break;
    }
    case UP_RIGHT: {
      ++coor.x;
      --coor.y;
      break;
    }
    case UP_LEFT: {
      --coor.x;
      --coor.y;
      break;
    }
    case DOWN_RIGTH: {
      ++coor.x;
      ++coor.y;
      break;
    }
    default:;
  }

  Step step;
  step.point = coor;
  step.direction = direction;

  return step;
}


//********************************************************************
// ARRAY2D ENTITY
//
// Class Array2d, containes 2-dimentional array
//********************************************************************


//--------------------------------------------------------------------
// Array2d entity class
//--------------------------------------------------------------------
class Array2d {
public:
  Array2d( const int&, const int& ); // default constructor
  ~Array2d(); // destructor
  int Height() const { return _height; }
  int Width() const { return _width; }
  int Item( const int& x, const int& y ) const { return _array[x][y]; }
  void SetItem( const int&, const int&, const int&);
  class exc_out_of_range{}; // out of range exception
private:
  Array2d( const Array2d& ){} // hidden copy constructor
  Array2d operator= ( const Array2d& ) {} // hidden assing operation
  int _height;
  int _width;
  int** _array;
};

//--------------------------------------------------------------------
// Default constructor - creates 2d-array, filled with 0, and diagonal array
// Bad example - might be raised bad_alloc exception
//--------------------------------------------------------------------
Array2d::Array2d( const int& a, const int& b ) : _height(a), _width(b) {
  _array = new int* [ Width() ];
  for ( int i = 0; i < Width(); ++i )
    _array[i] = new int[ Height() ];

  for ( int i = 0; i < Height(); ++i ) {
    for ( int j = 0; j < Width(); ++j ) {
      _array[j][i] = 0;
    }
  }
}

//--------------------------------------------------------------------
// Destructor - destroys 2d-array and diagonal array
//--------------------------------------------------------------------
Array2d::~Array2d() {
  for ( int i = 0; i < Width(); ++i )
    delete[] _array[i];
  delete[] _array;
}

//--------------------------------------------------------------------
// Operator << - showes 2d-array with intendation
//--------------------------------------------------------------------
std::ostream& operator<< ( std::ostream& o, const Array2d& a ) {
  float d = a.Height()*a.Width();
  int intend = 0;
  while ( d > 1 ) {
    ++intend;
    d /= 10;
  }
  for ( int i = 0; i < a.Height(); ++i ) {
    for ( int j = 0; j < a.Width(); ++j )
      o << std::setw( intend ) << a.Item( j,i ) << ' ';
    o << std::endl;
  }
}

//--------------------------------------------------------------------
// Sets [x][y] item by value
//--------------------------------------------------------------------
void Array2d::SetItem(const int& x, const int& y, const int& value) {
  _array[x][y] = value;
}

//--------------------------------------------------------------------
// Sets array according Desicion directions
//--------------------------------------------------------------------
void SetArray( Array2d& array ) {
  int count = array.Height() * array.Width();
  Step step;
  Direction direction;

  array.SetItem( step.point.x, step.point.y, 1 );
  for ( int i = 2; i <= count; ++i ) {
    direction = Desicion( step, array.Height(), array.Width() );
    step = DoStep( step.point,  direction);
    array.SetItem( step.point.x, step.point.y, i );
  }
}

//********************************************************************
// INPUT INTEGER FUNCTIONS
//
// Input integer from cin functions
//********************************************************************

class exc_bad_int{};

//--------------------------------------------------------------------
// Indicates whether string can be NONNEGATIVE integer
//--------------------------------------------------------------------
bool IsDigits( const std::string& str ) {
  std::string::const_iterator iter = str.begin();
  for ( iter; iter != str.end(); ++iter )
    if ( !( isdigit( *iter ) ) )
      return false;
  return true;
}

//--------------------------------------------------------------------
// Translates string to NONNEGATIVE integer. Exception exc_bad_int could be raised
//--------------------------------------------------------------------
int StringToInt( const std::string& str ) {
  if ( !( IsDigits( str ) ) )
    throw exc_bad_int();
  return atoi( str.c_str() );
}

//--------------------------------------------------------------------
// Returns NONNEGATIVE integer from cin
//--------------------------------------------------------------------
int IntegerIntput() {
  bool input = false;
  int i = 0;
  std::string str;
  while ( true ) {
    std::cin >> str;
    try {
      i = StringToInt( str );
      return i;
    }
    catch( exc_bad_int ) {
      std::cout << "Error: value is not positive integer." << std::endl;
    }
  }
}


//********************************************************************
// MAIN SECTION
//********************************************************************
int main( int argc, char **argv ) {
  std::cout << "Input array width:" << std::endl;
  int width = IntegerIntput();
  std::cout << "Input array height:" << std::endl;
  int height = IntegerIntput();

  Array2d a( height, width );
  SetArray(a);
  std::cout << a;

  return 0;
}
