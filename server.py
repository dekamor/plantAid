# echo-server.py

import socket

import firebase_admin
from firebase_admin import credentials
from firebase_admin import firestore
from firebase_admin import db

import hashlib
import datetime


cred = credentials.Certificate('plantaid-rvln-firebase-adminsdk-b2s73-5884f84186.json')
firebase_admin.initialize_app(cred)

db = firestore.client()

HOST = "192.168.2.13"  # Standard loopback interface address (localhost)
PORT = 65432  # Port to listen on (non-privileged ports are > 1023)

def write_to_db(humidity_reading, temp_reading, soil_reading):
    time_stamp = datetime.datetime.now()
    time_stamp = datetime.datetime.now()
    doc_name = time_stamp.strftime("%y") + time_stamp.strftime("%m") + time_stamp.strftime("%d") + time_stamp.strftime("%H") + time_stamp.strftime("%M") + time_stamp.strftime("%S")
    doc_ref = db.collection(u'readings').document(u'{}'.format(doc_name))
    doc_ref.set({
        u'date': u'{}'.format(time_stamp.strftime("%x")),
        u'time': u'{}'.format(time_stamp.strftime("%X")),
        u'humidity': u'{}'.format(humidity_reading),
        u'temp': u'{}'.format(temp_reading),
        u'soil_moisture': u'{}'.format(soil_reading)
    })


s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((HOST, PORT))
s.listen()



while True:
    print(f"listening for connection")
    conn, addr = s.accept()
    print(f"Connected by {addr}")
    data = conn.recv(1024)
    print(data.decode("utf-8"))
    values = data.decode("utf-8").split(',')
    try:
        write_to_db(int(values[0]), int(values[1]), int(values[2]))
    except:
        print(data)
        print("Could not convert data to an integer")



