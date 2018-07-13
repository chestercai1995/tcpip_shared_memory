#define BUF_SIZE 1
#define CLADDR_LEN 100

/* For internal debugging use.
 * Prints out the error message and then quit the program.
 * It does not return to the original functionl
 */
void error(char* msg);

/* Initialize the server. need to input the port number.
 * Should probably make sure the port isn't use by anything else
 * This function will wait indefinately for a client to connect.
 * The server must call init_server() before a client can call init_client().
 * See init_client for more information
 *
 * input: int portno -- port number to use
 * output: int sockfd  -- socket number to be used later in transmit and receive
 */
int init_server(int portno);

/* Initialize the client. Need to input the server IP address and port number
 * The port number need to match the port number used in init_server()
 * If this function is called before init_server is called, it will prints out
 * an error message and exits.
 *
 * input: char* serveraddr -- string of the server IP address
 *        int portno -- port number to use
 * output: int sockfd -- socket number to be used later in transmit and receive
 */
int init_client(char* serveraddr, int portno);

/* transmit the File pointed by pointer in. 
 * File needs to be opened with the option rb intead of just r to read the file in binary
 * This function also deso not close the file. 
 * The caller is responsible for that unlike receive
 * 
 * input: FILE* in -- the file you want to transmit
 *        int sockfd -- the socket number returned from initialization
 */
void transmit(FILE* in, int sockfd);

/* Receive the File named with the given string. 
 * Note that the input is a char*(string), not a file pointer.
 * This function will open and close the file.
 *
 * input: char* out -- name of the output file(string)
 *        int sockfd -- socket number returned from initialization
 */
void receive(char* out, int sockfd);

/* Closes the communication. Should only use this when all communication is over
 *
 * input: int sockfd -- socket number returned from initialization
 */
void close_connection(int sockfd);
