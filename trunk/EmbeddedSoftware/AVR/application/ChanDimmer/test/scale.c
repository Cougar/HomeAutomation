#include <stdio.h>
#include <stdlib.h>

double scale( long val, long k, long m );
long iscale( long val, long k, long m, long fact );

int main( int argc, char *argv[] ) {
    long k, m;

    if( argc != 3 ) {
        fprintf( stderr, "Usage: %s <k> <m>\n", argv[0] );
        return 1;
    }

    k=atoi(argv[1]);
    m=atoi(argv[2]);

    printf( "k=%d m=%d\n", k, m );

    printf( "scale(    0)=%f\n", scale(     0, k, m ) );
    printf( "scale(65535)=%f\n", scale( 65535, k, m ) );
    printf( "scale(1)-scale(0)=%f\n", scale( 1, k, m ) - scale( 0, k, m ) );

    printf( "iscale(0,100)=%d", iscale( 0, k, m, 100 ) );

    return 0;
}

double scale( long val, long k, long m ) {
    return (double)m - 32768.0 + ((double)k*(double)val)/256.0;
}

long iscale( long val, long k, long m, long fact ) {
    return m*fact - (fact<<15) + ((k*val*fact)>>8);
}
