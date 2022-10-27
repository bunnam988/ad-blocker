from http import client
# from dotenv import load_dotenv, find_dotenv
# import os
from pymongo import MongoClient
import time
import socket

HOST = "192.168.31.128" 
PORT = 65432  # Port to listen on 


with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((HOST, PORT))
    s.listen()
    conn, addr = s.accept()
    with conn:
        print(f"Connected by {addr}")
       
        while True:
            string = conn.recv(1024)
            string = string.decode("utf-8")
            print(string)
            client = MongoClient("mongodb://localhost:27017")	
            collection = client.domains.domainNamesList
            start = time.time()
            query = {"domainName": string}
            cursor = collection.find_one(query)
            if cursor :
                msg = "yes\0"
                conn.send(msg.encode())
            else :
                msg = "no\0"
                conn.send(msg.encode())
        
        conn.close()
print(time.time() - start)
