// Solution Two Dimensional Geometry Problem
//
// File:	vec-2d.cc
// Authors:	Bob Walton (walton@acm.org)
// Date:	Tue Dec 15 04:01:32 EST 2020
//
// The authors have placed this program in the public
// domain; they make no warranty and accept no liability
// for this program.

// Assert failure is on input line after last line
// printed.

// C++ Data Structures
//

#include <iostream>
#include <cstring>
#include <cctype>
#include <cassert>
#include <cmath>    // PI is M_PI

using std::cin;
using std::cout;
using std::endl;
using std::ostream;

struct vec { double x, y; };
struct linear { vec lx, ly; };
struct element { vec v; element * next; };
enum type { NONE = 0, BOOLEAN, SCALAR, VECTOR,
            LINEAR, LIST };
struct var {
    type t;
    bool b;
    double s; // scalar
    vec v;
    linear l;
    element * first;
        // First element of vector LIST.
};
double units[13] =  // units[d] = 10**(-d)
    { 1e-0, 1e-1, 1e-2, 1e-3, 1e-4, 1e-5, 1e-6,
      1e-7, 1e-8, 1e-9, 1e-10, 1e-11, 1e-12 };


// C++ Vector Function Signatures
// --- ------ -------- ----------
//
// For Scalar Calculator
//
int lt ( double x, double y, int d );   // x<y:d
int le ( double x, double y, int d );   // x<=y:d
int eq ( double x, double y, int d );   // x==y:d

// For Vector Calculator
//
vec new_vec ( double x, double y );      // (x,y)
bool eq ( vec v, vec w, int d );         // v==w:d
std::ostream & operator <<
        ( std::ostream & s, vec v );
vec operator * ( double s, vec v );      // s*v
vec operator + ( vec v, vec w );         // v+w
vec operator - ( vec v, vec w );         // v-w
vec operator - ( vec v );                // -v
vec zerov = { 0, 0 };                    // (0,0)
double len ( vec v );                    // ||v||
double azm ( vec v );                    // azm v
vec polar ( double l, double t );        // l^t

// For Linear Calculator
//
linear new_linear ( vec v, vec w );     // (v,w)
int eq ( linear L, linear K, int d );   // L==K:d
std::ostream & operator <<
        ( std::ostream & s, linear L );
vec operator * ( linear L, vec v );       // L*v
    // application
linear operator * ( double s, linear L ); // s*L
linear operator + ( linear K, linear L ); // K+L
linear operator - ( linear K, linear L ); // K-L
linear operator - ( linear L );           // -L
linear operator * ( linear K, linear L ); // K*L
    // composition

// For Rotations and Reflections Calculator
//
vec rotate ( vec v, double p );     // v^p
vec reflect ( vec v, double p );    // v|p
linear rotation ( double p );       // ^p
linear reflection ( double p );     // |p

// For Product Calculator
//
vec operator * ( vec v, vec w );      // v*w
vec cross ( vec v, vec w );           // v#w
vec change ( vec v, vec w );          // v:w
vec unit ( vec v );                   // v!
vec uchange ( vec v, vec w );         // v!w
double area ( vec p, vec q, vec r );  // area pqr

// C++ Main Program
// --- ---- -------

const int MAX_LINE = 100000;

char line[MAX_LINE+2];
char squashed[MAX_LINE+2];

var vars[128];
    // Implicitly initialized to
    // vars[...].t == 0 == NONE,

// Read and return scalar.
//
double read_scalar ( char * & p )
{
    char * endp;
    double r = strtod ( p, & endp );
    assert ( endp > p );
    p = endp;
    return r;
}

// Read and return vector.
//
vec read_vector ( char * & p )
{
    vec r;
    assert ( * p ++ == '(' );
    r.x = read_scalar ( p );
    assert ( * p ++ = ',' );
    r.y = read_scalar ( p );
    assert ( * p ++ = ')' );
    return r;
}

// Print vector.
//
ostream & operator << ( ostream & s, vec v )
{
    return s << "(" << v.x << "," << v.y << ")";
}

