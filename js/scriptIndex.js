function parseInfo(){
    parseTable = [$("#fmin").val(),$("#fmax").val(),$("#nbpts").val(),$("#dap").val(),$("#spar").val()]
    sessionStorage.setItem("params", JSON.stringify(parseTable));
}