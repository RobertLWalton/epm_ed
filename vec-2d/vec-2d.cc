// Solution Two Dimensional Geometry Problem
//
// File:	vec-2d.cc
// Authors:	Bob Walton (walton@acm.org)
// Date:	Sun Nov 29 02:04:41 EST 2020
//
// The authors have placed this program in the public
// domain; they make no warranty and accept no liability
// for this program.

// Assert failure is on input line after last line
// printed.

#include <iostream>
#include <cstring>
#include <cctype>
#include <cassert>
#include "cc-defs-vec-2d.txt"
using std::cin;
using std::cout;
using std::endl;
using std::ostream;

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
	    // Must be before goto PRINT_LINE.
	while ( true )
	{ 
	    * p = * q ++;
	    if ( * p  == 0 ) break;
	    if ( ! isspace ( * p ) ) ++ p;
	}

	if ( squashed[0] == '#' )
	    goto PRINT_LINE;

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
		goto PRINT_LINE;
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
			goto PRINT_LINE;
		    }
		    assert ( * p ++ == ',' );
		}
	    }

	    else if ( p[0] == '(' )
	    {
		R.t = VECTOR;
		R.v = read_vector ( p );
		assert ( * p == 0 );
		goto PRINT_LINE;
	    }

	    else
	    {
		R.t = SCALAR;
		R.s = read_scalar ( p );
		assert ( * p == 0 );
		goto PRINT_LINE;
	    }
	}

	// Has_digit is false from here on, and we
	// use exclusively the `match' function to do
	// the rest of the parsing.

	if ( match ( "$=true" ) )
	{
	    R.t = BOOLEAN;
	    R.b = true;
	    goto PRINT_LINE;
	}
	if ( match ( "$=false" ) )
	{
	    R.t = BOOLEAN;
	    R.b = false;
	    goto PRINT_LINE;
	}
	if ( match ( "$=()" ) )
	{
	    R.t = LIST;
	    R.first = NULL;
	    goto PRINT_LINE;
	}
	if ( match ( "$=$" ) )
	{
	    R = OP1;
	    goto PRINT_RESULT;
	}
	if ( match ( "$=$+$" ) )
	{
	    assert ( OP1.t == OP2.t );
	    if ( OP1.t == SCALAR )
	        R.s = OP1.s + OP2.s;
	    else
	        goto UNKNOWN_OPERATION;
	    R.t = OP1.t;
	    goto PRINT_RESULT;
	}
	if ( match ( "$=$-$" ) )
	{
	    assert ( OP1.t == OP2.t );
	    if ( OP1.t == SCALAR )
	        R.s = OP1.s - OP2.s;
	    else
	        goto UNKNOWN_OPERATION;
	    R.t = OP1.t;
	    goto PRINT_RESULT;
	}
	if ( match ( "$=$*$" ) )
	{
	    if ( OP2.t == SCALAR )
	    {
		assert ( OP1.t == SCALAR );
		R.t = SCALAR;
	        R.s = OP1.s * OP2.s;
	    }
	    else
	        goto UNKNOWN_OPERATION;
	    goto PRINT_RESULT;
	}
	if ( match ( "$=$/$" ) )
	{
	    assert ( OP1.t == SCALAR );
	    assert ( OP2.t == SCALAR );
	    R.t = SCALAR;
	    R.s = OP1.s / OP2.s;
	    goto PRINT_RESULT;
	}
	if ( match ( "$=$%$" ) )
	{
	    assert ( OP1.t == SCALAR );
	    assert ( OP2.t == SCALAR );
	    R.t = SCALAR;
	    R.s = fmod ( OP1.s, OP2.s );
	    goto PRINT_RESULT;
	}
	if ( match ( "$=-$" ) )
	{
	    if ( OP1.t == SCALAR )
	        R.s = - OP1.s;
	    else
	        goto UNKNOWN_OPERATION;
	    R.t = OP1.t;
	    goto PRINT_RESULT;
	}
	if ( match ( "$=|$|" ) )
	{
	    assert ( OP1.t == SCALAR );
	    R.t = SCALAR;
	    R.s = fabs ( OP1.s );
	    goto PRINT_RESULT;
	}
	if ( match ( "$=sin$" ) )
	{
	    assert ( OP1.t == SCALAR );
	    R.t = SCALAR;
	    R.s = sin ( ( M_PI / 180 ) * OP1.s );
	    goto PRINT_RESULT;
	}
	if ( match ( "$=cos$" ) )
	{
	    assert ( OP1.t == SCALAR );
	    R.t = SCALAR;
	    R.s = cos ( ( M_PI / 180 ) * OP1.s );
	    goto PRINT_RESULT;
	}
	if ( match ( "$=tan$" ) )
	{
	    assert ( OP1.t == SCALAR );
	    R.t = SCALAR;
	    R.s = tan ( ( M_PI / 180 ) * OP1.s );
	    goto PRINT_RESULT;
	}
	if ( match ( "$=$<$:$" ) )
	{
	    assert ( OP3.t == SCALAR );
	    if ( OP1.t == SCALAR )
	    {
		assert ( OP2.t == SCALAR );
		R.t = BOOLEAN;
	        R.s = lt ( OP1.s, OP2.s, OP3.s );
	    }
	    else
	        goto UNKNOWN_OPERATION;
	    goto PRINT_RESULT;
	}
	if ( match ( "$=$<=$:$" ) )
	{
	    assert ( OP3.t == SCALAR );
	    if ( OP1.t == SCALAR )
	    {
		assert ( OP2.t == SCALAR );
		R.t = BOOLEAN;
	        R.s = le ( OP1.s, OP2.s, OP3.s );
	    }
	    else
	        goto UNKNOWN_OPERATION;
	    goto PRINT_RESULT;
	}
	if ( match ( "$=$==$:$" ) )
	{
	    assert ( OP3.t == SCALAR );
	    if ( OP1.t == SCALAR )
	    {
		assert ( OP2.t == SCALAR );
		R.t = BOOLEAN;
	        R.s = eq ( OP1.s, OP2.s, OP3.s );
	    }
	    else
	        goto UNKNOWN_OPERATION;
	    goto PRINT_RESULT;
	}


    UNKNOWN_OPERATION:
	{
	    cout << "ERROR: unrecognized operation"
	         << endl;
	    exit ( 1 );
	}

    PRINT_LINE:
	cout << line << endl;
	continue;

    PRINT_RESULT:
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
	    cout << "ERROR: bad result type "
	         << R.t << endl;
	    exit ( 1 );
	}
	cout << endl;
    }

    return 0;
}
