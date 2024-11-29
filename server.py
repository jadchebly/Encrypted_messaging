import socket
import threading
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import rsa, padding
from cryptography.hazmat.primitives.hashes import SHA256
import os

# Global variables
clients = {}  # To store client sockets and symmetric keys
SERVER_IP = "127.0.0.1"
SERVER_PORT = 12345

# RSA keys (for secure key exchange)
private_key = rsa.generate_private_key(public_exponent=65537, key_size=2048)
public_key = private_key.public_key()

# Encrypt message using AES
def encrypt_message(message, key):
    iv = os.urandom(16)  # Initialization vector
    cipher = Cipher(algorithms.AES(key), modes.CFB(iv))
    encryptor = cipher.encryptor()
    encrypted = encryptor.update(message.encode()) + encryptor.finalize()
    return iv + encrypted

# Decrypt message using AES
def decrypt_message(encrypted_message, key):
    iv = encrypted_message[:16]
    cipher = Cipher(algorithms.AES(key), modes.CFB(iv))
    decryptor = cipher.decryptor()
    decrypted = decryptor.update(encrypted_message[16:]) + decryptor.finalize()
    return decrypted.decode()

# Broadcast messages to all connected clients
def broadcast(sender_socket, encrypted_message):
    for client, key in clients.items():
        if client != sender_socket:  # Don't send back to sender
            try:
                client.send(encrypted_message)
            except:
                client.close()
                del clients[client]

# Handle each client
def handle_client(client_socket, addr):
    print(f"[NEW CONNECTION] {addr} connected.")

    # Secure key exchange
    client_public_key = serialization.load_pem_public_key(client_socket.recv(4096))
    symmetric_key = os.urandom(32)  # Generate AES key
    encrypted_key = client_public_key.encrypt(
        symmetric_key,
        padding.OAEP(mgf=padding.MGF1(algorithm=SHA256()), algorithm=SHA256(), label=None)
    )
    client_socket.send(encrypted_key)
    clients[client_socket] = symmetric_key  # Save key for this client

    while True:
        try:
            encrypted_message = client_socket.recv(4096)
            if not encrypted_message:
                break

            decrypted_message = decrypt_message(encrypted_message, symmetric_key)
            print(f"[{addr}] {decrypted_message}")
            broadcast(client_socket, encrypted_message)
        except:
            break

    print(f"[DISCONNECT] {addr} disconnected.")
    client_socket.close()
    del clients[client_socket]

# Start server
def start_server():
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((SERVER_IP, SERVER_PORT))
    server_socket.listen(5)
    print(f"[LISTENING] Server running on {SERVER_IP}:{SERVER_PORT}")

    while True:
        client_socket, addr = server_socket.accept()
        # Send server's public key to client
        client_socket.send(public_key.public_bytes(
            encoding=serialization.Encoding.PEM,
            format=serialization.PublicFormat.SubjectPublicKeyInfo
        ))

        thread = threading.Thread(target=handle_client, args=(client_socket, addr))
        thread.start()
        print(f"[ACTIVE CONNECTIONS] {threading.active_count() - 1}")

start_server()
