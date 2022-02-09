var mqtt;
var reconnectTimeout = 2000;
var host = "192.168.43.58"; //change this according to your brokers IP
var port = 9001;

var isConnected = false;

var dataToExport = new Array();
var impedTable = new Array();
var freqTable = new Array();
var params;

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

function sendParams(){
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

function parseCSV(path){
    $.ajax({
        url:     "../"+ path,
        cache: false,
        async: false,
        success: function (csv) {
            data = $.csv.toArrays(csv,{
                delimiter: "'",
                separator: ";"
            });
        },
        dataType: "text",
        complete: function () {
            console.log(data);
        }
    });
    
    
    frequency = new Array();
    impedance = new Array();
    phase = new Array();
    
    for (var i = 0; i < data.length; i++) {
        frequency.push(data[i][0])
        impedance.push(data[i][1])
        phase.push(data[i][2])
    }

    drawChart(frequency,impedance,phase);

}

function onMessageArrived(r_message){
    out_msg = r_message.payloadString;

    if (r_message.destinationName == "stopMesure") {
        csv_download()
    }

    else if(r_message.destinationName == "drawGraph"){
        parseCSV(out_msg);
    }

}


function onConnect(){
    // Once a connection has been made, make a subscription and send a message.

    console.log("Connected");
    mqtt.subscribe("stopMesure");
    mqtt.subscribe("drawGraph");
    sendParams();

}

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

function drawChart(frequency,impedance,phase){
    var chart = c3.generate({
        bindto:'#chart',
        point: {
            r: 1.5
        },
        data:{
            x: 'frequency',
            columns: [
                frequency,
                impedance,
                phase
            ]
        }
    })
}