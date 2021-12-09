var mqtt;
var reconnectTimeout = 2000;
var host = "192.168.1.146"; //change this
var port = 9001;

var isConnected = false;

var dataToExport = new Array();
var impedTable = new Array();
var freqTable = new Array();
var params;

function getItems() {
    var fmin = 0;
    var fmax = 0;
    var nbpts = 0;
    var dap = 0;
    var spar = 0;

    params = JSON.parse(sessionStorage.getItem("params"));

    fmin = params[0];
    fmax = params[1];
    nbpts = params[2];
    dap = params[3];
    spar = params[4];

    $("#fmin").append(fmin);
    $("#fmax").append(fmax);
    $("#nbpts").append(nbpts);
    $("#dap").append(dap);
    $("#spar").append(spar);

    var skip = false;
}

function sendParams() {
    for (var i = 0; i < params.length; i++) {
        var topic = "mesureParams";
        console.log(params[i]);
        message = new Paho.MQTT.Message(params[i]);
        message.destinationName = topic;
        mqtt.send(message);

    }
}

function addToTab() {
    var value = $("#impedence").text()
    impedTable.push(value);
}

function onMessageArrived(r_message) {
    out_msg = r_message.payloadString;
    console.log(out_msg);
    $("#impedence").text(out_msg);
    addToTab();
    console.log(impedTable);
}


function onConnect() {
    // Once a connection has been made, make a subscription and send a message.

    console.log("Connected");

    mqtt.subscribe("sendTable");
    sendParams();

}

function csv_download() {
    dataToExport.length = 0;
    dataToExport.push(impedTable);

    let csvContent = "data:text/csv;charset=utf-8," +
        dataToExport.map(e => e.join(",")).join("\n");

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