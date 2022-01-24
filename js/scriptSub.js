var mqtt;
var reconnectTimeout = 2000;
var host = "192.168.43.58"; //change this according to your brokers IP
var port = 9001;

var isConnected = false;

var dataToExport = new Array();
var impedTable = new Array();
var freqTable = new Array();
var params;

var upDownCount = 0;

function startMesure() {

    var timer = $("#timer").val();

    var topic = "mesureParams";
    message = new Paho.MQTT.Message(timer);
    message.destinationName = topic;
    mqtt.send(message);

    console.log(timer);
    var countDown = setInterval(function() {
        if (timer <= 0) {
            clearInterval(countDown);
            $("#timerWrite").text("Finished");
        } else {
            $("#timerWrite").text(timer);
        }
        timer -= 1;
    }, 1000);

}

function getItems() {
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

function sendParams() {
    for (var i = 0; i <= params.length; i++) {
        if (i == params.length) {
            var topic = "readyToStart";
            message = new Paho.MQTT.Message(" ");
            message.destinationName = topic;
            mqtt.send(message);
        }
        else {
            var topic = "params";
            console.log(params[i]);
            message = new Paho.MQTT.Message(params[i]);
            message.destinationName = topic;
            mqtt.send(message);
        }

    }
}


function onMessageArrived(r_message) {
    if (r_message.destinationName == "sendTable") {
        out_msg = r_message.payloadString;
        out_msg = out_msg.replace(".", ",")
        $("#impedence").text(out_msg);
        impedTable.push(out_msg);
    }
    if (r_message.destinationName == "stopMesure") {
        csv_download()
    }

}


function onConnect() {
    // Once a connection has been made, make a subscription and send a message.

    console.log("Connected");
    mqtt.subscribe("sendTable");
    mqtt.subscribe("stopMesure");
    sendParams();

}

function csv_download() {
    dataToExport.length = 0;
    dataToExport.push(impedTable);

    let csvContent = "data:text/csv;charset=utf-8," +
        dataToExport.map(e => e.join("\n")).join("\n");

    var encodedUri = encodeURI(csvContent);
    var link = document.createElement("a");
    link.setAttribute("href", encodedUri);
    link.setAttribute("download", "my_data.csv");
    document.body.appendChild(link); // Required for FF
    link.click(); // This will download the data file named "my_data.csv".

    impedTable.length = 0;
    freqTable.length = 0;
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

    mqtt.onMessageArrived = onMessageArrived;
    mqtt.connect(options); //connect

}

function sleep(milliseconds) {
    var start = new Date().getTime();
    for (var i = 0; i < 1e7; i++) {
        if ((new Date().getTime() - start) > milliseconds) {
            break;
        }
    }
}