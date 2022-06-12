#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Arduino_JSON.h>

const char* PARAM_MESSAGE = "message";
JSONVar board;

AsyncWebServer server(80);
AsyncEventSource events("/events");


const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head> <title>COLMENA HORREO</title> <meta name="viewport" content="width=device-width, initial-scale=1"> <script src="https://code.highcharts.com/highcharts.js"></script> <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous"> <link rel="icon" href="data:,"> <style> html { font-family: Arial; display: inline-block; text-align: center; } p { font-size: 1.2rem; } body { margin: 0; } .topnav { overflow: hidden; background-color: #2f4468; color: white; font-size: 1.7rem; } .content { padding: 20px; } .card { background-color: white; box-shadow: 2px 2px 12px 1px rgba(140, 140, 140, .5); } .cards { max-width: 700px; margin: 0 auto; display: grid; grid-gap: 2rem; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); } .reading { font-size: 2.8rem; } .packet { color: #6c6c6c; } .card.temperature { color: #fd7e14; } .card.humidity { color: #1b78e2; } .card.pressure { color: #e21b1b; } </style></head><body> <div class="topnav"> <h3>COLMENA HORREO</h3> </div> <div> <p class="packet">Lectura: <span id="rs1"></span></p> </div> <div class="content"> <div class="cards"> <div class="card temperature"> <h4><i class="fas fa-thermometer-half"></i> TEMPERATURA EXTERIOR</h4> <p><span class="reading"><span id="t_ext1"></span> &deg;C</span></p> </div> <div class="card humidity"> <h4><i class="fas fa-tint"></i> HUMIDADE EXTERIOR</h4> <p><span class="reading"><span id="h_ext1"></span> &percnt;</span></p> </div> <div class="card temperature"> <h4><i class="fas fa-thermometer-half"></i> TEMPERATURA INTERIOR</h4> <p><span class="reading"><span id="t_in1"></span> &deg;C</span></p> </div> <div class="card humidity"> <h4><i class="fas fa-tint"></i> HUMIDADE INTERIOR</h4> <p><span class="reading"><span id="h_in1"></span> &percnt;</span></p> </div> <div class="card pressure"> <h4><i class="fas fa-cloud"></i> PRESION</h4> <p><span class="reading"><span id="press1"></span> mBar</span></p> </div> </div> </div> <div id="chart-pressure" class="container"></div> <script> var chartP = new Highcharts.Chart({ chart: { renderTo: 'chart-pressure' }, title: { text: 'Presion' }, series: [{ showInLegend: false, data: [] }], plotOptions: { line: { animation: false, dataLabels: { enabled: true } }, series: { color: '#18009c' } }, xAxis: { type: 'datetime', dateTimeLabelFormats: { second: '%H:%M:%S' } }, yAxis: { title: { text: 'Pressure (hPa)' } }, credits: { enabled: false } }); if (!!window.EventSource) { var source = new EventSource('/events'); source.addEventListener('open', function (e) { console.log("Events Connected"); }, false); source.addEventListener('error', function (e) { if (e.target.readyState != EventSource.OPEN) { console.log("Events Disconnected"); } }, false); source.addEventListener('message', function (e) { console.log("message", e.data); }, false); source.addEventListener('new_readings', function (e) { console.log("new_readings", e.data); var obj = JSON.parse(e.data); document.getElementById("t_ext" + obj.board_id).innerHTML = obj.temp_ext.toFixed(2); document.getElementById("h_ext" + obj.board_id).innerHTML = obj.hum_ext.toFixed(2); document.getElementById("t_in" + obj.board_id).innerHTML = obj.temp_in.toFixed(2); document.getElementById("h_in" + obj.board_id).innerHTML = obj.hum_in.toFixed(2); document.getElementById("press" + obj.board_id).innerHTML = obj.press.toFixed(2); document.getElementById("rs" + obj.board_id).innerHTML = obj.readingId; var x = (new Date()).getTime(), y = parseFloat(obj.press.toFixed(2)); if (chartP.series[0].data.length > 40) { chartP.series[0].addPoint([x, y], true, true, true); } else { chartP.series[0].addPoint([x, y], true, false, true); } }, false); } </script></body></html>
)rawliteral";

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void runserver()
{
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html", index_html);
    });

    // Send a GET request to <IP>/get?message=<message>
    server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
        String message;
        if (request->hasParam(PARAM_MESSAGE)) {
            message = request->getParam(PARAM_MESSAGE)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, GET: " + message);
    });

    // Send a POST request to <IP>/post with a form field message set to <message>
    server.on("/post", HTTP_POST, [](AsyncWebServerRequest *request){
        String message;
        if (request->hasParam(PARAM_MESSAGE, true)) {
            message = request->getParam(PARAM_MESSAGE, true)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, POST: " + message);
    });

    server.onNotFound(notFound);

    events.onConnect([](AsyncEventSourceClient *client){
        if(client->lastId()){
        Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
        }
        // send event with message "hello!", id current millis
        // and set reconnect delay to 1 second
        client->send("hello!", NULL, millis(), 10000);
    });
    server.addHandler(&events);
    server.begin();
}