var mqtt;
var reconnectTimeout = 2000;
var host = "localhost"; //change this according to your brokers IP
var port = 9001;

var isConnected = false;

var dataToExport = new Array();
var impedTable = new Array();
var freqTable = new Array();
var params;

// function which gets all items stored in the sessionStorage "params" and puts the into an array 
function getItems(){
    var fmin = 0;
    var fmax = 0;
    var nbpts = 0;
    var ampmin = 0;
    var ampmax = 0;
    var ampres = 0;
    var ptd = 0;
    var pta = 0;
    var spar = 0;

    params = JSON.parse(sessionStorage.getItem("params"));

    fmin = params[0];
    fmax = params[1];
    nbpts = params[2];
    ampmin = params[3];
    ampmax = params[4];
    ampres = params[5];
    ptd = params[6];
    pta = params[7];
    spar = params[8];
    
    // writes the previous information on the web page
    $("#fmin").append(fmin);
    $("#fmax").append(fmax);
    $("#nbpts").append(nbpts);
    $("#ampmin").append(ampmin);
    $("#ampmax").append(ampmax);
    $("#ampres").append(ampres);
    $("#ptd").append(ptd);
    $("#pta").append(pta);
    $("#spar").append(spar);

}

// function needed to send the different parameters to the python and esp
function sendParams(){
    
    for (var i = 0; i < params.length; i++) {
                
        var topic = "params";
        console.log(params[i]);
        message = new Paho.MQTT.Message(params[i]);
        message.destinationName = topic;
        mqtt.send(message);
        
    }

    var topic = "readyToStart";
    message = new Paho.MQTT.Message(" ");
    message.destinationName = topic;
    mqtt.send(message);
}

// this function deals with every message received on the mqtt thread
function onMessageArrived(r_message){
    out_msg = r_message.payloadString;

    // if the message is "drawGraph" we open a new browser tab with the corresponding graph
    if(r_message.destinationName == "drawGraph"){
        parseTable = [
            out_msg,
            $("#curAmplitude").val(),
            $("#curPosition").val()
        ]

        sessionStorage.setItem("pathCSV", JSON.stringify(parseTable));

        window.open("../drawGraph.html", '_blank').focus();
    }

    // if the message is "newFreq" we change the text at the id "curFrequency"
    else if(r_message.destinationName == "newFreq"){
        $("#curFrequency").text(out_msg);
    }

    // if the message is "newAmp" we change the text at the id "curAmplitude"
    else if(r_message.destinationName == "newAmp"){
        $("#curAmplitude").text(out_msg);
    }

    // if the message is "newPosition" we change the text at the id "curPosition"
    else if(r_message.destinationName == "newPosition"){
        $("#curPosition").text(out_msg);
    }

}


function onConnect(){
    // Once a connection has been made, make a subscription and send a message.

    console.log("Connected");
    mqtt.subscribe("stopMesure");
    mqtt.subscribe("drawGraph");
    mqtt.subscribe("newFreq");
    mqtt.subscribe("newAmp");
    mqtt.subscribe("newPosition");
    sendParams();

}

// function deals with mqtt
function MQTTconnect(){

    console.log("connecting to " + host + " " + port);
    mqtt = new Paho.MQTT.Client(host, port, "clientJSSubscriber");
    //document.write("connecting to "+ host);
    var options = {

        timeout: 3,
        onSuccess: onConnect,
        // onFailure: console.log("ALLO"),

    };

    mqtt.onMessageArrived = onMessageArrived;
    mqtt.connect(options); //connect

}

function sleep(milliseconds){
    var start = new Date().getTime();
    for (var i = 0; i < 1e7; i++) {
        if ((new Date().getTime() - start) > milliseconds) {
            break;
        }
    }
}