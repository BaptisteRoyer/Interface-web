import matplotlib.pyplot as plt
import numpy as np
import os
import paho.mqtt.client as mqtt
import tarfile

from impedance_simulator import *

toto = "test_dir"
tata = "test_dir_dir"
titi = "test_dir_dir_dir"
file = "ma_bite.csv"

def on_message(client,userdata,message):
	new_message = str(message.payload.decode("utf-8"))
	if "zip" in new_message:
		os.chdir("../../../")
		tar = tarfile.open(toto + ".tar.gz", "w:gz")
		tar.add("toto", arcname="toto")
		tar.close()

	else:
		os.system("echo " + new_message + " >> " + file)

def on_connect(client, userdata, flags, rc):
	print("Connection returned result: " + connack_string(rc))

os.system("mkdir "+toto)
os.chdir(toto)
os.system("mkdir " + tata)
os.chdir(tata)
os.system("mkdir " + titi)
os.chdir(titi)
os.system("touch "+file)

client_name = "python_client"
host = "192.168.43.58"
client = mqtt.Client(client_name)

client.connect(host)
client.on_message = on_message

client.subscribe("tableTest")

try:
	while True:
		client.loop()

except KeyboardInterrupt:
	client.loop_stop()