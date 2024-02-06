

class Footswitch {

    serialUSB;
    fxCallback;
    channelCallback;
    channelSaveCallback;

    constructor(serialUSB) {
        this.serialUSB = serialUSB;
    }

    start() {
        var self = this;

        this.serialUSB.receivedCallbacks.push(function(data, port, response) {
            if(data.action == "fxon") {
                self.fxCallback(data.channel, "on");
            }
            if(data.action == "fxoff") {
                self.fxCallback(data.channel, "off");
            }
            if(data.action == "channelswitch") {
                self.channelCallback(data.channel);
            }
            if(data.action == "channelsave") {
                self.channelSaveCallback(data.channel, data.isFX);
            }
        });
    }

    switchFX(channel, state) {
        const action = (state == "off") ? "fxoff" : "fxon";

        this.serialUSB.send({
            action: action,
            channel: channel
        });
    }

    switchChannel(channel) {
        this.serialUSB.send({
            action: "channelswitch",
            channel: channel
        });
    }

}

module.exports = Footswitch;
