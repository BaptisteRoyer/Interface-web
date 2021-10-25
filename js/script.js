var mqtt;
var reconnectTimeout = 2000;
var host = "192.168.43.58"; //change this
var port = 1883;

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

        timeout: 5,
        onSuccess: onConnect,
        // onFailure: console.log("ALLO"),

    };

    mqtt.connect(options); //connect
}