// Match a pattern to the squashed input line.  Returns
// true iff successful.  Sets V[.] to variables matched
// to $ in left-to-right order:
// //
var * V[4];
# define R   (*V[0])    // Result
# define OP1 (*V[1])    // First Operand
# define OP2 (*V[2])    // Second Operand
# define OP3 (*V[3])    // Third Operand
bool match ( const char * pattern )
{
    const char * p = squashed, * q = pattern;
    int i = 0;
    while ( * p != 0 )
    {
        if ( * q == '$' )
	{
	    if ( ! isalpha ( * p ) ) return false;
	    assert ( i < 4 );
	    V[i++] = & vars[*p++];
	    q ++;
	}
	else if ( * q ++ != * p ++ ) return false;
    }
    if ( * q != 0 ) return false;
    else return true;
}

bool compute_result ( void );  // See below.
int main ( int argc, char * argv[] )
{
    while ( cin.getline ( line, sizeof ( line ) ),
            cin.good() )
    {
        assert ( strlen ( line ) <= MAX_LINE );
	    // Line is too long.

	// Set squashed to line with whitespace
	// characters deleted.
	//
	char * p = squashed, * q = line;
	bool has_digit;
	while ( true )
	{ 
	    * p = * q ++;
	    if ( * p  == 0 ) break;
	    if ( ! isspace ( * p ) ) ++ p;
	}

	if ( squashed[0] == '#' )
	{
	    cout << line << endl;
	    continue;
	}

	p = squashed;
	has_digit = false;
	while ( * p && ! has_digit )
	    has_digit = isdigit ( * p ++ );

	if ( has_digit )
	{
	    p = squashed;

	    assert ( isalpha ( p[0] ) );
		// First line character is not variable.
	    assert ( p[1] == '=' );
		// Second line character is not '='.
	
	    V[0] = & vars[p[0]];
	    p += 2;

	    if ( p[0] == '[' )
	    {
		R.t = LINEAR;
		++ p;
		R.l.lx = read_vector ( p );
		assert ( * p ++ == ',' );
		R.l.ly = read_vector ( p );
		assert ( * p ++ == ']' );
		assert ( * p == 0 );
		cout << line << endl;
		continue;
	    }

	    else if ( p[0] == '(' && p[1] == '(' )
	    {
		R.t = LIST;
		element * last = NULL;
		++ p;
		while ( true )
		{
		    element & e = * new element;
		    e.next = NULL;
		    if ( last == NULL )
			R.first = & e;
		    else
			last->next = & e;
		    last = & e;
		    e.v = read_vector ( p );
		    if ( * p == ')' )
		    {
			++ p;
			assert ( * p == 0 );
			break;
		    }
		    assert ( * p ++ == ',' );
		}
		cout << line << endl;
		continue;
	    }

	    else if ( p[0] == '(' )
	    {
		R.t = VECTOR;
		R.v = read_vector ( p );
		assert ( * p == 0 );
		cout << line << endl;
		continue;
	    }

	    else
	    {
		R.t = SCALAR;
		R.s = read_scalar ( p );
		assert ( * p == 0 );
		cout << line << endl;
		continue;
	    }
	}

	// Has_digit is false from here on, and we
	// use exclusively the `match' function to do
	// the rest of the parsing.

	if ( match ( "$=true" ) )
	{
	    R.t = BOOLEAN;
	    R.b = true;
	    cout << line << endl;
	    continue;
	}
	if ( match ( "$=false" ) )
	{
	    R.t = BOOLEAN;
	    R.b = false;
	    cout << line << endl;
	    continue;
	}
	if ( match ( "$=()" ) )
	{
	    R.t = LIST;
	    R.first = NULL;
	    cout << line << endl;
	    continue;
	}

	// Compute_result() does the rest of the calls
	// to match an result computation.  Returns true
	// on success and false if operation not
	// recognized.
	//
	if ( compute_result() )
	{
	    cout << line << " = ";
	    switch ( R.t )
	    {
	    case BOOLEAN:
		cout << ( R.b ? "true" : "false" );
		break;
	    case SCALAR:
		cout << R.s; break;
	    case VECTOR:
		cout << R.v; break;
	    case LINEAR:
		cout << "[" << R.l.lx << ","
			    << R.l.ly << "]";
		break;
	    case LIST:
		cout << "(";
		{
		    element * ep = R.first;
		    while ( ep != NULL )
		    {
			if ( ep != R.first ) cout << ",";
			cout << ep->v;
			ep = ep->next;
		    }
		}
		cout << ")";
		break;
	    default:
		cout << "(BAD RESULT TYPE "
		     << R.t << ")";
	    }
	    cout << endl;
	}
        else
	{
	    cout << "ERROR: unrecognized operation in:"
	         << endl << "    " << line << endl;
	    exit ( 1 );
	}
    }

    return 0;
}

