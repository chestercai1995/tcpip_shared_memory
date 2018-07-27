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

/* initiate a shared memory, data will be put in the shared memory. only one side can call this, the other side must call accept _sm.
 * Starting a shared memory will also start a listening thread which will handle all of the receives. You should not use transmit_buffer or transmit_file when a shared memory is in use. 
 * You can only have one shared memory at a time. If you need to store more than one data type in the shared memory, use a struct.
 * This function must be paired with accept_sm()
 *
 * input: void* data, numbers/strings/chars/structs(whatever you want) to be in the shared memory
 *        int size size of the data
 * output: int 0 on success, 1 on failure
 */
int init_sm(void* data, int32_t size);

/* accept an shared memory, the other side should have called or are going to call init_sm
 * This function acts the same way as the init_sm, the only difference is that it would receive the data and size from the init side instead of the user.
 * This function also creates a listening thread. transmit_buffer or transmit_file should not be used while a shared memory is in use.
 * This function must be paired with init_sm()
 * 
 * output: int 0 on success, 1 on failure
 */
int accept_sm();

/* destroy the shared memory in use. data in the shared memory are lost. 
 * listening thread will be canceled at the next cancellation point. look at man 7 threads
 * 
 * returns 0 on success, 1 on failure
 */
int destroy_sm();

/* this would return a void pointer to an array with specified sized. the data in returning array would be from the starting byte with specified size
 * the pointer needs to be freed by the user in order for it not to be 
 */
void* read_sm(int start, int size);

int write_sm(void* data, int32_t start, int size);

int resize_sm(int new_size);

int sys_sync();
