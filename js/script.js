var mqtt;
var reconnectTimeout = 2000;
var host = "192.168.43.58"; //change this according to your brokers IP
var port = 9001;

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
    sessionStorage.setItem("params", JSON.stringify(parseTable));
}