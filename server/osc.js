const osc = require("node-osc");

class OSC {

    port;
    oscClient;

    constructor(port) {
        this.port = port;
    }

    start() {
        this.oscClient = new osc.Client("127.0.0.1", this.port);
    }

    send(path, value, callback) {
        this.oscClient.send(path, value, callback);
    }

}

module.exports.OSC = OSC;
