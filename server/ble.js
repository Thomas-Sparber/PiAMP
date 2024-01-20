const NodeBleHost = require('ble-host');
const HciSocket = require('hci-socket');

const BleManager = NodeBleHost.BleManager;
const AdvertisingDataBuilder = NodeBleHost.AdvertisingDataBuilder;
const HciErrors = NodeBleHost.HciErrors;
const AttErrors = NodeBleHost.AttErrors;

class BLE {

    deviceName;
    updateParameterCallback;
    getIrsCallback;
    getModelsCallback;

    constructor(deviceName) {
        this.deviceName = deviceName;
    }

    start() {
        var transport = new HciSocket(); // connects to the first hci device on the computer, for example hci0

        var options = {
            // optional properties go here
        };

        var self = this;

        BleManager.create(transport, options, function(err, manager) {
            // err is either null or an Error object
            // if err is null, manager contains a fully initialized BleManager object
            if (err) {
                console.error(err);
                return;
            }
            
            var writeHandler = function(parameter, value, callback) {
                console.log('A new value for ' + parameter + ' was written:', value.toString());
                self.updateParameterCallback(parameter, value.toString());
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
            
            manager.gattDb.setDeviceName(self.deviceName);
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
                            uuid: 0x000C,
                            properties: ['read','write'],
                            onWrite: function(connection, needsResponse, value, callback) { writeHandler("Chorus", value, callback); },
                            onRead: function(connection, callback) { readHandler("Chorus", callback); }
                        }, {
                            uuid: 0x000D,
                            properties: ['read','write'],
                            onWrite: function(connection, needsResponse, value, callback) { writeHandler("Flanger", value, callback); },
                            onRead: function(connection, callback) { readHandler("Flanger", callback); }
                        }, {
                            uuid: 0x000A,
                            properties: ['read'],
                            onRead: function(connection, callback) {
                                if(!writeCache["Ir"]) {
                                    const result = self.getIrsCallback();
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
                                    const result = self.getModelsCallback();
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
                                    .addLocalName(/*isComplete*/ true, self.deviceName)
                                    .add128BitServiceUUIDs(/*isComplete*/ true, ['22222222-3333-4444-5555-666666666666'])
                                    .build();
            manager.setAdvertisingData(advDataBuffer);
            // call manager.setScanResponseData(...) if scan response data is desired too
            startAdv();
        
            function startAdv() {
                console.log("Start BLE advertising " + self.deviceName);
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
    }

}

module.exports.BLE = BLE;
