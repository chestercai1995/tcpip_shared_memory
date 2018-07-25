#include <semaphore.h>

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

void *listener (void *arg);

/* Closes the communication. Should only use this when all communication is over
 *
 * input: int sockfd -- socket number returned from initialization
 * output: int -- success = 0 failure = -1
 */
int close_connection();

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
int transmit_file(FILE* in);

/* Receive the File named with the given string. 
 * Note that the input is a char*(string), not a file pointer.
 * This function will open and close the file.
 * This function returns 1 on failure. The caller should check the return value to insure correct bahavior. 
 *
 * input: char* out -- name of the output file(string)
 *        int sockfd -- socket number returned from initialization
 * output: int -- success = 0 failure = 1
 */
int receive_file(char* out);

/* Transmits specified bytes of data using sockfd. 
 * data input is a void pointer to work with any type of data
 *
 * input: void* data -- data buffer
 *        int32_t size -- data size
 *        int sockfd -- socket number returned from initialization 
 * output: 0 on success, 1 on error
 */
int transmit_buffer(void* data, int32_t size);

/* Receive data transmitted by transmit_buffer()
 *
 * input: int sockfd -- socket number returned from initialization
 * output: void* -- buffer that holds the data, need to be cast by user into apporiate type
 * on failure, it returns a NULL pointer
 */
void* receive_buffer();

int transmit_int32(int32_t num);

int transmit_char(char num);

int init_sm(void* data, int32_t size);

int accept_sm();

int destroy_sm();

void* read_sm(int start, int size);

int write_sm(void* data, int32_t start, int size);

int sys_sync();
