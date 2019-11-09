#include <time.h>
#include <stdio.h>

int main()
{
    time_t start, now;

    FILE* fp;

    start = time( NULL );

    while( 1 )
    {
        now = time( NULL );
        if( difftime(now, start) >= 300 )
            return 0;
        
        fp = fopen( "test.txt", "w+" );
        fputc( 'F', fp );
        fclose( fp );
    }
}