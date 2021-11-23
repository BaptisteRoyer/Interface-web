var mqtt;
var reconnectTimeout = 2000;
var host = "192.168.1.146"; //change this
var port = 9001;






function send_message() {


    var value = $("#numberInPub").val()
    var msg = value;
    // var msg = dataTable[i];
    console.log(msg);
    var topic = "tableTest";
    message = new Paho.MQTT.Message(msg);

    message.destinationName = topic;
    mqtt.send(message);



}


function onConnect() {
    // Once a connection has been made, make a subscription and send a message.

    console.log("Connected ");
}

function MQTTconnect() {
    console.log("connecting to " + host + " " + port);
    mqtt = new Paho.MQTT.Client(host, port, "clientJSPublisher");
    //document.write("connecting to "+ host);
    var options = {

        timeout: 3,
        onSuccess: onConnect,
        // onFailure: console.log("ALLO"),

    };

    mqtt.connect(options); //connect
}

$("#formMessageSubmit").submit(function(e) {
    e.preventDefault();
});