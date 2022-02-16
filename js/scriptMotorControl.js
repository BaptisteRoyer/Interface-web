console.log("script Motor Control loaded")

var isConnected = false;

var upDownCount = 0;
$('#current_motor_position').text(upDownCount);


// launches a function when a key is pressed
function eventListnenerHandler() {
    document.addEventListener('keydown', keyDownHandler, false);
}


// function called when a key is pressed
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

// function launched when mqtt is connected to a broker
function onConnect() {
    // Once a connection has been made, make a subscription and send a message.

    console.log("Connected");

}

// function deals with mqtt 
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

// input the height of the motor for the maximum position, the start position of the program
function pushStart(){
    $('#ptd').val($('#current_motor_position').text());
}

// input the height of the motor for the minimum position, the end position of the program
function pushEnd(){
    $('#pta').val($('#current_motor_position').text());
}