L'organisation est faite ainsi :
	- index.html se lance à la première requête envoyée au serveur.
	- resultViewer permet d'avoir une vue sur la mesure en cours. Indique la position actuelle du robot, l'amplitude de fonctionnement du transducteur et la fréquence de fonctionnement, ainsi que les paramètres indiquées au lancement du programme
	- drawGraph dessine le dernier graph enregistré

	- Dossier JS : 
			- c3, d3, jquery-csv, jquery, mqttws31 sont des bibliothèques externes
					- c3 et d3 gèrent l'affichage de graph
					- jquery-csv la gestion des csv pour l'affichage des graphs
					- jquery un javascript plus facile à utiliser
					- mqttws31 gère le mqtt pour le js

			- script.js est le script avec les fonctions pour index.html
			- scriptGraphDraw.js est le script avec les fonctions pour drawGraph.html
			- scriptResultViewer.js est le script avec les fonctions pour resultViewer
			- scriptMotorControl.js est le script qui gère les contrôles moteur. Appelé par index.html

	- Dossier PyScript :
			- client.py est lancé au démarrage de la raspberry PI
			- impedance_calculator.py contient toutes les formules nécessaires pour le calcul d'impédance

	- Dossier ESP32 contient le code de l'ESP


Fonctionnement global :
	1. On lance une demande de mesure via le formulaire.
	2. Les différentes données sont parse, et envoyées au script Python ainsi qu'à l'ESP
	3. L'ESP effectue ses mesures et à chaque fois qu'une mesure de fréquence a été faite, envoie ses données au script python
	4. Le script python se charge de ranger chacune des mesures dans des fichiers CSV
	5. Une fois que toutes les mesures ont été faites, le script python se charge de calculer l'impédance à telle hauteur, pour telle amplitude, à telle fréquence
	6. Une fois les impédances données pour chacune des fréquences, le script python indique au javascript qu'il peut dessiner le graph.

	Tout cela fonctionne grâce au MQTT. 
	Un broker (raspberry pi) se charge de faire voyager des messages entre différents clients (javascript, python, esp32). Les clients peuvent subscribe ou publish sur un topic, qui est un sujet de discussion en quelques sortes.