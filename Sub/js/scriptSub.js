var mqtt;
var reconnectTimeout = 2000;
var host = "192.168.43.58"; //change this
var port = 9001;

function onMessageArrived(r_message) {
    out_msg = r_message.payloadString;
    console.log(out_msg);
    document.getElementById("messages").innerHTML = out_msg;
}

function onConnect() {
    // Once a connection has been made, make a subscription and send a message.

    console.log("Connected ");
    //mqtt.subscribe("sensor1");
    mqtt.subscribe("topic1")
}

function MQTTconnect() {
    console.log("alo")
    console.log("connecting to " + host + " " + port);
    mqtt = new Paho.MQTT.Client(host, port,"clientJSSubscriber");
    //document.write("connecting to "+ host);
    var options = {

        timeout: 3,
        onSuccess: onConnect,
        // onFailure: console.log("ALLO"),

    };

    mqtt.onMessageArrived = onMessageArrived;
    mqtt.connect(options); //connect

}