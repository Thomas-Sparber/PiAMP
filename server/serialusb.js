const serialport = require("serialport");
const bindings = require('@serialport/bindings')

class SerialUSB {

    ports;
    receivedCallbacks;

    constructor() {
        this.receivedCallbacks = [];
    }

    start() {
        this.ports = [];

        var self = this;

        bindings.list().then(function(results, err) {
            if(err) {
                console.log("Error listing serial ports");
            }

            results.forEach(function(result) {
                if(result.path.indexOf("ttyUSB") != -1) {
                    const port = {
                        serialport: new serialport.SerialPort({
                            path: result.path,
                            baudRate: 115200
                        }, function(err) {
                            if(!err) {
                                console.log("Serial port opened: ", result.path);
                            } else {
                                console.log("Error opening serial port: ", err);
                            }
                        }),
                        path: result.path,
                        lastWritten: ""
                    }

                    self.ports.push(port);

                    let helper = "";
                    port.serialport.on('data', function(data){
                        helper += data;

                        if(helper.endsWith("\n")) {
                            if(helper != port.lastWritten) {
                                receivedCallbacks.forEach(function(callback) {
                                    callback(JSON.parse(helper), port, function(toSend) {
                                        toSend = JSON.stringify(toSend) + "\n";
                                        port.lastWritten = toSend;
                                        port.serialport.write(toSend);
                                    });
                                });
                            }
                        }
                    });

                }
            });
        });
    }

    send(data) {
        data = JSON.stringify(data) + "\n";

        this.ports.forEach(function(port) {
            port.lastWritten = data;
            port.serialport.write(data);
        });
    }

}

module.exports = SerialUSB;
