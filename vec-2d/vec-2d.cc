// Solution Two Dimensional Geometry Problem
//
// File:	vec-2d.cc
// Authors:	Bob Walton (walton@acm.org)
// Date:	Tue Dec 29 04:22:13 EST 2020
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
bool lt ( double x, double y, int d );	// x<y:d
bool le ( double x, double y, int d );	// x<=y:d
bool eq ( double x, double y, int d );	// x==y:d

// For Vector Calculator
//
vec new_vec ( double x, double y );	// (x,y)
bool eq ( vec v, vec w, int d );	// v==w:d
ostream & operator << ( ostream & s, vec v );
vec operator * ( double s, vec v );	// s*v
vec operator + ( vec v, vec w );	// v+w
vec operator - ( vec v, vec w );	// v-w
vec operator - ( vec v );		// -v
vec zerov = { 0, 0 };			// (0,0)
vec uxv = { 1, 0 };			// (1,0)
vec uyv = { 0, 1 };			// (0,1)
double len ( vec v );			// ||v||
double azm ( vec v );			// azm v
vec polar ( double l, double t );	// l^t

// For Linear Calculator
//
linear new_linear ( vec v, vec w );	    // (v,w)
int eq ( linear L, linear K, int d );	    // L==K:d
ostream & operator << ( ostream & s, linear L );
vec operator * ( linear L, vec v );	    // L*v
    // application
linear operator * ( double s, linear L );    // s*L
linear operator + ( linear K, linear L );    // K+L
linear operator - ( linear K, linear L );    // K-L
linear operator - ( linear L );		     // -L
linear operator * ( linear K, linear L );    // K*L
    // composition

// For Rotations and Reflections Calculator
//
vec rotate ( vec v, double p );		// v^p
vec reflect ( vec v, double p );	// v|p
linear rotation ( double p );		// ^p
linear reflection ( double p );		// |p

// For Product Calculator
//
double operator * ( vec v, vec w );	// v*w
double cross ( vec v, vec w );		// v#w
vec change ( vec v, vec w );		// v:w
vec unit ( vec v );			// v!
vec uchange ( vec v, vec w );		// v!w
double area ( vec p, vec q, vec r );	// area pqr

// For Line and Point Calculator
//
double disti				//disti pqr
    ( vec p, vec q, vec r );
double distf				//distf pqr
    ( vec p, vec q, vec r );
vec closei				//closei pqr
    ( vec p, vec q, vec r );
vec closef				//closef pqr
    ( vec p, vec q, vec r );
double sidei				//sidei pqr
    ( vec p, vec q, vec r, double d );
