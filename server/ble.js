const NodeBleHost = require('ble-host');
const HciSocket = require('hci-socket');

const BleManager = NodeBleHost.BleManager;
const AdvertisingDataBuilder = NodeBleHost.AdvertisingDataBuilder;
const HciErrors = NodeBleHost.HciErrors;
const AttErrors = NodeBleHost.AttErrors;

class BLE {

    deviceName;
    getParameterCallback;
    updateParameterCallback;
    getCallbacks;

    parameterCharacteristics = {
        Gain: 0x0000,
        Master: 0x0001,
        Bass: 0x0002,
        Mid: 0x0003,
        Treble: 0x0004,
        Presence: 0x0005,
        Model: 0x0006,
        Ir: 0x0007,
        Delay: 0x0008,
        Reverb: 0x0009,
        Chorus: 0x000C,
        Flanger: 0x000D,
        DelayWetLevel: 0x000E,
        DelayTime: 0x000F,
        DelayFeedback: 0x0010,
        ChorusMix: 0x0011,
        ChorusRate: 0x0012,
        ChorusDepth: 0x0013,
        ChorusCentreDelay: 0x0014,
        ChorusFeedback: 0x0015,
        FlangerMix: 0x0016,
        FlangerRate: 0x0017,
        FlangerDepth: 0x0018,
        FlangerCentreDelay: 0x0019,
        FlangerFeedback: 0x001A,
        ReverbWetLevel: 0x001B,
        ReverbDamping: 0x001C,
        ReverbRoomSize: 0x001D,
        AmpState: 0x001E,
        LSTMState: 0x001F,
        IrState: 0x0020
    };

    listCharacteristics = {
        Ir: 0x000A,
        Model: 0x000B
    }

    constructor(deviceName) {
        this.deviceName = deviceName;
        this.getCallbacks = {};
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
                    value: self.getParameterCallback(parameter)
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
                        ...Object.keys(self.parameterCharacteristics).map(function(ch) {
                            return {
                                uuid: self.parameterCharacteristics[ch],
                                properties: ['read','write'],
                                onWrite: function(connection, needsResponse, value, callback) { writeHandler(ch, value, callback); },
                                onRead: function(connection, callback) {
                                    //console.log("Read " + self.parameterCharacteristics[ch]);
                                    readHandler(ch, callback);
                                }
                            }
                        }),
                        ...Object.keys(self.listCharacteristics).map(function(ch) {
                            return {
                                uuid: self.listCharacteristics[ch],
                                properties: ['read'],
                                onRead: function(connection, callback) {
                                    //console.log("Read " + self.listCharacteristics[ch]);

                                    if(!writeCache[ch]) {
                                        const result = self.getCallbacks[ch]();
                                        writeCache[ch] = JSON.stringify(result) + "\n";

                                    }
            
                                    let sent = false;
                                    let divide = 1;
                                    while(!sent) {
                                        const temp = writeCache[ch].substring(0, writeCache[ch].length / divide);
                                        try {
                                            callback(AttErrors.SUCCESS, temp);
                                            sent = true;
                                            writeCache[ch] = writeCache[ch].substring(writeCache[ch].length / divide);
                                        } catch(e) {
                                            divide++;
                                        }
                                    }
                                }
                            }
                        })/*,
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
                console.log('BLE Connection established!'/*, conn*/);
            }
        });
    }

}

module.exports = BLE;
