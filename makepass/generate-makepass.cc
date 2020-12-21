// Make Passwords Problem Generate Program
//
// File:	generate-makepass.cc
// Authors:	Bob Walton (walton@deas.harvard.edu)
// Date:	Thu Oct  8 03:02:04 EDT 2020
//
// The authors have placed this program in the public
// domain; they make no warranty and accept no liability
// for this program.

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <cassert>
using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::istringstream;
using std::ws;

#define FOR0(i,n) for ( int i = 0; i < (n); ++ i )
#define FOR1(i,n) for ( int i = 1; i <= (n); ++ i )

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

// Random Number Generator
// ------ ------ ---------

// We embed the random number generator so it cannot
// change on us.  This is the same as lrand48 in 2016.
// Note that for this random number generator the
// low order k bits produced depend only on the low
// order k bits of the seed.  Thus the generator is
// only good at generating floating point numbers in
// the range 0 .. 1.  To get random integers, use
// random ( n ) or random ( low, high ) below.
//
# include <cmath>
# define random RANDOM
# define srandom SRANDOM
    // To avoid conflict with libraries.
unsigned long long last_random_number;
const unsigned long long MAX_RANDOM_NUMBER =
	( 1ull << 32 ) - 1;
void srandom ( unsigned long long seed )
{
    seed &= MAX_RANDOM_NUMBER;
    last_random_number = ( seed << 16 ) + 0x330E;
}
// Return floating point random number in range [0 .. 1)
//
inline double drandom ( void )
{
    last_random_number =
        0x5DEECE66Dull * last_random_number + 0xB;
    unsigned long long v =
          ( last_random_number >> 16 )
	& MAX_RANDOM_NUMBER;
    return (double) v / (MAX_RANDOM_NUMBER + 1 );
}
// Return a random number in the range 0 .. n - 1.
//
inline unsigned long random ( unsigned long n )
{
    return (unsigned long) floor ( drandom() * n );
}
// Return a random number in the range [first,last].
//
inline long random ( long first, long last )
{
    assert ( first <= last );
    return first + random ( last - first + 1 );
}

// Shuffle vector v of n elements randomly.
//
template <typename T> inline void shuffle
	( T * v, int n )
{
    for ( int i = 0; i < n; ++ i )
    {
        int j = random ( n - i );
	swap ( v[i], v[i+j] );
    }
}

char documentation [] =
"generate-makepass [-doc]\n"
"\n"
"    Copies standard input to standard output,\n"
"    removing lines the begin with `!!'.\n"
"\n"
"    Lines that begin with `!!##' are considered to\n"
"    be comment lines.\n"
"\n"
"    A non-comment line of the form:\n"
"\n"
"        !!R seed k min max\n"
"\n"
"    denotes k lines each containing a random number\n"
"    in the range [min,max].\n"
"\n"
"    A non-comment line of the form:\n"
"\n"
"        !!L k a b\n"
"\n"
"    denotes k lines each containing the number a*i+b\n"
"    for 0 <= i < k.  a < 0 is allowed.\n"
"\n"
"    Other lines beginning with `!!' are considered\n"
"    to be bad comment lines and cause a warning\n"
"    message to be output on the standard error.\n"
;

const int MAX_K = 10000;
    // Greater than 21 * 5 * 21;
const long MAX_NUMBER = 999999999999L;
    // 12 digits.

// Input Data
//
int k;
long min, max, a, b;
unsigned long seed;

void R ( string line )
{
    char op[100];

    istringstream in ( line );
    in >> op >> seed >> k >> min >> max;
    if ( in.fail() )
	  error ( "badly formatted"
	          " or too few parameters" );
    assert ( strcmp ( op, "!!R" ) == 0 );
    if ( seed < 1e8 || seed >= 1e9 )
	  error ( "seed out of range" );
    if ( k < 1 || k > MAX_K )
	  error ( "k out of range" );
    if ( min < 0 || min > MAX_NUMBER )
	  error ( "min out of range" );
    if ( max < min || max > MAX_NUMBER )
	  error ( "max out of range" );
    if ( ! in.eof() )
	  error ( "extra stuff at end of line" );

    srandom ( seed );


    long diff = max + 1 - min;
    FOR0 ( i, k )
    {
        // Generate an random long of 60 bits.
	//
	unsigned long limit = 1 << 30;
	unsigned long low = random ( limit );
	unsigned long high = random ( limit );
	long r = low;
	r += ( (long) high ) << 30;

	// Generate output.
	//
	r %= diff;
	cout << min + r << endl;
    }
}

void L ( string line )
{
    char op[100];

    istringstream in ( line );
    in >> op >> k >> a >> b;
    if ( in.fail() )
	  error ( "badly formatted"
	          " or too few parameters" );
    assert ( strcmp ( op, "!!L" ) == 0 );
    if ( k < 1 || k > MAX_K )
	  error ( "k out of range" );
    if ( b < 0 || b > MAX_NUMBER )
	  error ( "b out of range" );
    long last = a + b * (k - 1 );
    if ( last < 0 || last > MAX_NUMBER )
	  error ( "a*(k-1) + b out of range" );
    if ( ! in.eof() )
	  error ( "extra stuff at end of line" );

    FOR0 ( i, k )
        cout << a * i + b << endl;
}


// Main program.
//
int bad_comments = 0;    // Number of bad comment lines.
int bad_first;           // First bad comment line.
string line;
//
int main ( int argc, char ** argv )
{
    assert ( MAX_K >= 21 * 5 * 21 );

    if (    argc > 1
         && strncmp ( argv[1], "-deb", 4 ) == 0 )
    {
        debug = true;
	-- argc, ++ argv;
    }
    if ( argc > 1 )
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
	if ( strncmp ( p, "!!##", 4 ) == 0 )
	    continue;
	else if ( strncmp ( p, "!!R", 3 ) == 0
	          && isspace ( p[3] ) )
	{
	    R ( line );
	    continue;
	}
	else if ( strncmp ( p, "!!L", 3 ) == 0
	          && isspace ( p[3] ) )
	{
	    L ( line );
	    continue;
	}
	else if ( strncmp ( p, "!!", 2 ) == 0 )
	{
	    if ( bad_comments ++ == 0 )
	        bad_first = line_number;
	    continue;
	}
	else
	    cout << line << endl;
    }

    if ( bad_comments > 0 )
        cerr << "WARNING: there were " << bad_comments
	     << " bad comment lines, the first being"
	     << " line " << bad_first << endl;

    
    return 0;
}
