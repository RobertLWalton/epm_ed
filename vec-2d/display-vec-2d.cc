// Two Dimensional Geometry Display Program
//
// File:	display-vec-2d.cc
// Authors:	Bob Walton (walton@acm.org)
// Date:	Tue Dec 15 12:15:10 EST 2020
//
// The authors have placed this program in the public
// domain; they make no warranty and accept no liability
// for this program.

// C++ Data Structures
//

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdarg>
using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::ostream;
using std::string;

#include <sstream>
#include <iomanip>
using std::istringstream;
using std::ws;

bool debug = false;
# define dout if ( debug ) cout

int line_number = 0;
void error ( const char * format... )
{
    va_list args;
    va_start ( args, format );
    cerr << "ERROR: line: " << line_number << ": ";
    vfprintf ( stderr, format, args );
    va_end ( args );
    cerr << endl;
    exit ( 1 );
}

char documentation [] =
"display-vec-2d [-doc] [title]\n"
"\n"
"    Copies standard input to standard output,\n"
"    processing lines read.  Lines beginning with\n"
"    `!!' are ignored.  Lines beginning with `#!'\n"
"    are processed as display commands, and other\n"
"    lines beginning with `#' are ignored.  Other\n"
"    lines must have one of the forms:\n"
"\n"
"            variable = value\n"
"    or\n"
"            variable = ... = value\n"
"\n"
"    and are used to set the current value of the\n"
"    variable, which may then be used in display com-\n"
"    mands.\n"
"\n"
"    The output is a sequence of epm_display"
				  " commands.\n"
"\n"
"    The title, if given, causes physical pages to\n"
"    have a header consisting of the left-adjusted\n"
"    title and a right adjusted page number.\n"
;

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

var vars[128];
    // Implicitly initialized to
    // vars[...].t == 0 == NONE,

// Read specific character.
//
void read_char ( char * & p, char c )
{
    if ( * p ++ = c ) return;
    if ( c != 0 )
	error ( "expected `%c' and found `%c'",
	        c, p[-1] );
    else
	error ( "expected line end and found `%c'",
	        p[-1] );
}

// Read and return scalar.
//
double read_scalar ( char * & p )
{
    char * endp;
    double r = strtod ( p, & endp );
    if ( p >= endp )
        error ( "expected scalar, found %s", p );
    p = endp;
    return r;
}

// Read and return vector.
//
vec read_vector ( char * & p )
{
    vec r;
    read_char ( p, '(' );
    read_char ( p, ',' );
    r.y = read_scalar ( p );
    read_char ( p, ')' );
    return r;
}

// Print vector.
//
ostream & operator << ( ostream & s, vec v )
{
    return s << "(" << v.x << "," << v.y << ")";
}

// Read value from line.
//
void read_value ( const char * line )
{
    size_t s = strlen ( line );
    char squashed[s+1];
    char * p = squashed;
    while ( * line )
    {
        if ( isspace ( * line ) ) ++ line;
	else * p ++ = * line ++;
    }
    * p = 0;

    if ( ! isalpha ( squashed[0] ) )
        error ( "expected variable and found `%c'",
	        squashed[0] );
    if ( squashed[1] != '=' )
        error ( "expected `=' and found `%c'",
	        squashed[1] );
    p = strrchr ( squashed, '=' );
    ++ p;

    var & R = vars[squashed[0]];

    if ( p[0] == '[' )
    {
	R.t = LINEAR;
	++ p;
	R.l.lx = read_vector ( p );
	read_char ( p, ',' );
	R.l.ly = read_vector ( p );
	read_char ( p, ']' );
	read_char ( p, 0 );
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
		read_char ( p, 0 );
		break;
	    }
	    read_char ( p, ',' );
	}
    }

    else if ( p[0] == '(' )
    {
	R.t = VECTOR;
	R.v = read_vector ( p );
	read_char ( p, 0 );
    }

    else if ( strcmp ( p, "()" ) )
    {
    	R.t = LIST;
	R.first = NULL;
    }

    else if ( strcmp ( p, "true" ) )
    {
    	R.t = BOOLEAN;
	R.b = true;
    }

    else if ( strcmp ( p, "false" ) )
    {
    	R.t = BOOLEAN;
	R.b = false;
    }

    else
    {
	R.t = SCALAR;
	R.s = read_scalar ( p );
	read_char ( p, 0 );
    }
}

// Execute display command.
//
void execute ( const char * p )
{
}

// Main program.
//
string line;
//
int main ( int argc, char ** argv )
{
    if (    argc > 1
         && strncmp ( argv[1], "-deb", 4 ) == 0 )
    {
        debug = true;
	-- argc, ++ argv;
    }
    if (    argc > 1
         && strncmp ( argv[1], "-doc", 4 ) == 0 )
    {
	FILE * out = popen ( "less -F", "w" );
	fputs ( documentation, out );
	pclose ( out );
	return 0;
    }

    while ( getline ( cin, line ) )
    {
        ++ line_number;
	const char * p = line.c_str();
	if ( strncmp ( p, "!!", 2 ) == 0 )
	    continue;
	else if ( strncmp ( p, "#!", 2 ) == 0 )
	    execute ( p );
	else if ( * p == '#' )
	    continue;
	else
	    read_value ( p );
    }

    return 0;
}
