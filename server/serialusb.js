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

                    port.serialport.on('error', function(err) {
                        console.log('Serial port error: ', err.message);
                    });

                    self.ports.push(port);

                    let helper = "";
                    port.serialport.on('data', function(data){
                        helper += data;

                        const index = helper.indexOf("\n");
                        if(index != -1) {
                            let received = helper.substring(0, index);
                            helper = helper.substring(index + 1);

                            if(received != port.lastWritten) {
                                try {
                                    received = JSON.parse(received);
                                } catch(e) {
                                    console.log("Invalid JSON: " + received, e);
                                }

                                self.receivedCallbacks.forEach(function(callback) {
                                    callback(received, port, function(toSend, format) {
                                        if(!format || format == "json") {
                                            toSend = JSON.stringify(toSend) + "\n";
                                        }
                                        else if(format == "string") {
                                            toSend = toSend + "\n";
                                        }
                                        else if(format == "binary") {
                                            //don't do anything
                                        }

                                        port.lastWritten = toSend;
                                        port.serialport.write(toSend, function(er) {
                                            if(err) {
                                                console.log("Error writing to serial port", err);
                                            }
                                        });
                                        //console.log("Sent", toSend);
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
