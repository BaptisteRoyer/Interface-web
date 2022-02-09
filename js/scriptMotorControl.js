console.log("script Motor Control loaded")

var isConnected = false;

var upDownCount = 0;
$('#current_motor_position').text(upDownCount);

function eventListnenerHandler() {
    document.addEventListener('keydown', keyDownHandler, false);
}

function keyDownHandler(event) {

    if (event.keyCode == 40) {
        upDownCount++;
        $('#current_motor_position').text(upDownCount);

        var topic = "motorMoveDown";
        message = new Paho.MQTT.Message("");
        message.destinationName = topic;
        mqtt.send(message);

    } else if (event.keyCode == 38) {
        upDownCount--;
        $('#current_motor_position').text(upDownCount);
                    
        var topic = "motorMoveUp";
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