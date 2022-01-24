from genericpath import exists
import matplotlib.pyplot as plt
import numpy as np
import csv
import os
import paho.mqtt.client as mqtt
import tarfile

from impedance_calculator import *

directory_exists = True
directory_name_counter = 0
main_directory_name = "mesure"

def on_message(client,userdata,message):
	new_message = str(message.payload.decode("utf-8"))
	
	if message.topic == "newPosition":
		position_directory_name = new_message


	elif message.topic == "newAmp":
		print("1")

	elif message.topic == "newFreq":
		print("1")

	# if "zip" in new_message:
	if message.topic == "zip":
		os.chdir("../../../")
		tar = tarfile.open(main_directory_name + ".tar.gz", "w:gz")
		tar.add("main_directory_name", arcname="main_directory_name")
		tar.close()

	# else:
	# 	os.system("echo " + new_message + " >> " + file)

def on_connect(client, userdata, flags):
	print("Connected")

while directory_exists:
	if os.path.exists(main_directory_name):
		new_directory_name = "mesure_" +str(directory_name_counter)
		directory_name_counter += 1
		print(main_directory_name + " already exists. Attempting to create " + new_directory_name)
		main_directory_name = new_directory_name
	else:
		directory_exists = False

print("Directory " + main_directory_name + " created")
os.system("mkdir "+ main_directory_name)
os.chdir(main_directory_name)

# client_name = "python_client"
# host = "192.168.43.58"
# client = mqtt.Client(client_name)

# client.connect(host)
# client.on_message = on_message

# try:
# 	while True:
# 		client.loop()

# except KeyboardInterrupt:
# 	client.loop_stop()