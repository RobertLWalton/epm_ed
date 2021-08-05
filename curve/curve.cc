// Some code moved from  vec2d problem.


// Circle, Line, and Point Calculator Functions
//
double sidec		            	//sidec crpd
    ( vec c, double r, vec p, double d )
{
    double diff = (p-c) * (p-c) - r*r;
    if ( eq ( diff, 0, d ) ) return 0;
    else if ( diff > 0 ) return -1;
    else return +1;
}
double distc ( vec c, double r, vec p )	//distc crp
{
    return fabs ( len ( p - c ) - r );
}
vec tangc ( vec c, double r, vec p )	// tangc crp
{
    vec pc = p - c;
    double L = len ( pc );
    assert ( L > r );
    vec b = c + ( ( r*r ) / ( L*L ) ) * pc;
    return b + ( r / ( L*L ) ) * rotate90 ( pc );
}
vec basec ( vec c, double r, vec p )	// basec crp
{
    vec pc = p - c;
    double L = len ( pc );
    assert ( L > r );
    return c + ( r*r / L*L ) * pc;
}
vec intersectc			     // intersectc crpq
    ( vec c, double r, vec p, vec q )
{
    vec t = closei ( p, q, c );
    vec tc = t - c;
    double tcsq = tc * tc;
    assert ( tcsq <= r * r );
    double ts = sqrt ( r * r - tcsq );
    vec u = unit ( q - p );
    return t + ts * u;
}
double distc				// distc crpq
    ( vec c, double r, vec p, vec q )
{
    double pc = len ( p - c );
    double qc = len ( q - c );
    if ( pc < r && qc < r )
        return min ( r - pc, r - pc );
    else if ( pc > r && qc > r )
	return distf ( p, q, c ) - r;
    else
        return 0;
}
