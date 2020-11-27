// Solution Two Dimensional Geometry Problem
//
// File:	vec-2d.cc
// Authors:	Bob Walton (walton@acm.org)
// Date:	Fri Nov 27 05:08:41 EST 2020
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
	assert ( isalpha ( p[0] ) );
	    // First line character is not variable.
	assert ( p[1] == '=' );
	    // Second line character is not '='.
	assert ( p[2] != 0 );
	    // Line has < 3 characters.
	
	var & result = vars[p[0]];

	p += 2;
	// PRINT_{RESULT/LINE} asserts * p == 0.
	if ( p[1] == 0 )
	{
	    // Rightside (of =) has 1 character.
	    //
	    if ( isdigit ( p[0] ) )
	    {
	        result.t = SCALAR;
		result.s = read_scalar ( p );
		goto PRINT_LINE;
	    }
	    else if ( isalpha ( p[0] ) )
	    {
	        result = vars[*p++];
		goto PRINT_RESULT;
	    }

	    // Put single operator rightsides here.

	    // End of single operator rightsides here.

	    else
	        goto UNRECOGNIZED_OPERATOR;
	}

	else if ( p[0] == '[' )
	{
	    result.t = LINEAR;
	    ++ p;
	    result.l.lx = read_vector ( p );
	    assert ( * p ++ == ',' );
	    result.l.ly = read_vector ( p );
	    assert ( * p ++ == ']' );
	    goto PRINT_LINE;
	}

	else if ( p[0] == '(' && p[1] == ')' )
	{
	    result.t = LIST;
	    result.first = NULL;
	    p += 2;
	    goto PRINT_LINE;
	}

	else if ( p[0] == '(' && p[1] == '(' )
	{
	    result.t = LIST;
	    element * last = NULL;
	    ++ p;
	    while ( true )
	    {
	        element & e = * new element;
		e.next = NULL;
		if ( last == NULL )
		    result.first = & e;
		else
		    last->next = & e;
		last = & e;
		e.v = read_vector ( p );
		if ( * p == ')' )
		{
		    ++ p;
		    goto PRINT_LINE;
		}
		assert ( * p ++ == ',' );
	    }
	}

	else if ( p[0] == '(' && ! isalpha ( p[1] ) )
	{
	    result.t = VECTOR;
	    result.v = read_vector ( p );
	    goto PRINT_LINE;
	}

	else if ( isdigit ( p[0]) || isdigit ( p[1] ) )
	{
	    result.t = SCALAR;
	    result.s = read_scalar ( p );
	    goto PRINT_LINE;
	}

	else if ( strcmp ( p, "true" ) == 0 )
	{
	    result.t = BOOLEAN;
	    result.b = TRUE;
	    p += 4;
	    goto PRINT_LINE;
	}

	else if ( strcmp ( p, "false" ) == 0 )
	{
	    result.t = BOOLEAN;
	    result.b = FALSE;
	    p += 5;
	    goto PRINT_LINE;
	}

	// Put function recognizers here.

	// End of function recognizers.

	else if ( isalpha ( p[0]) && isalpha ( p[1] ) )
	{
	    cout << "ERROR: unrecognized function"
	         << endl;
	    exit ( 1 );
	}

	// Put operator recognizers here.

	// End of operator recognizers.

    UNRECOGNIZED_OPERATOR:
	{
	    cout << "ERROR: unrecognized operator"
	         << endl;
	    exit ( 1 );
	}

    PRINT_RESULT:
	assert ( * p == 0 );
	cout << line << " = ";
	switch ( result.t )
	{
	case BOOLEAN:
	    cout << ( result.b == TRUE ? "true" : "false" );
	    break;
	case SCALAR:
	    cout << result.s; break;
	case VECTOR:
	    cout << result.v; break;
	case LINEAR:
	    cout << "[" << result.l.lx << ","
	                << result.l.ly << "]";
	    break;
	case LIST:
	    cout << "(";
	    {
	        element * ep = result.first;
		while ( ep != NULL )
		{
		    if ( ep != result.first )
		        cout << ",";
		    cout << ep->v;
		    ep = ep->next;
		}
	    }
	    cout << ")";
	    break;
	default:
	    cout << "ERROR: bad result type "
	         << result.t << endl;
	    exit ( 1 );
	}
	cout << endl;
	continue;

    PRINT_LINE:
	assert ( * p == 0 );
	cout << line << endl;
    }

    return 0;
}
