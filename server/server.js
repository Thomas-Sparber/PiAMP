const HciSocket = require('hci-socket');
const NodeBleHost = require('ble-host');
const express = require('express');
const http = require('http');
const https = require('https');

const BleManager = NodeBleHost.BleManager;
const AdvertisingDataBuilder = NodeBleHost.AdvertisingDataBuilder;
const HciErrors = NodeBleHost.HciErrors;
const AttErrors = NodeBleHost.AttErrors;

const deviceName = 'PiAMP';
const port = 3600;

var transport = new HciSocket(); // connects to the first hci device on the computer, for example hci0

var app = express();
var server = http.createServer(app);

var options = {
    // optional properties go here
};

var callbacks = {
    'help': function(args, callback) {
        var response = JSON.stringify(Object.keys(callbacks));
        callback(response);
    },
    'uirequest': function(args, callback) {
        let parsed;
        try {
            parsed = JSON.parse(args);
        } catch (e) {
            console.log('Unable to parse JSON payload: ', e);
        }

        const httpsCallback = function(res) {
            if (res.statusCode !== 200) {
                console.log("Non success", res);
                callback("");
                res.resume();
                return;
            }

            let data = '';
            res.on('data', function(chunk) {
                data += chunk;
            });

            res.on('close', function() {
                callback(data);
            });
        }

        if(!parsed.method || parsed.method == "GET") {
            http.get(parsed.url, httpsCallback);
        }
    }
};
var currentCallback;

BleManager.create(transport, options, function(err, manager) {
    // err is either null or an Error object
    // if err is null, manager contains a fully initialized BleManager object
    if (err) {
        console.error(err);
        return;
    }
    
    var notificationCharacteristic;
    
    manager.gattDb.setDeviceName(deviceName);
    manager.gattDb.addServices([
        {
            uuid: '22222222-3333-4444-5555-666666666666',
            characteristics: [
                {
                    uuid: '22222222-3333-4444-5555-666666666668',
                    properties: ['read','write'],
                    onRead: function(connection, callback) {
                        if(currentCallback && callbacks[currentCallback.split(' ',2)[0]]) {
                            var splitted = currentCallback.split(' ',2);
                            callbacks[splitted[0]](splitted[1], function(response) { callback(AttErrors.SUCCESS, response) });
                        } else {
                            callback(AttErrors.SUCCESS, "Invalid callback " + currentCallback);
                        }
                    },
                    onWrite: function(connection, needsResponse, value, callback) {
                        console.log('A new value was written:', value.toString());
                        currentCallback = value.toString();
                        callback(AttErrors.SUCCESS); // actually only needs to be called when needsResponse is true
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
    console.log('Http server started');
});

app.use(express.static(__dirname + '/www'));
