var mqtt;
var reconnectTimeout = 2000;
var host = "192.168.20.154"; //change this according to your brokers IP
var port = 9001;

// function which parses the different values of the form in index.js
function parseInfo() {
    parseTable = [
        $("#fmin").val(),
        $("#fmax").val(),
        $("#nbpts").val(),
        $("#ampmin").val(),
        $("#ampmax").val(),
        $("#ampres").val(),
        $("#ptd").val(),
        $("#pta").val(),
        $("#spar").val()
    ]
    // the different infos are stored in the browser cache so that they can be used by other scripts
    sessionStorage.setItem("params", JSON.stringify(parseTable));
}