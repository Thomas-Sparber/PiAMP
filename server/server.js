

const express = require('express');
const http = require('http');
const fs = require('fs');
const path = require('path');

const OSC = require('./osc');
const BLE = require('./ble');
const ImageReader = require('./imagereader');
const SerialUSB = require('./serialusb');
const Footswitch = require('./footswitch');
const Controls = require('./controls');
const PoweroffButton = require('./poweroffbutton');
const ParameterDefinitions = require('./ParameterDefinitions.json');


const deviceName = 'PiAMP';
const port = 3600;
const oscPort = 9001;
const version = "1.0.0";
const irsFolder = "/home/mind/Documents/GuitarML/NeuralPi/irs";
const modelsFolder = "/home/mind/Documents/GuitarML/NeuralPi/tones";
const imageFolder = "/home/mind/Documents/GuitarML/NeuralPi/images";
const channelsSettingsFile = "/home/mind/Documents/piamp-channels.json";

var oscClient = new OSC(oscPort);
oscClient.start();

var app = express();
var server = http.createServer(app);

var serialusb = new SerialUSB();
serialusb.receivedCallbacks.push(function(data) {
    if(data && data.action == "log") {
        console.log("Serial data received: ", data);
    }
});
serialusb.start();

var imageReader = new ImageReader(imageFolder);

var controls = new Controls(serialusb);
controls.getImageCallback = function(name, x, y, w, h) {
    return imageReader.getImageData(name, x, y, w, h);
}
controls.getCallbacks["Ir"] = getIrs;
controls.getCallbacks["Model"] = getModels;
controls.relevantParameters.forEach(function(param) {
    controls.paramChangedCallbacks[param] = function(value) {
        updateParameter(param, value, { notifyControls: false });
    }
});


var tempParameters = {};
var parameters = {};
var channels = {};

function updateParameter(parameter, value, notifyOptions) {
    if(parseFloat(value) == value) {
        value = parseFloat(value);
    }

    oscClient.send("/parameter/NeuralPi/" + parameter, value, function() {
        console.log("OSC " + parameter + " sent: " + value);
        parameters[parameter] = value;
    });

    if(!channels["current"]) {
        channels["current"] = {};
    }

    channels["current"][parameter] = value;
    saveChannels();

    if(!notifyOptions || notifyOptions.notifyControls !== false) {
       controls.setParameter(parameter, value);
    }
}

function getIrs() {
    const result = [];

    fs.readdirSync(irsFolder).forEach(file => {
        if(file.toLowerCase().endsWith("wav")) {
            const id = path.parse(irsFolder + "/" + file).name;
            let name = id;
            let image = id;

            const jsonFile = irsFolder + "/" + id + ".json";
            if(fs.existsSync(jsonFile)) {
                try {
                    const content = fs.readFileSync(jsonFile);
                    const parsed = JSON.parse(content);
                    if(parsed.name)name = parsed.name;
                    if(parsed.image)image = parsed.image;
                } catch(e) {
                    console.log("Error parsing file " + jsonFile, e);
                }
            }

            result.push({
                id: id,
                name: name,
                image: image
            });
        }
    });

    result.sort(function(a, b) {
        return a.name.toLowerCase().localeCompare(b.name.toLowerCase());
    });

    return result;
}

function getModels() {
    const result = [];

    fs.readdirSync(modelsFolder).forEach(file => {
        if(file.toLowerCase().endsWith("json")) {
            try {
                const content = fs.readFileSync(modelsFolder + "/" + file);
                const parsed = JSON.parse(content);
                const id = path.parse(modelsFolder + "/" + file).name;

                result.push({
                    id: id,
                    name: parsed.name || id,
                    image: parsed.image || id
                });
            } catch (e) {
                console.log("Error parsing file " + modelsFolder + "/" + file, e);
            }
        }
    });

    result.sort(function(a, b) {
        return a.name.toLowerCase().localeCompare(b.name.toLowerCase());
    });

    return result;
}

var bleClient = new BLE(deviceName);
bleClient.start();
bleClient.getParameterCallback = function(parameter) { return parameters[parameter]; };
bleClient.updateParameterCallback = function(parameter, value) { updateParameter(parameter, value, { notifyBle: false }); };
bleClient.getCallbacks["Ir"] = getIrs;
bleClient.getCallbacks["Model"] = getModels;


function switchFXChannel(channel, state, notifyOptions) {
    console.log("Switching FX channel " + state);

    tempParameters = JSON.parse(JSON.stringify(parameters));

    if(channels[channel]) {
        Object.keys(channels[channel]).forEach(function(parameter) {
            if(state == "on") {
                updateParameter(parameter, channels[channel][parameter]);
            } else {
                updateParameter(parameter, 0);
            }
        });
    }
}

function switchChannel(channel, notifyOptions) {
    console.log("Switching to channel " + channel, channels[channel]);

    tempParameters = JSON.parse(JSON.stringify(parameters));

    if(channels[channel]) {
        Object.keys(channels[channel]).forEach(function(parameter) {
            tempParameters[parameter] = parameters[parameter];
            updateParameter(parameter, channels[channel][parameter]);
        });
    }
}

function saveChannel(channel, isFX, notifyOptions) {
    console.log("Saving channel " + channel + ", FX: " + isFX);

    channels[channel] = {};

    Object.keys(parameters).forEach(function(parameter) {
        var fx = isFX && ParameterDefinitions[parameter] && ParameterDefinitions[parameter].isFX;
        var ch = !isFX && ParameterDefinitions[parameter] && ParameterDefinitions[parameter].isChannel;

        if((fx || ch) && parameters[parameter]) {
            channels[channel][parameter] = tempParameters[parameter];
        }
    });

    saveChannels();
}

var saveTimer;

function saveChannels() {
    clearTimeout(saveTimer);

    saveTimer = setTimeout(function() {
        var content = JSON.stringify(channels);
        fs.writeFileSync(channelsSettingsFile, content, 'utf-8');
    }, 1000);
}

function loadChannels() {
    if(fs.existsSync(channelsSettingsFile)) {
        var content = fs.readFileSync(channelsSettingsFile);

        try {
            channels = JSON.parse(content);

            if(channels["current"]) {
                Object.keys(channels["current"]).forEach(function(parameter) {
                    updateParameter(parameter, channels["current"][parameter]);
                });
            }
        } catch(e) {
            console.log("Error loading channels", e);
        }
    }
}

loadChannels();

var footswitch = new Footswitch(serialusb);
footswitch.start();
footswitch.fxCallback = function(channel, state) { switchFXChannel(channel, state, { notifyFootswitch: false }); };
footswitch.channelCallback = function(channel) { switchChannel(channel, { notifyFootswitch: false }); };
footswitch.channelSaveCallback = function(channel, isFX) { saveChannel(channel, isFX, { notifyFootswitch: false }); };


controls.start();



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
    updateParameter(parameter, value, { notifyWeb: false });
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

const poweroffbutton = new PoweroffButton();
poweroffbutton.watch(function() {
    console.log("Poweroff button pressed, bye bye");
    poweroffbutton.poweroff();
});
