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

    var readHandler = function(parameter, callback) {
        const result = {
            value: parameters[parameter]
        };

        callback(AttErrors.SUCCESS, JSON.stringify(result));
    }
    
    var notificationCharacteristic;

    var writeCache = {};
    
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
                    onWrite: function(connection, needsResponse, value, callback) { writeHandler("Gain", value, callback); },
                    onRead: function(connection, callback) { readHandler("Gain", callback); }
                }, {
                    uuid: 0x0001,
                    properties: ['read','write'],
                    onWrite: function(connection, needsResponse, value, callback) { writeHandler("Master", value, callback); },
                    onRead: function(connection, callback) { readHandler("Master", callback); }
                }, {
                    uuid: 0x0002,
                    properties: ['read','write'],
                    onWrite: function(connection, needsResponse, value, callback) { writeHandler("Bass", value, callback); },
                    onRead: function(connection, callback) { readHandler("Bass", callback); }
                }, {
                    uuid: 0x0003,
                    properties: ['read','write'],
                    onWrite: function(connection, needsResponse, value, callback) { writeHandler("Mid", value, callback); },
                    onRead: function(connection, callback) { readHandler("Mid", callback); }
                }, {
                    uuid: 0x0004,
                    properties: ['read','write'],
                    onWrite: function(connection, needsResponse, value, callback) { writeHandler("Treble", value, callback); },
                    onRead: function(connection, callback) { readHandler("Treble", callback); }
                }, {
                    uuid: 0x0005,
                    properties: ['read','write'],
                    onWrite: function(connection, needsResponse, value, callback) { writeHandler("Presence", value, callback); },
                    onRead: function(connection, callback) { readHandler("Presence", callback); }
                }, {
                    uuid: 0x0006,
                    properties: ['read','write'],
                    onWrite: function(connection, needsResponse, value, callback) { writeHandler("Model", value, callback); },
                    onRead: function(connection, callback) { readHandler("Model", callback); }
                }, {
                    uuid: 0x0007,
                    properties: ['read','write'],
                    onWrite: function(connection, needsResponse, value, callback) { writeHandler("Ir", value, callback); },
                    onRead: function(connection, callback) { readHandler("Ir", callback); }
                }, {
                    uuid: 0x0008,
                    properties: ['read','write'],
                    onWrite: function(connection, needsResponse, value, callback) { writeHandler("Reverb", value, callback); },
                    onRead: function(connection, callback) { readHandler("Reverb", callback); }
                }, {
                    uuid: 0x0009,
                    properties: ['read','write'],
                    onWrite: function(connection, needsResponse, value, callback) { writeHandler("Delay", value, callback); },
                    onRead: function(connection, callback) { readHandler("Delay", callback); }
                }, {
                    uuid: 0x000A,
                    properties: ['read'],
                    onRead: function(connection, callback) {
                        if(!writeCache["Ir"]) {
                            const result = getIrs();
                            writeCache["Ir"] = JSON.stringify(result) + "\n";
                        }

                        let sent = false;
                        let divide = 1;
                        while(!sent) {
                            const temp = writeCache["Ir"].substring(0, writeCache["Ir"].length / divide);
                            try {
                                callback(AttErrors.SUCCESS, temp);
                                sent = true;
                                writeCache["Ir"] = writeCache["Ir"].substring(writeCache["Ir"].length / divide);
                            } catch(e) {}
                        }
                    }
                }, {
                    uuid: 0x000B,
                    properties: ['read'],
                    onRead: function(connection, callback) {
                        if(!writeCache["Model"]) {
                            const result = getModels();
                            writeCache["Model"] = JSON.stringify(result) + "\n";
                        }

                        let sent = false;
                        let divide = 1;
                        while(!sent) {
                            const temp = writeCache["Model"].substring(0, writeCache["Model"].length / divide);
                            try {
                                callback(AttErrors.SUCCESS, temp);
                                sent = true;
                                writeCache["Model"] = writeCache["Model"].substring(writeCache["Model"].length / divide);
                            } catch(e) {}
                        }
                    }
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
