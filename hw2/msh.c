/*
 * msh.c
 * Parker Piedmont
 * CS 240 - Michael Wilder
 * 10/18/2019
 * 
 * msh is a basic shell. It supports all bash commands, including cd. However,
 * it does not support metacharacters. msh works by reading user input one
 * character at a time and building a list of the arguments as it reads the
 * characters. It then forks a child process and passes the argument list to
 * execvp, which is called by the child.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void build_arg( char c, char** arg, int* arglen );
void build_arglist( char* arg, int arglen, char*** arglist, int* numargs );

int main()
{
    char c = 0;

    // Stores the list of arguments typed by the user
    char** arglist = malloc( 1 * sizeof(char*) );
    if( arglist == NULL )
    {
        perror( "Unable to allocate memory for argument list" );
        exit( 1 );
    }

    // Stores the argument msh is currently processing
    char* arg = malloc( 1 * sizeof(char) );
    if( arg == NULL )
    {
        perror( "Unable to allocate memory for argument" );
        exit( 1 );
    }

    int numargs = 0;
    int arglen = 0;

    // Loop until user types "exit"
    while( 1 )
    {
        printf( ">: " );

        c = 0;

        arglist = malloc( 1 * sizeof(char*) );
        if( arglist == NULL )
        {
            perror( "Unable to allocate memory for argument list" );
            exit( 1 );
        }

        numargs = 0;

        while( c != '\n' )
        {
            // Reset arg at the beginning of each argument
            arg = realloc( arg, 1 * sizeof(char) );
            if( arg == NULL )
            {
                perror( "Unable to allocate memory for argument" );
                exit( 1 );
            }

            arglen = 0;

            c = getchar();

            // Build argument
            while( c != ' ' && c != '\t' && c != '\n' )
            {
                build_arg( c, &arg, &arglen );
                c = getchar();
            }

            build_arglist( arg, arglen, &arglist, &numargs );
        }

        // Exit if user types "exit"
        if( strcmp(arglist[0], "exit") == 0 )
        {
            int i;
            for( i = 0; i <= numargs; i++ )
            {
                free( arglist[i] );
            }

            free( arglist );
            free( arg );
            exit( 0 );
        }

        // Because the exec family doesn't support cd, it must be
        // manually implemented
        if( strcmp(arglist[0], "cd") == 0 )
        {
            int return_code = chdir( arglist[1] );
            if( return_code == -1 )
                perror( "Unable to change directory" );
        }

        else
        {   
            pid_t pid = fork();
            int status;

            if( pid == -1 )
            {
                perror( "Failed to fork child process" );
            }

            // Child process
            else if( pid == 0 )
            {
                int return_code = execvp( arglist[0], arglist );

                if( return_code != 0 )
                    perror( arglist[0] );

                exit( return_code );
            }

            // Parent process
            else
            {
                // Wait for child to complete execution
                // That way there aren't two children in a race condition
                wait( &status );
            }
        }
    }
}

/*
 * Concatenates the current character to the argument fragment the program
 * has built so far.
 * 
 * Parameters:
 *     c (char): The current character
 *     arg (char pointer): String storing the partial or complete argument
 *         the program has built up to now
 *     arglen (int pointer): Address of the int storing the length of arg
 * 
 * Does not explicitly return a value, but modifies arg and arglen. Since
 * they are passed by reference, the changes persist in the rest of the program.
 */
void build_arg( char c, char** arg, int* arglen )
{
    (*arglen)++;

    *arg = realloc( *arg, (*arglen + 1) * sizeof(char) );
    if( *arg == NULL )
    {
        perror( "Unable to allocate memory for argument list" );
        exit( 1 );
    }

    (*arg)[*arglen - 1] = c;
    (*arg)[*arglen] = 0;
}

/*
 * Concatenates the current argument to the list of arguments the program
 * has built so far.
 * 
 * Parameters:
 *     arg (char pointer): String storing the complete argument the program has built
 *     arglen (int): Length of arg
 *     arglist (string pointer): Array of strings, each of which is an argument
 *         typed by the user
 *     numargs (int pointer): Address of the int storing the length of arglist
 * 
 * Does not explicitly return a value, but modifies arglist and numargs. Since
 * they are passed by reference, the changes persist in the rest of the program.
 */
void build_arglist( char* arg, int arglen, char*** arglist, int* numargs )
{
    (*numargs)++;

    *arglist = realloc( *arglist, (*numargs + 1) * sizeof(char*) );
    if( *arglist == NULL )
    {
        perror( "Unable to allocate memory for argument list" );
        exit( 1 );
    }

    (*arglist)[*numargs - 1] = malloc( (arglen + 1) * sizeof(char) );
    if( (*arglist)[*numargs - 1] == NULL )
    {
        perror( "Unable to allocate memory for argument list" );
        exit( 1 );
    }

    strcpy( (*arglist)[*numargs - 1], arg );
    (*arglist)[*numargs] = NULL;
}