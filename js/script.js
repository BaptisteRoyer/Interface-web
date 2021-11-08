var mqtt;
var reconnectTimeout = 2000;
var host = "192.168.1.146"; //change this
var port = 9001;

function onConnect() {
    // Once a connection has been made, make a subscription and send a message.

    console.log("Connected ");
    //mqtt.subscribe("sensor1");
    message = new Paho.MQTT.Message("Hello World");
    message.destinationName = "sensor1";
    mqtt.send(message);
}

function MQTTconnect() {
    console.log("connecting to " + host + " " + port);
    mqtt = new Paho.MQTT.Client(host, port,"clientJS");
    //document.write("connecting to "+ host);
    var options = {

        timeout: 3,
        onSuccess: onConnect,
        // onFailure: console.log("ALLO"),

    };

    mqtt.onMessageArrived = onMessageArrived;
    mqtt.connect(options); //connect
}