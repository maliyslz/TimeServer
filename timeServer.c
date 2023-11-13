#include <stdio.h>
#include <string.h> // for strlen
#include <sys/socket.h>
#include <arpa/inet.h> // for inet_addr
#include <unistd.h>    // for write
#include <time.h>
#include <stdlib.h> //for clean input

#define numb_of_cmd 6                  // number of commands
#define wrongInput "INCORRECT REQUEST" // The text to be printed on the screen when wrong command is given
#define closed "GOOD BYE\n"            // The text to be printed on the screen when the shutdown command is given
#define PORT_NUMBER 60085              // local host connection port

int compare(char mychar[numb_of_cmd][20], char input[]) // to compare compliance with the commands given to us
{
    for (int i = 0; i < numb_of_cmd; i++)
    {
        if (strcmp(input, mychar[i]) == 0) // comparison function
        {
            return i;
        }
    }
    return -1;
}

int main(int argc, char *argv[])
{
    int socket_desc, new_socket, c;
    struct sockaddr_in server, client;
    char *message;
    char rtrn_time[100]; // time message to return
    time_t timeobj;
    struct tm *time_detail;
    char command_input[128];
    char commands[6][20] = {"GET_TIME", "GET_DATE", "GET_TIME_ZONE", "GET_DAY_OF_WEEK", "GET_TIME_DATE", "CLOSE_SERVER"};
    char shortcut[5][10] = {"%X", "%x", "%z", "%A", "%X, %x"}; // printing the time we receive according to the request command

    // Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1)
    {
        puts("Could not create socket");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT_NUMBER);

    // Bind
    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        puts("Bind failed");
        return 1;
    }
    puts("Socket is binded");

    // Listen
    listen(socket_desc, 3);

    // Accept and incoming connection
    puts("Waiting for incoming connections...");

    c = sizeof(struct sockaddr_in);

    while (new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c))
    {
        puts("\nConnection accepted");
        message = "Connection Started\n";
        write(new_socket, message, strlen(message)); // write data to a output stream.
        while (1)
        {
            message = "\n\nClient input: \n";
            write(new_socket, message, strlen(message));

            read(new_socket, command_input, sizeof(command_input)); // receiving input

            // Cleaning it because it also gets something like /n/r at the end of the input
            int input_size = strlen(command_input);
            char self_text[input_size - 2 + 1];
            strncpy(self_text, command_input, input_size - 2);
            self_text[input_size - 2] = '\0'; // our cleared input

            timeobj = time(NULL);
            time_detail = localtime(&timeobj); // get current time

            int comp_result = compare(commands, self_text); // return value from comparison
            switch (comp_result)                            // the return value tells us which command was entered
            {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
                strftime(rtrn_time, 100, shortcut[comp_result], time_detail);
                write(new_socket, rtrn_time, strlen(rtrn_time));
                break;
            case 5:
                write(new_socket, closed, strlen(closed));
                close(new_socket);
                close(socket_desc);
                break;
            default:
                write(new_socket, wrongInput, strlen(wrongInput));
            }

            memset(command_input, 0, sizeof(command_input)); // reset a block of memory to all zeros..
            memset(rtrn_time, 0, sizeof(rtrn_time));
        }
    }
    return 0;
}
