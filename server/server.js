

const express = require('express');
const http = require('http');
const fs = require('fs');
const path = require('path');

const OSC = require('./osc');
const BLE = require('./ble');
const SerialUSB = require('./serialusb');
const Footswitch = require('./footswitch');


const deviceName = 'PiAMP';
const port = 3600;
const oscPort = 9001;
const version = "1.0.0";
const irsFolder = "/home/mind/Documents/GuitarML/NeuralPi/irs";
const modelsFolder = "/home/mind/Documents/GuitarML/NeuralPi/tones";

var oscClient = new OSC(oscPort);
oscClient.start();

var app = express();
var server = http.createServer(app);

var serialusb = new SerialUSB();
serialusb.receivedCallbacks.push(function(data) {
    console.log("Serial data received: ", data);
});
serialusb.start();




var parameters = {};

function updateParameter(parameter, value) {
    if(parseFloat(value) == value) {
        value = parseFloat(value);
    }

    oscClient.send("/parameter/NeuralPi/" + parameter, value, function() {
        console.log("OSC value sent");
        parameters[parameter] = value;
    });
}

function getIrs() {
    const result = [];

    fs.readdirSync(irsFolder).forEach(file => {
        const name = path.parse(irsFolder + "/" + file).name
        result.push({ id: name, name: name });
    });

    return result;
}

function getModels() {
    const result = [];

    fs.readdirSync(modelsFolder).forEach(file => {
        const content = fs.readFileSync(modelsFolder + "/" + file);
        const parsed = JSON.parse(content);
        const id = path.parse(modelsFolder + "/" + file).name;
        result.push({ id: id, name: parsed.name || id });
    });

    return result;
}

var bleClient = new BLE(deviceName);
bleClient.start();
bleClient.updateParameterCallback = updateParameter;
bleClient.getIrsCallback = getIrs;
bleClient.getModelsCallback = getModels;


function switchFXChannel(channel, state, notifyOptions) {

}

function switchChannel(channel, notifyOptions) {

}


var footswitch = new Footswitch(serialusb);
footswitch.fxCallback = function(channel, state) { switchFXChannel(channel, state, { notifyFootswitch: false }) };
footswitch.channelCallback = function(channel) { switchChannel(channel, { notifyFootswitch: false }) };
footswitch.channelSaveCallback = function(channel, isFX) {};


server.listen(port, function() {
    console.log('Http server started on port ' + port);
});

app.use(express.json());

app.get("/api/version", function(req, res) {
    res.send(JSON.stringify(version));
});

app.get("/api/name", function(req, res) {
    res.send(JSON.stringify(deviceName));
});

app.post("/api/parameter/:parameter", function(req, res) {
    const parameter = req.params.parameter;
    const value = req.body.value;
    console.log("Http new value for parameter " + parameter + ": ", req.body);
    updateParameter(parameter, value);
    res.send(JSON.stringify("OK"));
});

app.get("/api/parameter/:parameter", function(req, res) {
    const parameter = req.params.parameter;
    const result = {
        value: parameters[parameter]
    };
    res.send(result);
});

app.get("/api/parameter/list/:parameter", function(req, res) {
    const parameter = req.params.parameter;

    if(parameter == "Ir") {
        const result = getIrs();
        res.send(result);
    }

    if(parameter == "Model") {
        const result = getModels();
        res.send(result);
    }
});

app.use(express.static(__dirname + '/../piamp-app/www/'));

app.get("*", function(req, res) {
    res.sendFile(path.join(__dirname + '/../piamp-app/www/index.html'));
});
