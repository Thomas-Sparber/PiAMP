const HciSocket = require('hci-socket');
const NodeBleHost = require('ble-host');
const express = require('express');
const http = require('http');
const https = require('https');
const osc = require("node-osc");
const fs = require('fs');
const path = require('path');

const BleManager = NodeBleHost.BleManager;
const AdvertisingDataBuilder = NodeBleHost.AdvertisingDataBuilder;
const HciErrors = NodeBleHost.HciErrors;
const AttErrors = NodeBleHost.AttErrors;

const deviceName = 'PiAMP';
const port = 3600;
const oscPort = 24024;
const version = "1.0.0";
const irsFolder = "/home/mind/Documents/GuitarML/NeuralPi/irs";
const modelsFolder = "/home/mind/Documents/GuitarML/NeuralPi/tones";

var oscClient = new osc.Client("127.0.0.1", oscPort);

var transport = new HciSocket(); // connects to the first hci device on the computer, for example hci0

var app = express();
var server = http.createServer(app);

var options = {
    // optional properties go here
};

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

BleManager.create(transport, options, function(err, manager) {
    // err is either null or an Error object
    // if err is null, manager contains a fully initialized BleManager object
    if (err) {
        console.error(err);
        return;
    }
    
    var writeHandler = function(parameter, value, callback) {
        console.log('A new value for ' + parameter + ' was written:', value.toString());
        updateParameter(parameter, value.toString());
        callback(AttErrors.SUCCESS);
    }
    
    var notificationCharacteristic;
    
    manager.gattDb.setDeviceName(deviceName);
    manager.gattDb.addServices([
        {
            uuid: '22222222-3333-4444-5555-666666666666',
            characteristics: [
                {
                    uuid: 0x0000,
                    properties: ['read','write'],
                    onRead: function(connection, callback) {
                        callback(AttErrors.SUCCESS, "Currently not implemented");
                    },
                    onWrite: function(connection, needsResponse, value, callback) { writeHandler("Gain", value, callback); }
                }, {
                    uuid: 0x0001,
                    properties: ['read','write'],
                    onWrite: function(connection, needsResponse, value, callback) { writeHandler("Master", value, callback); }
                }, {
                    uuid: 0x0002,
                    properties: ['read','write'],
                    onWrite: function(connection, needsResponse, value, callback) { writeHandler("Bass", value, callback); }
                }, {
                    uuid: 0x0003,
                    properties: ['read','write'],
                    onWrite: function(connection, needsResponse, value, callback) { writeHandler("Mid", value, callback); }
                }, {
                    uuid: 0x0004,
                    properties: ['read','write'],
                    onWrite: function(connection, needsResponse, value, callback) { writeHandler("Treble", value, callback); }
                }, {
                    uuid: 0x0005,
                    properties: ['read','write'],
                    onWrite: function(connection, needsResponse, value, callback) { writeHandler("Presence", value, callback); }
                }, {
                    uuid: 0x0006,
                    properties: ['read','write'],
                    onWrite: function(connection, needsResponse, value, callback) { writeHandler("Model", value, callback); }
                }, {
                    uuid: 0x0007,
                    properties: ['read','write'],
                    onWrite: function(connection, needsResponse, value, callback) { writeHandler("Ir", value, callback); }
                }, {
                    uuid: 0x0008,
                    properties: ['read','write'],
                    onWrite: function(connection, needsResponse, value, callback) { writeHandler("Reverb", value, callback); }
                }, {
                    uuid: 0x0009,
                    properties: ['read','write'],
                    onWrite: function(connection, needsResponse, value, callback) { writeHandler("Delay", value, callback); }
                }/*,
                notificationCharacteristic = {
                    uuid: '22222222-3333-4444-5555-66666666666A',
                    properties: ['notify'],
                    onSubscriptionChange: function(connection, notification, indication, isWrite) {
                        if (notification) {
                            // Notifications are now enabled, so let's send something
                            notificationCharacteristic.notify(connection, 'Sample notification');
                        }
                    }
                }*/
            ]
        }
    ]);
    
    const advDataBuffer = new AdvertisingDataBuilder()
                            .addFlags(['leGeneralDiscoverableMode', 'brEdrNotSupported'])
                            .addLocalName(/*isComplete*/ true, deviceName)
                            .add128BitServiceUUIDs(/*isComplete*/ true, ['22222222-3333-4444-5555-666666666666'])
                            .build();
    manager.setAdvertisingData(advDataBuffer);
    // call manager.setScanResponseData(...) if scan response data is desired too
    startAdv();

    function startAdv() {
        manager.startAdvertising({/*options*/}, connectCallback);
    }
    
    function connectCallback(status, conn) {
        if (status != HciErrors.SUCCESS) {
            // Advertising could not be started for some controller-specific reason, try again after 10 seconds
            setTimeout(startAdv, 10000);
            return;
        }
        conn.on('disconnect', startAdv); // restart advertising after disconnect
        console.log('Connection established!'/*, conn*/);
    }
});

server.listen(port, function() {
    console.log('Http server started on port ' + port);
});

app.use(express.json());

app.get("/api/version", function(req, res) {
    res.send(JSON.stringify(version));
});

app.post("/api/parameter/:parameter", function(req, res) {
    const parameter = req.params.parameter;
    const value = req.body.value;
    console.log("Http new value for parameter " + parameter + ": ", req.body);
    updateParameter(parameter, value);
    res.send("OK");
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
