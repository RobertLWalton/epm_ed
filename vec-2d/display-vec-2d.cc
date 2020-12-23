// Two Dimensional Geometry Display Program
//
// File:	display-vec-2d.cc
// Authors:	Bob Walton (walton@acm.org)
// Date:	Wed Dec 23 17:29:03 EST 2020
//
// The authors have placed this program in the public
// domain; they make no warranty and accept no liability
// for this program.

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdarg>
#include <cmath>
using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::ostream;
using std::string;
using std::fpclassify;

#include <sstream>
#include <iomanip>
using std::istringstream;
using std::ws;

bool debug = false;
# define dout if ( debug ) cout

string line;
int line_number = 0;
struct line_error {} thrown_line_error;
void error ( const char * format... )
{
    va_list args;
    va_start ( args, format );
    cerr << "ERROR: line: " << line_number << ": ";
    vfprintf ( stderr, format, args );
    va_end ( args );
    cerr << endl;
    cerr << "    " << line << endl;
    throw thrown_line_error;
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
    if ( * p ++ == c ) return;
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
    r.x = read_scalar ( p );
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

// End page if necessary.
//
bool page_non_empty = false;
bool page_has_head = false;
void end_page ( void )
{
    if ( ! page_non_empty ) return;
    if ( page_has_head )
        cout << "head" << endl
	     << "space 10pt" << endl;
    cout << "*" << endl;
    page_non_empty = false;
    page_has_head = false;
}

// Output layout.
//
const char * title = NULL;
bool layout_output = false;
void output_layout ( int R, int C )
{
    end_page();

    layout_output = true;
    cout << "layout " << R << " " << C << endl;
    cout << "margins 0.25in" << endl;
    cout << "scale 1" << endl;
    cout << "stroke solid 0pt s" << endl;
    cout << "stroke line" << endl;
    cout << "font text 10pt" << endl;
    if ( title != NULL )
        cout << "text large-bold " << title
	     << "\\\\page ###" << endl
	     << "space 10pt" << endl;
    cout << "*" << endl;
}

void skip_space ( const char * & p )
{
    if ( * p == 0 ) return;
    if ( ! isspace ( * p ++ ) )
        error ( "missing whitespace in"
	        " display command" );
    while ( isspace ( * p ) ) ++ p;
}

// Get the operands, color, and options of a command.
// Call output_layout if no commands output yet.
//
var * V[4];
# define OP1 (*V[0])    // First Operand
# define OP2 (*V[1])    // Second Operand
# define OP3 (*V[2])    // Third Operand
# define OP4 (*V[4])    // Fourth Operand
int number_of_operands;
char color[100];
char options[100];
    // Point size is stored here as an `option'.
char text[4097];
const char * colors[5] = {
    "red", "blue", "brown", "black", NULL };


// Get operands into OP1, OP2, ....  Set number_of_
// operands to number gotten, and check that this
// is in the range [min_operands,max_operands].
//
// Also get color, options, and text.
//
// Also executes output_layout if no commands have
// been output so far, and ends by setting page
// non-empty.
//
void get_operands
	( const char * & p,
	  int min_operands, int max_operands,
	  bool text_allowed = false )
{
    if ( ! layout_output ) output_layout ( 1, 1 );

    skip_space ( p );
    number_of_operands = 0;
    while ( isalpha ( * p ) )
    {
	if ( number_of_operands >= max_operands )
	    error ( "too many operands in display"
	            " command" );
        V[number_of_operands++] = & vars[*p++];
    }
    if ( number_of_operands < min_operands )
	error ( "too few operands in display command" );
    skip_space ( p );
    color[0] = 0;
    for ( const char ** q = colors; * q != NULL; ++ q )
    {
        size_t s = strlen ( * q );
	if ( strncmp ( p, * q, s ) != 0 )
	    continue;
	p += s;
	strcpy ( color, * q );
	break;
    }
    if ( color[0] == 0 )
        strcpy ( color, "black" );
    else
	skip_space ( p );
    char * q;
    for ( q = options;
          * p && ! isspace ( *p ); )
    {
	if ( q - options >= sizeof ( options ) - 1 )
	    error ( "bad options in display command" );
	* q ++ = * p ++;
    }
    * q = 0;
    skip_space ( p );
    if ( strlen ( p ) >= sizeof ( text ) - 1 )
	error ( "too long text in display command" );
    strcpy ( text, p );

    if ( ! text_allowed && text[0] != 0 )
        error ( "extra stuff after options in display"
	        " command" );

    page_non_empty = true;
}

// Check that all option characters are in `legal'
// and no options character appears twice.
//
void check_legal ( const char * legal )
{
    const char * p = options;
    while ( * p )
    {
        if ( strchr ( legal, * p ) == NULL )
	    error ( "bad option character `%c'", * p );
        if ( strchr ( options, * p ) != p )
	    error ( "duplicated option character `%c'",
	            * p );
	++ p;
    }
}

// Check that at most one of the conflict characters
// appears in options.
//
void check_conflict ( const char * conflict )
{
    const char * p = options;
    char c = 0;
    while ( * p )
    {
        if ( strchr ( conflict, * p ) != NULL )
	{
	    if ( c != 0 )
		error ( "option characters `%c' and"
		        " `%c' conflict", c, * p );
	    else
	        c = * p;
	}
	++ p;
    }
}

void check_scalar ( double x )
{
    int c = fpclassify ( x );
    if ( c == FP_NAN )
        error ( "scalar operand is NAN" );
    if ( c == FP_INFINITE )
        error ( "scalar operand is infinite" );
}

void check_vector ( vec v )
{
    int c = fpclassify ( v.x );
    if ( c == FP_NAN )
        error ( "vector (in) operand has NAN"
	        " X-coordinate" );
    if ( c == FP_INFINITE )
        error ( "vector (in) operand has infinite"
	        " X-coordinate" );
    c = fpclassify ( v.y );
    if ( c == FP_NAN )
        error ( "vector (in) operand has NAN"
	        " Y-coordinate" );
    if ( c == FP_INFINITE )
        error ( "vector (in) operand has infinite"
	        " Y-coordinate" );
}


// Execute display command.
//
void execute ( const char * p )
{
    p += 2;
    while ( isspace ( * p ) ) ++ p;

    if ( strncmp ( p, "layout", 6 ) == 0 )
    {
	p += 6;
	skip_space ( p );
	int R = * p ++ - '0';
	if ( R < 1 || R > 5 )
	    error ( "R not in [1,5] in"
	            " layout command" );
	skip_space ( p );
	int C = * p ++ - '0';
	if ( C < 1 || C > 3 )
	    error ( "C not in [1,3] in"
	            " layout command" );
	skip_space ( p );
	if ( * p != 0 )
	    error ( "extra stuff after numbers in"
	            " layout command" );
	output_layout ( R, C );
    }
    else if ( strncmp ( p, "newpage", 7 ) == 0 )
    {
	p += 7;
	skip_space ( p );
	end_page();
	if ( * p != 0 )
	{
	    cout << "head" << endl
	         << "text font " << p << endl
		 << "level" << endl;
	    page_non_empty = true;
	    page_has_head = true;
	}
    }
    else if ( strncmp ( p, "header", 6 ) == 0 )
    {
	p += 6;
	skip_space ( p );
	if ( * p != 0 )
	{
	    cout << "head" << endl
	         << "text font " << p << endl
		 << "level" << endl;
	    page_non_empty = true;
	    page_has_head = true;
	}
    }
    else if ( strncmp ( p, "point", 5 ) == 0 )
    {
	p += 5;
	get_operands ( p, 1, 1 );
	if ( options[0] == 0 )
	    strcpy ( options, "2" );
	char * endp;
	long size = strtol ( options, & endp, 10 );
	if ( * endp != 0 || size < 1 || size > 18 )
	    error ( "bad size in point command" );
	if ( OP1.t == VECTOR )
	{
	    check_vector ( OP1.v );
	    cout << "arc solid " << color
		 << " " << OP1.v.x << " " << OP1.v.y
		 << " " << size << "pt" << endl;
	}
	else if ( OP1.t == LIST )
	{
	    for ( element * e = OP1.first;
	          e != NULL; e = e->next )
	    {
		check_vector ( e->v );
		cout << "arc solid " << color
		     << " " << e->v.x << " " << e->v.y
		     << " " << size << "pt" << endl;
	    }
	}
	else
	    error ( "operand is not a point or list of"
	            " points" );
    }
    else if ( strncmp ( p, "line", 4 ) == 0 )
    {
	p += 4;
	get_operands ( p, 1, 2 );
	check_legal ( ".-cmesdhv" );
	check_conflict ( ".-" );
	check_conflict ( "sdhv" );

	if ( number_of_operands == 2
	     &&
	     ( OP1.t != VECTOR
	       ||
	       OP2.t != VECTOR ) )
	    error ( "operand is not a point" );
	else if ( number_of_operands == 1
	          &&
	          OP1.t != LIST )
	    error ( "operand is not a list of points" );
	else if ( number_of_operands == 2 )
	{
	    check_vector ( OP1.v );
	    cout << "start line " << color
	         << " " << options
		 << " " << OP1.v.x << " " << OP1.v.y
		 << endl
		 << "line"
		 << " " << OP2.v.x << " " << OP2.v.y
		 << endl
		 << "end" << endl;
	}
	else if ( OP1.first != NULL
	          &&
		  OP1.first->next != NULL )
	{
	    element * e = OP1.first;
	    check_vector ( e->v );
	    cout << "start line " << color
		 << " " << options
		 << " " << e->v.x << " " << e->v.y
		 << endl;
	    for ( e = e->next; e != NULL;
			       e = e->next )
	    {
		check_vector ( e->v );
		cout << "line"
		     << " " << e->v.x
		     << " " << e->v.y << endl;
	    }
	    cout << "end" << endl;
	}
    }
    else if ( strncmp ( p, "rectangle", 9 ) == 0 )
    {
	p += 9;
	get_operands ( p, 2, 2 );
	if ( OP1.t != VECTOR )
	    error ( "first operand is not vector" );
	check_vector ( OP1.v );
	if ( OP2.t != VECTOR )
	    error ( "second operand is not vector" );
	check_vector ( OP2.v );
	vec c = { ( OP1.v.x + OP2.v.x ) / 2,
	          ( OP1.v.y + OP2.v.y ) / 2 };
	double width = fabs ( OP1.v.x - OP2.v.x );
	double height = fabs ( OP1.v.y - OP2.v.y );
	cout << "rectangle line " << color
	     << " " << options
	     << " " << c.x << " " << c.y
	     << endl;
    }
    else if ( strncmp ( p, "circle", 6 ) == 0 )
    {
	p += 6;
	get_operands ( p, 2, 2 );
	if ( OP1.t != VECTOR )
	    error ( "first operand is not vector" );
	check_vector ( OP1.v );
	if ( OP2.t != SCALAR )
	    error ( "second operand is not scalar" );
	check_scalar ( OP2.s );
	if ( OP2.s <= 0 )
	    error ( "radius is <= 0" );
	cout << "circle line " << color
	     << " " << options
	     << " " << OP1.v.x << " " << OP1.v.y
	     << " " << OP2.s
	     << endl;
    }
    else if ( strncmp ( p, "ellipse", 7 ) == 0 )
    {
	p += 7;
	get_operands ( p, 3, 3 );
	if ( OP1.t != VECTOR )
	    error ( "first operand is not vector" );
	check_vector ( OP1.v );
	if ( OP2.t != VECTOR )
	    error ( "second operand is not vector" );
	check_vector ( OP2.v );
	if ( OP3.t != SCALAR )
	    error ( "third operand is not scalar" );
	check_scalar ( OP3.s );
	if ( OP2.v.x <= 0 )
	    error ( "x-radius is <= 0" );
	if ( OP2.v.y <= 0 )
	    error ( "y-radius is <= 0" );
	cout << "arc line " << color
	     << " " << options
	     << " " << OP1.v.x << " " << OP1.v.y
	     << " " << OP2.v.x << " " << OP2.v.y
	     << " " << OP3.s
	     << endl;
    }
    else if ( strncmp ( p, "text", 4 ) == 0 )
    {
	p += 4;
	get_operands ( p, 1, 2, true );
	if ( strcmp ( options, "-" ) == 0 )
	    options[0] = 0;
	else
	{
	    check_legal ( "tblrxco" );
	    check_conflict ( "tb" );
	    check_conflict ( "lr" );
	    check_conflict ( "xc" );
	    if ( strchr ( options, 'o' ) != NULL
	         &&
		 strchr ( options, 'x' ) == NULL
	         &&
		 strchr ( options, 'c' ) == NULL )
	        error ( "options contain `o' but not"
		        " `x' or `c'" );
	}
	if ( text[0] == 0 )
	    error ( "no text... in text command" );

	if ( OP1.t != VECTOR )
	    error ( "first operand is not vector" );
	check_vector ( OP1.v );
	vec p = OP1.v;
	if ( number_of_operands == 2 )
	{
	    if ( OP2.t != VECTOR )
		error ( "second operand is not"
		        " vector" );
	    check_vector ( OP2.v );
	    vec r = { ( OP1.v.x + OP2.v.x ) / 2,
	              ( OP1.v.y + OP2.v.y ) / 2 };
	    p = r;
	}

	cout << "level 100" << endl
	     << "text text " << color
	     << " " << options
	     << " " << p.x << " " << p.y
	     << " " << text << endl
	     << "level" << endl;
    }
    else
        error ( "undefined command name" );
}

// Main program.
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
    if ( argc > 1 ) title = argv[1];

    while ( getline ( cin, line ) )
    {
        try {
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
	catch ( line_error e ){};
    }
    end_page();

    return 0;
}