// C++ Computational Functions
// --- ------------- ---------

bool lt ( double x, double y, double d )
{
    assert ( 0 <= d && d <= 12 );
    int i = int ( d );
    assert ( i == d );
    return x < y - 0.5 * units[i];
}

bool le ( double x, double y, double d )
{
    assert ( 0 <= d && d <= 12 );
    int i = int ( d );
    assert ( i == d );
    return x < y + 0.5 * units[i];
}

bool eq ( double x, double y, double d )
{
    assert ( 0 <= d && d <= 12 );
    int i = int ( d );
    assert ( i == d );
    return fabs ( x - y ) < 0.5 * units[i];
}

bool eq ( vec v, vec w, double d )
{
    return eq ( v.x, w.x, d ) && eq ( v.y, w.y, d );
}

vec operator * ( double s, vec v )
{
    vec r = { s*v.x, s*v.y };
    return r;
}

vec operator - ( vec v )
{
    vec r = { - v.x, - v.y };
    return r;
}

vec operator + ( vec v, vec w )
{
    vec r = { v.x + w.x, v.y + w.y };
    return r;
}

vec operator - ( vec v, vec w )
{
    vec r = { v.x - w.x, v.y - w.y };
    return r;
}

double length ( vec v )
{
    return sqrt ( v.x*v.x + v.y*v.y );
}

double azm ( vec v )
{
    assert ( v.x != 0 || v.y != 0 );
        // Judge's assertion to be sure v == 0 does
	// not happen.
    return (180/M_PI) * atan2 ( v.y, v.x );
}

vec exp ( double l, double t )
{
    double theta = ( M_PI / 180 ) * t;
    vec r = { l * cos(theta), l * sin(theta) };
    return r;
}

// C++ Compute Result Function
// --- ------- ------ --------

