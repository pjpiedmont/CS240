#include <time.h>
#include <stdio.h>

int main()
{
    time_t start, now;
    double elapsed;

    FILE* fp;
    // char first10[11];

    start = time( NULL );

    while( 1 )
    {
        now = time( NULL );
        elapsed = difftime( now, start );
        if( elapsed >= 300 )
            return 0;
        
        fp = fopen( "test.txt", "w+" );
        // fgets( fp, 11, fp );
        fputc( 'F', fp );
        fclose( fp );
    }
}