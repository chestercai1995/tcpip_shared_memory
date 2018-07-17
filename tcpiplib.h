#define BUF_SIZE 1
#define CLADDR_LEN 100

/* Initialize the server. need to input the port number.
 * Should probably make sure the port isn't use by anything else
 * This function will wait indefinately for a client to connect.
 * The server must call init_server() before a client can call init_client().
 * See init_client for more information
 * This function returns -1 on failure. The caller should check the return value to insure correct bahavior. 
 *
 * input: int portno -- port number to use
 * output: int sockfd  -- socket number to be used later in transmit and receive
 */
int init_server(int portno);

/* Initialize the client. Need to input the server IP address and port number
 * The port number need to match the port number used in init_server()
 * If this function is called before init_server is called, it will prints out
 * an error message and returns -1 
 * This function returns -1 on failure. The caller should check the return value to insure correct bahavior. 
 *
 * input: char* serveraddr -- string of the server IP address
 *        int portno -- port number to use
 * output: int sockfd -- socket number to be used later in transmit and receive
 */
int init_client(char* serveraddr, int portno);

/* Closes the communication. Should only use this when all communication is over
 *
 * input: int sockfd -- socket number returned from initialization
 * output: int -- success = 0 failure = -1
 */
int close_connection(int sockfd);

/* transmit the File pointed by pointer in. 
 * File needs to be opened with the option rb intead of just r to read the file in binary
 * This function also deso not close the file. 
 * The caller is responsible for that unlike receive
 * This function returns 1 on failure. The caller should check the return value to insure correct bahavior. 
 * 
 * input: FILE* in -- the file you want to transmit
 *        int sockfd -- the socket number returned from initialization
 * output: int -- success = 0 failure = 1
 */
int transmit_file(FILE* in, int sockfd);

/* Receive the File named with the given string. 
 * Note that the input is a char*(string), not a file pointer.
 * This function will open and close the file.
 * This function returns 1 on failure. The caller should check the return value to insure correct bahavior. 
 *
 * input: char* out -- name of the output file(string)
 *        int sockfd -- socket number returned from initialization
 * output: int -- success = 0 failure = 1
 */
int receive_file(char* out, int sockfd);

int transmit_buffer(void* data, int32_t size, int sockfd);

void* receive_buffer(int sockfd);

