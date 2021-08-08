// Solution to the Make Passwords Problem
//
// File:     makepass.cc
// Authors:  Bob Walton <walton@seas.harvard.edu>
// Date:     Thu Oct  8 12:28:05 EDT 2020
//
// The authors have placed this program in the public
// domain; they make no warranty and accept no liability
// for this program.

#include <iostream>
#include <cassert>
using std::cin;
using std::cout;
using std::endl;

# define FOR0(i,n) for ( int i = 0; i < (n); ++ i )

bool debug = false;
# define dout if ( debug ) cout

const long MAX_NUMBER = 999999999999L;

const char * formats[6] = {
    "WWWDWWWDDD",
    "WWWDDWWWDD",
    "WWWDDDWWWD",
    "DWWWDDDWWW",
    "DDWWWDDWWW",
    "DDDWWWDWWW"
};

const int dict_size = 21 * 5 * 21;
char dict[dict_size][4];

void init_dict ( void )
{
    const char * vowels = "aeiou";
    const char * consonants = "bcdfghjklmnpqrstvwxyz";

    int d = 0;
    FOR0 ( i, 21 )
    FOR0 ( j, 5 )
    FOR0 ( k, 21 )
    {
        dict[d][0] = consonants[i];
        dict[d][1] = vowels[j];
        dict[d][2] = consonants[k];
        dict[d][3] = 0;
	++ d;
    }
    assert ( d == dict_size );
}

int main ( int argc, char * argv[] )
{
    debug = (argc > 1 );

    dout << "Numbers are considered modulo "
         << 6L * ( 21 * 5 * 21 ) * ( 21 * 5 * 21 )
	       * 10000 << endl;

    init_dict();

    if ( debug ) FOR0 ( i, dict_size )
        cout << dict[i] << endl;

    long n;
    while ( cin >> n )
    {
	assert ( cin.get() == '\n' );
	assert ( n >= 0 );
	assert ( n <= MAX_NUMBER );

	int f = n % 6;
	n /= 6;

	const char * p = formats[f];
	while ( * p )
	{
	    char c = * p ++;
	    if ( c == 'D' )
	    {
		int d = n % 10;
		n /= 10;
		char digit = '0' + d;
		cout << digit;
	    }
	    else
	    {
		int d = n % dict_size;
		n /= dict_size;
		cout << dict[d];
		p += 2;
	    }
	}
	cout << endl;
    }
    
    return 0;
}
