const Gpio = require("onoff").Gpio;
const { exec } = require('child_process');

class PoweroffButton {

    watch(callback) {
        const button = new Gpio(4, 'in', 'rising', { debounceTimeout: 10  });
        button.watch(function(err, value) {
            if(err) {
                console.log("Error watching poweroff button", err);
                return;
            }

            callback();
        });
    }

    poweroff() {
        exec('/sbin/shutdown -h now');
    }

}

module.exports = PoweroffButton;
