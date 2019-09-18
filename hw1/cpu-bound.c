#include <time.h>

int main()
{
    time_t start, now;
    double elapsed;

    start = time( NULL );

    int number = 0;
    while( 1 )
    {
        now = time( NULL );
        elapsed = difftime( now, start );
        if( elapsed >= 300 )
            return 0;
        
        number++;
        if( number > 1000 )
            number = 0;
    }
}