from http import client
# from dotenv import load_dotenv, find_dotenv
# import os
from pymongo import MongoClient
import time
import socket

HOST = "192.168.31.128"  # Standard loopback interface address (localhost)
PORT = 65432  # Port to listen on (non-privileged ports are > 1023)
"""

HOST = 'localhost'
PORT = 5000
"""
#start = time.time()
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((HOST, PORT))
    s.listen()
    conn, addr = s.accept()
    with conn:
        print(f"Connected by {addr}")
       
#        data = conn.recv(1024)
#        actData = str(data)[2:-1]
#        print("data received :",actData,len(actData))
# load_dotenv(find_dotenv())
        while True:
            string = conn.recv(1024)
            string = string.decode("utf-8")
            print(string)
# connection_string = os.environ.get("MONGO_URL_LOCAL")
            client = MongoClient("mongodb://localhost:27017")	
            collection = client.domains.domainNamesList
            start = time.time()
            query = {"domainName": string}
            cursor = collection.find_one(query)
# pprint.pprint(collection.find_one(query))
#        res = "1" if cursor else "0"

#        ress = res.encode()
#        print("res",res,ress,res.encode())
#        conn.sendall(ress)

            if cursor :
#            conn.send(b"yes")
                msg = "yes\0"
                conn.send(msg.encode())
            else :
#            conn.send(b"no")
                msg = "no\0"
                conn.send(msg.encode())
        
        conn.close()
print(time.time() - start)

#conn.close()
# for i in cursor:
#   print(i)

