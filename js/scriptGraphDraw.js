function getItems(){
    var path = "";
    var position = "";
    var amplitude = "";

    pathCSV = JSON.parse(sessionStorage.getItem("pathCSV"));

    path = pathCSV[0];
    position = pathCSV[2];
    amplitude = pathCSV[1];

    $("#curPosition").append(position)
    $("#curAmplitude").append(amplitude);

    parseCSV(path);
    
}

function parseCSV(path){
    $.ajax({
        url:    "../" + path,
        cache:  false,
        async:  false,
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