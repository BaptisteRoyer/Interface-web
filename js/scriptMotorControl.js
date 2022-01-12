console.log("script Motor Control loaded")
var mqtt;
var reconnectTimeout = 2000;
var host = "192.168.43.58"; //change this
var port = 9001;

var isConnected = false;

var upDownCount = 0;

function eventListnenerHandler() {
    document.addEventListener('keydown', keyDownHandler, false);
}

function keyDownHandler(event) {

    if (event.keyCode == 40) {
        upDownCount--;

        var topic = "motorMoveDown";
        console.log("motorMoveDown");
        message = new Paho.MQTT.Message("");
        message.destinationName = topic;
        mqtt.send(message);

    } else if (event.keyCode == 38) {
        upDownCount++;

        var topic = "motorMoveUp";
        console.log("motorMoveUp");
        message = new Paho.MQTT.Message("");
        message.destinationName = topic;
        mqtt.send(message);

    }
}

function onConnect() {
    // Once a connection has been made, make a subscription and send a message.

    console.log("Connected");

}

function MQTTconnect() {

    console.log("connecting to " + host + " " + port);
    mqtt = new Paho.MQTT.Client(host, port, "clientJSSubscriber");
    //document.write("connecting to "+ host);
    var options = {

        timeout: 3,
        onSuccess: onConnect,
        // onFailure: console.log("ALLO"),

    };

    mqtt.connect(options); //connect

}