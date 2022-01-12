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