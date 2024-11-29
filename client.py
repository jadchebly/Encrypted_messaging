import socket
import threading
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import rsa, padding
from cryptography.hazmat.primitives.hashes import SHA256
import os

# Global variables
SERVER_IP = "127.0.0.1"
SERVER_PORT = 12345
symmetric_key = None

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

# Receive messages from server
def receive_messages(sock):
    global symmetric_key
    while True:
        try:
            encrypted_message = sock.recv(4096)
            if not encrypted_message:
                break
            print("[SERVER]:", decrypt_message(encrypted_message, symmetric_key))
        except:
            print("[ERROR] Connection closed.")
            break

# Connect to server
def start_client():
    global symmetric_key

    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect((SERVER_IP, SERVER_PORT))

    # Receive server's public key
    server_public_key = serialization.load_pem_public_key(client_socket.recv(4096))

    # Generate RSA key pair for client
    private_key = rsa.generate_private_key(public_exponent=65537, key_size=2048)
    public_key = private_key.public_key()

    # Send client's public key to server
    client_socket.send(public_key.public_bytes(
        encoding=serialization.Encoding.PEM,
        format=serialization.PublicFormat.SubjectPublicKeyInfo
    ))

    # Receive encrypted symmetric key
    encrypted_key = client_socket.recv(4096)
    symmetric_key = private_key.decrypt(
        encrypted_key,
        padding.OAEP(mgf=padding.MGF1(algorithm=SHA256()), algorithm=SHA256(), label=None)
    )

    print("[CONNECTED] Successfully connected to server.")
    threading.Thread(target=receive_messages, args=(client_socket,)).start()

    while True:
        message = input("Enter message: ")
        encrypted_message = encrypt_message(message, symmetric_key)
        client_socket.send(encrypted_message)

start_client()
