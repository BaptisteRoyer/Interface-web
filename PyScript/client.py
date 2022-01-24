import csv
from genericpath import exists
import os
import paho.mqtt.client as mqtt
import tarfile

# from impedance_calculator import *

directory_exists = True
directory_name_counter = 0
main_directory_name = "mesure"
voltage_list = []
current_list = []

def on_message(client,userdata,message):
	global voltage_list, current_list
	new_message = str(message.payload.decode("utf-8"))
	
	if message.topic == "firstPosition":
		os.mkdir(new_message)
		os.chdir(new_message)

	elif message.topic == "firstAmp":
		os.mkdir(new_message)
		os.chdir(new_message)	

	elif message.topic == "newPosition":
		os.chdir("../../")
		os.mkdir(new_message)
		os.chdir(new_message)

	elif message.topic == "newAmp":
		os.chdir("../")
		os.mkdir(new_message)
		os.chdir(new_message)

	elif message.topic == "newFreq":
		os.mkdir(new_message)
		os.chdir(new_message)
	
	elif message.topic == "sendValues":
		values = new_message.split(";")
		if values[0] == "DONE":
			print ("DONE")
			with open('values.csv', 'w', newline='') as csvfile:
				fieldnames = ['voltage', 'current']
				writer = csv.DictWriter(csvfile, delimiter=';',fieldnames=fieldnames)
				writer.writeheader()

				for i in range(len(voltage_list)):
					writer.writerow({'voltage': voltage_list[i], 'current': current_list[i]})
				
				voltage_list.clear()
				current_list.clear()
		
		else:
			voltage_list.append(values[0])
			current_list.append(values[1])
			print(new_message)
		

	# if "zip" in new_message:
	elif message.topic == "zip":
		os.chdir("../../../")
		tar = tarfile.open(main_directory_name + ".tar.gz", "w:gz")
		tar.add("main_directory_name", arcname="main_directory_name")
		tar.close()

	# else:
	# 	os.system("echo " + new_message + " >> " + file)

def on_connect(client, userdata, flags, properties=None):
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
os.mkdir(main_directory_name)
os.chdir(main_directory_name)

client_name = "python_client"
host = "192.168.43.58"
client = mqtt.Client(client_name)

client.connect(host)
client.on_connect = on_connect
client.on_message = on_message
client.subscribe("firstPosition")
client.subscribe("firstAmp")
client.subscribe("newPosition")
client.subscribe("newAmp")
client.subscribe("newFreq")
client.subscribe("sendValues")
client.subscribe("zip")

try:
	while True:
		client.loop()

except KeyboardInterrupt:
	client.loop_stop()