var mqtt;
var reconnectTimeout = 2000;
var host = "192.168.1.146"; //change this
var port = 9001;

var dataToExport = new Array();
var dataTable = new Array();

function addToTab() {
    var value = $("#messages").text()
    dataTable.push(value);
}

function onMessageArrived(r_message) {
    out_msg = r_message.payloadString;
    console.log(out_msg);
    $("#messages").text(out_msg);
    addToTab();
    console.log(dataTable);
}


function onConnect() {
    // Once a connection has been made, make a subscription and send a message.

    console.log("Connected");
    //mqtt.subscribe("sensor1");
    mqtt.subscribe("sendTable");
}

function csv_download() {
	dataToExport.length = 0;
    dataToExport.push(dataTable);

    let csvContent = "data:text/csv;charset=utf-8," +
        dataToExport.map(e => e.join(",")).join("\n");

    var encodedUri = encodeURI(csvContent);
    var link = document.createElement("a");
    link.setAttribute("href", encodedUri);
    link.setAttribute("download", "my_data.csv");
    document.body.appendChild(link); // Required for FF
    link.click(); // This will download the data file named "my_data.csv".
    dataTable.length = 0;

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