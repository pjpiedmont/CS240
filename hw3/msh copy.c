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

struct string
{
    char* str;
    int len;
};

struct stringlist
{
    char** list;
    int len;
};

struct stringlist_list
{
    char*** list;
    int len;
};

void append_to_arg( char c, struct string* arg );
void append_to_arglist( char* arg, int arglen, char*** arglist, int* numargs );
void append_to_cmdlist( char** arglist, int numargs, char**** cmdlist, int* numcmds );

int main()
{
    char c = 0;

    // Stores the list of commands typed by the user
    // char*** cmdlist = malloc( 1 * sizeof(char**) );
    struct stringlist_list cmdlist;
    cmdlist.list = malloc( 1 * sizeof(char**) );
    if( cmdlist.list == NULL )
    {
        perror( "Unable to allocate memory for command list" );
        exit( 1 );
    }

    cmdlist.len = 0;

    // Stores the list of arguments typed by the user
    // char** arglist = malloc( 1 * sizeof(char*) );
    struct stringlist arglist;
    arglist.list = malloc( 1 * sizeof(char*) );
    if( arglist.list == NULL )
    {
        perror( "Unable to allocate memory for argument list" );
        exit( 1 );
    }

    arglist.list[0] = NULL;
    arglist.len = 0;

    // Stores the argument msh is currently processing
    // char* arg = malloc( 1 * sizeof(char) );
    struct string arg;
    arg.str =  malloc( 1 * sizeof(char) );
    if( arg.str == NULL )
    {
        perror( "Unable to allocate memory for argument" );
        exit( 1 );
    }

    arg.str[0] = 0;
    arg.len = 0;

    // int numcmds = 0;
    // int numargs = 0;
    // int arglen = 0;

    // Loop until user types "exit"
    while( 1 )
    {
        printf( ">: " );

        c = 0;

        arglist.list = realloc( arglist.list, 1 * sizeof(char*) );
        if( arglist.list == NULL )
        {
            perror( "Unable to allocate memory for argument list" );
            exit( 1 );
        }

        arglist.list[0] = NULL;
        arglist.len = 0;

        while( c != '\n' )
        {
            // Reset arg at the beginning of each argument
            arg.str = realloc( arg.str, 1 * sizeof(char) );
            if( arg.str == NULL )
            {
                perror( "Unable to allocate memory for argument" );
                exit( 1 );
            }

            arg.str[0] = 0;
            arg.len = 0;

            c = getchar();

            // Build argument
            while( c != ' ' && c != '\t' && c != '\n' )
            {
                append_to_arg( c, &arg );
                c = getchar();
            }

            if( arg.str[0] != 0 )
                append_to_arglist( arg.str, arg.len, &arglist.list, &arglist.len );
        }

        // Exit if user types "exit"
        if( strcmp(arglist.list[0], "exit") == 0 )
        {
            int i;
            for( i = 0; i <= arglist.len; i++ )
            {
                free( arglist.list[i] );
            }

            free( arglist.list );
            free( arg.str );
            exit( 0 );
        }

        // Because the exec family doesn't support cd, it must be
        // manually implemented
        if( strcmp(arglist.list[0], "cd") == 0 )
        {
            int return_code = chdir( arglist.list[1] );
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
                int return_code = execvp( arglist.list[0], arglist.list );

                if( return_code != 0 )
                    perror( arglist.list[0] );

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
void append_to_arg( char c, struct string* arg )
{
    (*arg).len++;

    (*arg).str = realloc( (*arg).str, ((*arg).len + 1) * sizeof(char) );
    if( (*arg).str == NULL )
    {
        perror( "Unable to allocate memory for argument list" );
        exit( 1 );
    }

    (*arg).str[(*arg).len - 1] = c;
    (*arg).str[(*arg).len] = 0;
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
void append_to_arglist( char* arg, int arglen, char*** arglist, int* numargs )
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

void append_to_cmdlist( char** arglist, int numargs, char**** cmdlist, int* numcmds )
{
    (*numcmds)++;

    (*cmdlist) = realloc( *cmdlist, (*numcmds + 1) * sizeof(char**) );

    memcpy( (*cmdlist)[*numcmds - 1], arglist, sizeof(arglist) );
}

/*
void execute( char*** cmdlist )
{
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
*/