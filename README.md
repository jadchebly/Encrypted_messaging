# Encrypted Messaging System  

## Table of Contents
1. Abstract
2. Introduction
3. Key Features
4. Definitions of Key Terminologies
5. How It Works
6. Testing
7. How to Run



## Abstract  
This project aims to develop a secure, real-time messaging system utilizing socket programming and encryption techniques. This system enables reliable communication between multiple clients and a server over a TCP/IP network. Messages exchanged between clients are encrypted to ensure confidentiality and protect user data during transmission.

## Introduction  
With increasing concerns about data privacy, secure communication protocols are essential for modern applications. This project leverages **TCP/IP sockets** to establish reliable connections and implements **encryption mechanisms** to secure data exchange. The application ensures real-time message delivery between clients while protecting against unauthorized access using encryption.

### Key Features:
- 🔒 **End-to-End Encryption**: Protects messages from unauthorized access.  
- ⚡ **Real-Time Messaging**: Supports instant communication with minimal latency.  
- 🖧 **Socket Programming**: Built on TCP sockets for reliable data transmission.  
- 🛠️ **Modular Design**: Clean architecture separating networking and encryption logic.  


## Definitions of Key Terminologies  
- **Socket**: A virtual endpoint for communication between the client and server. It is defined by the combination of an IP address and port number. Sockets enable message exchange over a network.  
- **IP Address**: A unique identifier assigned to each device in a network, enabling the location and identification of devices.  
- **Bind**: A server-side operation that assigns an IP address and port to a socket, allowing it to listen for incoming connections.  
- **Listen**: Prepares the server socket to accept incoming client connections.  
- **Accept**: Accepts a new connection from a client, establishing a dedicated communication channel.  
- **Send/Receive**: Transmit and receive messages over an established connection.  
- **Encrypt/Decrypt**: Secures data using an encryption algorithm, ensuring that only authorized users can read transmitted messages.  

## How It Works  
1. The server initializes a socket and binds it to a specific IP address and port.  
2. The server listens for incoming connections, and clients establish connections by using their sockets to connect to the server.  
3. Upon connection, messages sent by the client are encrypted and transmitted to the server.  
4. The server decrypts the message, processes it, and sends an encrypted response back to the client.  
5. Both sides decrypt messages locally to ensure secure communication.

## How to Run  
### Prerequisites:
- A system with a C or Python compiler (depending on your implementation).
- Multiple devices connected to the same network or configured for TCP/IP communication.
- Basic knowledge of terminal commands.

### Testing
1. Start the server and connect a client.
2. Send a test message from the client. Verify that:
   - The message appears encrypted in the terminal.
   - The server decrypts and processes the message correctly.
3. Send a response back to the client. Ensure proper decryption at the client's end.

### Instructions:
1. **Set Up the Network**: Ensure the devices (nodes) are on the same LAN or connected through a configured network. Use `ifconfig` or `ipconfig` to retrieve the IP addresses of the nodes.  
2. **Edit Configuration**: Update the server and client code with the correct IP addresses and port numbers.  
3. **Compile Code**:  
   - For C implementation:  
     ```bash
     gcc server.c -o server
     gcc client.c -o client
     ```  
   - For Python implementation, ensure you have Python 3 installed.  
4. **Run the Applications**:  
   - Start the server:  
     ```bash
     ./server
     ```  
   - Start the client in a separate terminal:  
     ```bash
     ./client
     ```  
5. **Exchange Messages**: The client can now send encrypted messages to the server, and the server will respond securely.


- A functional encrypted messaging application.
- Demonstration of secure real-time communication over TCP/IP.
- Insight into socket programming and encryption integration for secure data transmission.

---