bool onf				//onf pqr
    ( vec p, vec q, vec r, double d );

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
# define RES (*V[0])    // Result
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
		RES.t = LINEAR;
		++ p;
		RES.l.lx = read_vector ( p );
		assert ( * p ++ == ',' );
		RES.l.ly = read_vector ( p );
		assert ( * p ++ == ']' );
		assert ( * p == 0 );
		cout << line << endl;
		continue;
	    }

	    else if ( p[0] == '(' && p[1] == '(' )
	    {
		RES.t = LIST;
		element * last = NULL;
		++ p;
		while ( true )
		{
		    element & e = * new element;
		    e.next = NULL;
		    if ( last == NULL )
			RES.first = & e;
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
		RES.t = VECTOR;
		RES.v = read_vector ( p );
		assert ( * p == 0 );
		cout << line << endl;
		continue;
	    }

	    else
	    {
		RES.t = SCALAR;
		RES.s = read_scalar ( p );
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
	    RES.t = BOOLEAN;
	    RES.b = true;
	    cout << line << endl;
	    continue;
	}
	if ( match ( "$=false" ) )
	{
	    RES.t = BOOLEAN;
	    RES.b = false;
	    cout << line << endl;
	    continue;
	}
	if ( match ( "$=()" ) )
	{
	    RES.t = LIST;
	    RES.first = NULL;
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
	    switch ( RES.t )
	    {
	    case BOOLEAN:
		cout << ( RES.b ? "true" : "false" );
		break;
	    case SCALAR:
		cout << RES.s; break;
	    case VECTOR:
		cout << RES.v; break;
	    case LINEAR:
		cout << "[" << RES.l.lx << ","
			    << RES.l.ly << "]";
		break;
	    case LIST:
		cout << "(";
		{
		    element * ep = RES.first;
		    while ( ep != NULL )
		    {
			if ( ep != RES.first )
			    cout << ",";
			cout << ep->v;
			ep = ep->next;
		    }
		}
		cout << ")";
		break;
	    default:
		cout << "(BAD RESULT TYPE "
		     << RES.t << ")";
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

// Scalar Calculator Functions
// ------ ---------- ---------

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

// Vector Calculator Functions
// ------ ---------- ---------

vec new_vec ( double x, double y )
{
    vec r = { x, y };
    return r;
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

double len ( vec v )
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

vec polar ( double l, double t )
{
    double theta = ( M_PI / 180 ) * t;
    vec r = { l * cos(theta), l * sin(theta) };
    return r;
}

// Linear Transform Calculator Functions
// ------ --------- ---------- ---------

linear new_linear ( vec lx, vec ly )	    // [lx,ly]
{
    linear r = { lx, ly };
    return r;
}

bool eq ( linear K, linear L, double d )    // K==L:d
{
    return eq ( K.lx, L.lx, d ) && eq ( K.ly, L.ly, d );
}

vec operator * ( linear L, vec v )	    // L*v
{
    return v.x * L.lx + v.y * L.ly; 
}

linear operator * ( double s, linear K )    // s*K
{
    linear r = { s * K.lx, s * K.ly };
    return r;
}

linear operator + ( linear K, linear L )    // K+L
{
    linear r = { K.lx + L.lx, K.ly + L.ly };
    return r;
}

linear operator - ( linear K, linear L )    // K-L
{
    linear r = { K.lx - L.lx, K.ly - L.ly };
    return r;
}

linear operator - ( linear L )		    // -L
{
    linear r = { - L.lx, - L.ly };
    return r;
}

linear operator * ( linear K, linear L )    // K*L
{
    linear r = { K * L.lx, K * L.ly };
    return r;
}

// Rotations and Reflections Calculator Functions
// --------- --- ----------- ---------- ---------

vec rotate ( vec v, double p )		// v^p
{
    return polar ( len ( v ), azm ( v ) + p );
}
vec reflect ( vec v, double p )		// v|p
{
    return polar ( len ( v ), 2 * p - azm ( v ) );
}
linear rotation ( double p )		// ^p
{
    return new_linear ( rotate ( uxv, p ),
                        rotate ( uyv, p ) );
}
linear reflection ( double p )		// |p
{
    return new_linear ( reflect ( uxv, p ),
                        reflect ( uyv, p ) );
}

// Product Calculator Functions
// ------- ---------- ---------
//
double operator * ( vec v, vec w )	// v*w
{
    return v.x * w.x + v.y * w.y;
}
double cross ( vec v, vec w )		// v#w
{
    return - v.y * w.x + v.x * w.y;
}
vec change ( vec v, vec w )		// v:w
{
    return new_vec ( v*w, cross ( v, w ) );
}
vec unit ( vec v )			// v!
{
    double length = len ( v );
    return ( 1 / length ) * v;
}
vec uchange ( vec v, vec w )		// v!w
{
    vec u = unit ( v );
    return change ( u, w );
}
double area ( vec p, vec q, vec r )	// area pqr
{
    return 0.5 * cross ( r - p, q - p );
}

// Line and Point Calculator Functions
//
double disti				//disti pqr
    ( vec p, vec q, vec r )
{
    double length = len ( q - p );
    return   ( 1 / length )
           * fabs ( cross ( q - p, r - p ) );
}
double distf				//distf pqr
    ( vec p, vec q, vec r )
{
    vec Q = uchange ( q - p, q - p );
    vec R = uchange ( q - p, r - p );
    if ( R.x < 0 ) return len ( r - p );
    else if ( R.x > Q.x ) return len ( r - q );
    else return fabs ( R.y );
}
    
vec closei				//closei pqr
    ( vec p, vec q, vec r )
{
    return p + ( ((q-p)*(r-p)) / ((q-p)*(q-p)) )
             * (q-p);
}
vec closef				//closef pqr
    ( vec p, vec q, vec r )
{
    vec Q = uchange ( q - p, q - p );
    vec R = uchange ( q - p, r - p );
    if ( R.x < 0 ) return p;
    else if ( R.x > Q.x ) return q;
    else return closei ( p, q, r );
}
double sidei				//sidei pqr
    ( vec p, vec q, vec r, double d )
{
    vec R = change ( q - p, r - p );
    if ( eq ( 0, R.y, 2*d ) ) return 0;
    else if ( R.y < 0 ) return +1;
    else return -1;
}
bool onf				//onf pqr
    ( vec p, vec q, vec r, double d )
{
    vec Q = change ( q - p, q - p );
    vec R = change ( q - p, r - p );
    if ( ! eq ( 0, R.y, 2*d ) ) return false;
    else if ( lt ( R.y, 0, 2*d ) ) return false;
    else if ( lt ( Q.y, R.y, 2*d ) ) return false;
    else return true;
}

// C++ Compute Result Function
// --- ------- ------ --------

bool compute_result ( void )
{
    if ( match ( "$=$" ) )
    {
	RES = OP1;
	return true;
    }
    if ( match ( "$=$+$" ) )
    {
	assert ( OP1.t == OP2.t );
	if ( OP1.t == SCALAR )
	{
	    RES.t = SCALAR;
	    RES.s = OP1.s + OP2.s;
	}
	else if ( OP1.t == VECTOR )
	{
	    RES.t = VECTOR;
	    RES.v = OP1.v + OP2.v;
	}
	else if ( OP1.t == LINEAR )
	{
	    RES.t = LINEAR;
	    RES.l = OP1.l + OP2.l;
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
	    RES.t = SCALAR;
	    RES.s = OP1.s - OP2.s;
	}
	else if ( OP1.t == VECTOR )
	{
	    RES.t = VECTOR;
	    RES.v = OP1.v - OP2.v;
	}
	else if ( OP1.t == LINEAR )
	{
	    RES.t = LINEAR;
	    RES.l = OP1.l - OP2.l;
	}
	else
	    return false;
	return true;
    }
    if ( match ( "$=$*$" ) )
    {
	if ( OP1.t == SCALAR 
	     &&
	     OP2.t == SCALAR )
	{
	    RES.t = SCALAR;
	    RES.s = OP1.s * OP2.s;
	}
	else if ( OP1.t == SCALAR 
		  &&
		  OP2.t == VECTOR )
	{
	    RES.t = VECTOR;
	    RES.v = OP1.s * OP2.v;
	}
	else if ( OP1.t == SCALAR 
		  &&
		  OP2.t == LINEAR )
	{
	    RES.t = LINEAR;
	    RES.l = OP1.s * OP2.l;
	}
	else if ( OP1.t == LINEAR 
		  &&
		  OP2.t == VECTOR )
	{
	    RES.t = VECTOR;
	    RES.v = OP1.l * OP2.v;
	}
	else if ( OP1.t == LINEAR 
		  &&
		  OP2.t == LINEAR )
	{
	    RES.t = LINEAR;
	    RES.l = OP1.l * OP2.l;
	}
	else if ( OP1.t == VECTOR 
		  &&
		  OP2.t == VECTOR )
	{
	    RES.t = SCALAR;
	    RES.s = OP1.v * OP2.v;
	}
	else
	    return false;
	return true;
    }
    if ( match ( "$=$#$" ) )
    {
	assert ( OP1.t == VECTOR );
	assert ( OP2.t == VECTOR );
	RES.t = SCALAR;
	RES.s = cross ( OP1.v, OP2.v );
	return true;
    }
    if ( match ( "$=$:$" ) )
    {
	assert ( OP1.t == VECTOR );
	assert ( OP2.t == VECTOR );
	RES.t = VECTOR;
	RES.v = change ( OP1.v, OP2.v );
	return true;
    }
    if ( match ( "$=$!$" ) )
    {
	assert ( OP1.t == VECTOR );
	assert ( OP2.t == VECTOR );
	RES.t = VECTOR;
	RES.v = uchange ( OP1.v, OP2.v );
	return true;
    }
    if ( match ( "$=$/$" ) )
    {
	assert ( OP1.t == SCALAR );
	assert ( OP2.t == SCALAR );
	RES.t = SCALAR;
	RES.s = OP1.s / OP2.s;
	return true;
    }
    if ( match ( "$=$%$" ) )
    {
	assert ( OP1.t == SCALAR );
	assert ( OP2.t == SCALAR );
	RES.t = SCALAR;
	RES.s = fmod ( OP1.s, OP2.s );
	return true;
    }
    if ( match ( "$=-$" ) )
    {
	if ( OP1.t == SCALAR )
	{
	    RES.t = SCALAR;
	    RES.s = - OP1.s;
	}
	else if ( OP1.t == VECTOR )
	{
	    RES.t = VECTOR;
	    RES.v = - OP1.v;
	}
	else if ( OP1.t == LINEAR )
	{
	    RES.t = LINEAR;
	    RES.l = - OP1.l;
	}
	else
	    return false;
	return true;
    }
    if ( match ( "$=!$" ) )
    {
	assert ( OP1.t == VECTOR );
	RES.t = VECTOR;
	RES.v = unit ( OP1.v );
	return true;
    }
    if ( match ( "$=|$|" ) )
    {
	assert ( OP1.t == SCALAR );
	RES.t = SCALAR;
	RES.s = fabs ( OP1.s );
	return true;
    }
    if ( match ( "$=sin$" ) )
    {
	assert ( OP1.t == SCALAR );
	RES.t = SCALAR;
	RES.s = sin ( ( M_PI / 180 ) * OP1.s );
	return true;
    }
    if ( match ( "$=cos$" ) )
    {
	assert ( OP1.t == SCALAR );
	RES.t = SCALAR;
	RES.s = cos ( ( M_PI / 180 ) * OP1.s );
	return true;
    }
    if ( match ( "$=tan$" ) )
    {
	assert ( OP1.t == SCALAR );
	RES.t = SCALAR;
	RES.s = tan ( ( M_PI / 180 ) * OP1.s );
	return true;
    }
    if ( match ( "$=area$$$" ) )
    {
	assert ( OP1.t == VECTOR );
	assert ( OP2.t == VECTOR );
	assert ( OP3.t == VECTOR );
	RES.t = SCALAR;
	RES.s = area ( OP1.v, OP2.v, OP3.v );
	return true;
    }
    if ( match ( "$=$<$:$" ) )
    {
	assert ( OP3.t == SCALAR );
	if ( OP1.t == SCALAR )
	{
	    assert ( OP2.t == SCALAR );
	    RES.t = BOOLEAN;
	    RES.b = lt ( OP1.s, OP2.s, OP3.s );
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
	    RES.t = BOOLEAN;
	    RES.b = le ( OP1.s, OP2.s, OP3.s );
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
	    RES.t = BOOLEAN;
	    RES.b = eq ( OP1.s, OP2.s, OP3.s );
	}
	else if ( OP1.t == VECTOR )
	{
	    assert ( OP2.t == VECTOR );
	    RES.t = BOOLEAN;
	    RES.b = eq ( OP1.v, OP2.v, OP3.s );
	}
	else if ( OP1.t == LINEAR )
	{
	    assert ( OP2.t == LINEAR );
	    RES.t = BOOLEAN;
	    RES.b = eq ( OP1.l, OP2.l, OP3.s );
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
	    RES.t = VECTOR;
	    RES.v = new_vec ( OP1.s, OP2.s );
	}
	else if ( OP1.t == VECTOR )
	{
	    assert ( OP2.t == VECTOR );
	    RES.t = LINEAR;
	    RES.l = new_linear ( OP1.v, OP2.v );
	}
	else
	    return false;
	return true;
    }

    if ( match ( "$=[$,$]" ) )
    {
	assert ( OP1.t == VECTOR );
	assert ( OP2.t == VECTOR );
	RES.t = LINEAR;
	RES.l = new_linear ( OP1.v, OP2.v );
	return true;
    }

    if ( match ( "$=$^$" ) )
    {
	assert ( OP2.t == SCALAR );
        if ( OP1.t == SCALAR )
	{
	    RES.t = VECTOR;
	    RES.v = polar ( OP1.s, OP2.s );
	}
        else if ( OP1.t == VECTOR )
	{
	    RES.t = VECTOR;
	    RES.v = rotate ( OP1.v, OP2.s );
	}
	else
	    return false;
	return true;
    }

    if ( match ( "$=$|$" ) )
    {
	assert ( OP1.t == VECTOR );
	assert ( OP2.t == SCALAR );
	RES.t = VECTOR;
	RES.v = reflect ( OP1.v, OP2.s );
	return true;
    }

    if ( match ( "$=^$" ) )
    {
	assert ( OP1.t == SCALAR );
	RES.t = LINEAR;
	RES.l = rotation ( OP1.s );
	return true;
    }

    if ( match ( "$=|$" ) )
    {
	assert ( OP1.t == SCALAR );
	RES.t = LINEAR;
	RES.l = reflection ( OP1.s );
	return true;
    }

    if ( match ( "$=||$||" ) )
    {
	if ( OP1.t == VECTOR )
	{
	    RES.t = SCALAR;
	    RES.s = len ( OP1.v );
	}
	else
	    return false;
	return true;
    }

    if ( match ( "$=azm$" ) )
    {
	if ( OP1.t == VECTOR )
	{
	    RES.t = SCALAR;
	    RES.s = azm ( OP1.v );
	}
	else
	    return false;
	return true;
    }

    if ( match ( "$=$.x" ) )
    {
	if ( OP1.t == VECTOR )
	{
	    RES.t = SCALAR;
	    RES.s = OP1.v.x;
	}
	else if ( OP1.t == LINEAR )
	{
	    RES.t = VECTOR;
	    RES.v = OP1.l.lx;
	}
	else
	    return false;
	return true;
    }

    if ( match ( "$=$.y" ) )
    {
	if ( OP1.t == VECTOR )
	{
	    RES.t = SCALAR;
	    RES.s = OP1.v.y;
	}
	else if ( OP1.t == LINEAR )
	{
	    RES.t = VECTOR;
	    RES.v = OP1.l.ly;
	}
	else
	    return false;
	return true;
    }

    return false;
}
