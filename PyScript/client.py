from genericpath import exists
from impedance_calculator import *

import sys
import csv
import pandas
import os
import paho.mqtt.client as mqtt
import tarfile

# from impedance_calculator import *

# directory generator variables
directory_exists = True
directory_name_counter = 0
main_directory_name = "mesure"

# lists of voltage and current to generate csv files
voltage_list = []
current_list = []

def on_message(client,userdata,message):
	global voltage_list, current_list
	new_message = str(message.payload.decode("utf-8"))


	# sets the first position directory
	if message.topic == "firstPosition":
		os.mkdir(new_message)
		os.chdir(new_message)

	# sets the first amplitude directory
	elif message.topic == "firstAmp":
		os.mkdir(new_message)
		os.chdir(new_message)	
		client.publish("pythonReady")

	# sets the first frequency directory
	elif message.topic == "firstFreq":
		os.mkdir(new_message)
		os.chdir(new_message)

	# sets the directory for a new position
	elif message.topic == "newPosition":
		os.chdir("../../../")
		os.mkdir(new_message)
		os.chdir(new_message)

	#sets the directory for a new amplitude
	elif message.topic == "newAmp":
		os.chdir("../../")
		os.mkdir(new_message)
		os.chdir(new_message)

	# sets the directory for a new frequency
	elif message.topic == "newFreq":
		os.chdir("../")
		os.mkdir(new_message)
		os.chdir(new_message)
	
	# puts the voltage and current in a csv file for calculation
	elif message.topic == "voltageCurrent":
		values = new_message.split(";")

		# message recieved from esp can be DONE
		if values[0] == "DONE":
			print ("DONE")
			
			# creates a new csv file and puts propper voltage and current in it
			with open('values.csv', 'w', newline='') as csvfile:
				fieldnames = ['voltage', 'current']
				writer = csv.DictWriter(csvfile, delimiter=';',fieldnames=fieldnames)
				writer.writeheader()

				for i in range(len(voltage_list)):
					writer.writerow({'voltage': voltage_list[i], 'current': current_list[i]})
				
				voltage_list.clear()
				current_list.clear()
				
		else:
			print(new_message)
			voltage_list.append(values[0])
			current_list.append(values[1])
	
	# calculates impedance with values from values.csv
	elif message.topic == "calculateImpedance":
		os.chdir("../../")

		with open('impedance.csv', 'w', newline='') as csvfile:
				fieldnames = ['frequency', 'impedance','phase']
				writer = csv.DictWriter(csvfile, delimiter=';',fieldnames=fieldnames)
				writer.writeheader()

		dirList = os.listdir()

		# seeks through all frequency directories to calculate impedance and phase
		for i in range(len(dirList)):
			os.chdir(dirList[i])
			
			colnames = ['voltage','current']
			data = pandas.read_csv('values.csv', names=colnames)
			
			voltage = data.voltage.tolist()
			current = data.current.tolist()


			curFrequency = os.path.basename(os.getcwd()) # current working directory name
			curImpedance = get_impedance(voltage, current,1)[0]
			curPhase = get_impedance(voltage, current,1)[1]

			writer.writerow({'frequency': curFrequency, 'impedance':curImpedance,'phase': curPhase})
			os.chdir('../')
		
		cwd = os.getcwd()
		cwd_split = cwd.split("Interface web",1)[1]
		client.publish("drawGraph",cwd_split)
	
	# if "zip" in new_message:
	elif message.topic == "zip":
		os.chdir("../../../")
		tar = tarfile.open(main_directory_name + ".tar.gz", "w:gz")
		tar.add("main_directory_name", arcname="main_directory_name")
		tar.close()
		# os.execv(sys.argv[0], sys.argv)



# debug option to see if client is connected
def on_connect(client, userdata, flags, properties=None):
	print("Connected")

# automatically creates a directory name for the values
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


# the different mqtt topics to subscribe
client.subscribe("firstPosition")
client.subscribe("firstAmp")
client.subscribe("newPosition")
client.subscribe("newAmp")
client.subscribe("newFreq")
client.subscribe("sendValues")
client.subscribe("zip")



try:
	# infinite loop
	while True:
		client.loop()

# stops if ctrl+C is pressed
except KeyboardInterrupt:
	client.loop_stop()