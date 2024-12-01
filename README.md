# Encrypted Messaging System  

## Table of Contents  
1. Abstract  
2. Purpose  
3. Key Features  
4. Definitions of Key Terminologies  
5. How It Works  
6. File Structure  
7. Compilation and Setup  
8. Testing  
9. Example Usage  
10. Acknowledgments  

---

## Abstract  
This project demonstrates a secure, real-time messaging system using sockets and basic encryption. It ensures reliable communication over a TCP/IP network while protecting message confidentiality through encryption.

---

## Purpose  
The project is designed for:
- Learning networking and encryption fundamentals.
- Exploring modular programming in C.
- Building secure internal communication tools.

---

## Key Features  
- 🔒 **End-to-End Encryption**: Secure data exchange.  
- ⚡ **Real-Time Messaging**: Instant communication.  
- 🖧 **Socket Programming**: Reliable TCP communication.  
- 🛠️ **Modular Design**: Separated networking and encryption logic.  

---

## Definitions of Key Terminologies  
- **Socket**: A virtual endpoint for communication between the client and server. It is defined by the combination of an IP address and port number. Sockets enable message exchange over a network.  
- **IP Address**: A unique identifier assigned to each device in a network, enabling the location and identification of devices.  
- **Bind**: A server-side operation that assigns an IP address and port to a socket, allowing it to listen for incoming connections.  
- **Listen**: Prepares the server socket to accept incoming client connections.  
- **Accept**: Accepts a new connection from a client, establishing a dedicated communication channel.  
- **Send/Receive**: Transmit and receive messages over an established connection.  
- **Encrypt/Decrypt**: Secures data using an encryption algorithm, ensuring that only authorized users can read transmitted messages.  

---

## How It Works  
1. The server initializes a socket and binds it to a specific IP address and port.  
2. The server listens for incoming connections, and clients establish connections by using their sockets to connect to the server.  
3. Upon connection, messages sent by the client are encrypted and transmitted to the server.  
4. The server decrypts the message, processes it, and sends an encrypted response back to the client.  
5. Both sides decrypt messages locally to ensure secure communication.

---

## File Structure  

The project consists of the following files:

| **File Name**   | **Description**                                                                 |
|------------------|---------------------------------------------------------------------------------|
| `client.c`       | Implements the client-side logic, including message encryption and transmission.|
| `server.c`       | Implements the server-side logic, including message decryption and response.   |
| `encryption.c`   | Contains the implementation of encryption and decryption functions.            |
| `encryption.h`   | Header file declaring encryption-related functions for use in other modules.   |

The `client.c` and `server.c` files interact with `encryption.o` for encryption and decryption logic.

---

## Compilation and Setup  

### Step 1: Set Up the Files
Ensure all files (`client.c`, `server.c`, `encryption.c`, `encryption.h`) are in the same directory.  

### Step 2: Compile the Encryption Module
Run the following command to compile the encryption logic:
```bash
gcc -c encryption.c -o encryption.o
```

### Step 3: Compile and Link the Server
Compile the server with the encryption object file:
```bash
gcc -o server.exe server.c encryption.o -lws2_32
```

### Step 4: Compile and Link the Client
Compile the client with the encryption object file:
```bash
gcc -o client.exe client.c encryption.o -lws2_32
```

---

## Testing  

1. **Start the Server**:
   Run the server in one terminal:
   ```bash
   server.exe
   ```
   - The server should display: `[SERVER] Listening on port 12345`.

2. **Start the Client**:
   Run the client in another terminal:
   ```bash
   client.exe
   ```
   - The client should display: `[CLIENT] Connected to server at 127.0.0.1:12345`.

3. **Exchange Messages**:
   - **Client**: Enter a message (e.g., `Hello, Server!`).
   - **Server**: The decrypted message will appear in the server terminal.
   - **Client**: The response from the server (e.g., `Server received: Hello, Server!`) will appear decrypted.

4. **Troubleshooting**:
   - If the client fails to connect, ensure the server is running and listening on the correct IP and port.
   - Verify the compiled object files (`encryption.o`) exist in the same directory.

---

## Example Usage  

### Server Output:
```
[SERVER] Listening on port 12345...
[SERVER] Waiting for a new client...
[SERVER] Client connected.
[SERVER RECEIVED] Hello, Server!
```

### Client Output:
```
[CLIENT] Connected to server at 127.0.0.1:12345
[CLIENT] Enter a message (type 'exit' to quit): Hello, Server!
[CLIENT RECEIVED] Server received: Hello, Server!
```

---

## Acknowledgments  
- **GCC Compiler**: For compiling and linking the C code.  
- **Winsock2 Library**: For enabling socket programming on Windows.  
- **C Programming Language**: For its versatility in systems programming.  

## Resources  
- [Beej’s Guide to Network Programming](https://beej.us/guide/bgnet/)  
- [GCC Documentation](https://gcc.gnu.org/onlinedocs/)  

---