bool compute_result ( void )
{
    if ( match ( "$=$" ) )
    {
	R = OP1;
	return true;
    }
    if ( match ( "$=$+$" ) )
    {
	assert ( OP1.t == OP2.t );
	if ( OP1.t == SCALAR )
	{
	    R.t = SCALAR;
	    R.s = OP1.s + OP2.s;
	}
	else if ( OP1.t == VECTOR )
	{
	    R.t = VECTOR;
	    R.v = OP1.v + OP2.v;
	}
	else
	    return false;
	return true;
    }
    if ( match ( "$=$-$" ) )
    {
	assert ( OP1.t == OP2.t );
	if ( OP1.t == SCALAR )
	{
	    R.t = SCALAR;
	    R.s = OP1.s - OP2.s;
	}
	else if ( OP1.t == VECTOR )
	{
	    R.t = VECTOR;
	    R.v = OP1.v - OP2.v;
	}
	else
	    return false;
	return true;
    }
    if ( match ( "$=$*$" ) )
    {
	if ( OP2.t == SCALAR )
	{
	    assert ( OP1.t == SCALAR );
	    R.t = SCALAR;
	    R.s = OP1.s * OP2.s;
	}
	else if ( OP2.t == VECTOR )
	{
	    assert ( OP1.t == SCALAR );
	    R.t = VECTOR;
	    R.v = OP1.s * OP2.v;
	}
	else
	    return false;
	return true;
    }
    if ( match ( "$=$/$" ) )
    {
	assert ( OP1.t == SCALAR );
	assert ( OP2.t == SCALAR );
	R.t = SCALAR;
	R.s = OP1.s / OP2.s;
	return true;
    }
    if ( match ( "$=$%$" ) )
    {
	assert ( OP1.t == SCALAR );
	assert ( OP2.t == SCALAR );
	R.t = SCALAR;
	R.s = fmod ( OP1.s, OP2.s );
	return true;
    }
    if ( match ( "$=-$" ) )
    {
	if ( OP1.t == SCALAR )
	{
	    R.t = SCALAR;
	    R.s = - OP1.s;
	}
	else if ( OP1.t == VECTOR )
	{
	    R.t = VECTOR;
	    R.v = - OP1.v;
	}
	else
	    return false;
	return true;
    }
    if ( match ( "$=|$|" ) )
    {
	assert ( OP1.t == SCALAR );
	R.t = SCALAR;
	R.s = fabs ( OP1.s );
	return true;
    }
    if ( match ( "$=sin$" ) )
    {
	assert ( OP1.t == SCALAR );
	R.t = SCALAR;
	R.s = sin ( ( M_PI / 180 ) * OP1.s );
	return true;
    }
    if ( match ( "$=cos$" ) )
    {
	assert ( OP1.t == SCALAR );
	R.t = SCALAR;
	R.s = cos ( ( M_PI / 180 ) * OP1.s );
	return true;
    }
    if ( match ( "$=tan$" ) )
    {
	assert ( OP1.t == SCALAR );
	R.t = SCALAR;
	R.s = tan ( ( M_PI / 180 ) * OP1.s );
	return true;
    }
    if ( match ( "$=$<$:$" ) )
    {
	assert ( OP3.t == SCALAR );
	if ( OP1.t == SCALAR )
	{
	    assert ( OP2.t == SCALAR );
	    R.t = BOOLEAN;
	    R.b = lt ( OP1.s, OP2.s, OP3.s );
	}
	else
	    return false;
	return true;
    }
    if ( match ( "$=$<=$:$" ) )
    {
	assert ( OP3.t == SCALAR );
	if ( OP1.t == SCALAR )
	{
	    assert ( OP2.t == SCALAR );
	    R.t = BOOLEAN;
	    R.b = le ( OP1.s, OP2.s, OP3.s );
	}
	else
	    return false;
	return true;
    }
    if ( match ( "$=$==$:$" ) )
    {
	assert ( OP3.t == SCALAR );
	if ( OP1.t == SCALAR )
	{
	    assert ( OP2.t == SCALAR );
	    R.t = BOOLEAN;
	    R.b = eq ( OP1.s, OP2.s, OP3.s );
	}
	else if ( OP1.t == VECTOR )
	{
	    assert ( OP2.t == VECTOR );
	    R.t = BOOLEAN;
	    R.b = eq ( OP1.v, OP2.v, OP3.s );
	}
	else
	    return false;
	return true;
    }

    if ( match ( "$=($,$)" ) )
    {
	if ( OP1.t == SCALAR )
	{
	    assert ( OP2.t == SCALAR );
	    R.t = VECTOR;
	    R.v.x = OP1.s;
	    R.v.y = OP2.s;
	}
	else
	    return false;
	return true;
    }
    if ( match ( "$=$^$" ) )
    {
	assert ( OP2.t == SCALAR );
        if ( OP1.t == SCALAR )
	{
	    R.t = VECTOR;
	    R.v = exp ( OP1.s, OP2.s );
	}
	else
	    return false;
	return true;
    }

    if ( match ( "$=||$||" ) )
    {
	if ( OP1.t == VECTOR )
	{
	    R.t = SCALAR;
	    R.s = length ( OP1.v );
	}
	else
	    return false;
	return true;
    }

    if ( match ( "$=azm$" ) )
    {
	if ( OP1.t == VECTOR )
	{
	    R.t = SCALAR;
	    R.s = azm ( OP1.v );
	}
	else
	    return false;
	return true;
    }

    if ( match ( "$=$.x" ) )
    {
	assert ( OP1.t == VECTOR );
	R.t = SCALAR;
	R.s = OP1.v.x;
	return true;
    }

    if ( match ( "$=$.y" ) )
    {
	assert ( OP1.t == VECTOR );
	R.t = SCALAR;
	R.s = OP1.v.y;
	return true;
    }

    return false;
}
