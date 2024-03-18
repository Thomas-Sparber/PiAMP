

class Controls {

    serialUSB;
    paramChangedCallback;
    getImageCallback;
    currentModel = 0;
    getCallbacks;

    constructor(serialUSB) {
        this.serialUSB = serialUSB;
        this.getCallbacks = {};
    }

    start() {
        var self = this;

        this.serialUSB.receivedCallbacks.push(function(data, port, response) {
            if(data.action == "getImage") {
                var imageData = self.getImageCallback(data.image, data.x, data.y, data.w, data.h);

                response(imageData, "binary");
            }

            if(data.action == "getmodel") {
                const models = self.getCallbacks["Model"]();
                if(!models || models.length == 0)return;
                if(self.currentModel >= models.length)self.currentModel = 0;
                if(self.currentModel < 0) self.currentModel = models.length - 1;
                response({ "action": "model", "model": models[self.currentModel].id, "name": models[self.currentModel].name });
            }

            if(data.action == "modelchange") {
                const models = self.getCallbacks["Model"]();
                if(!models || models.length == 0)return;
                self.currentModel += data.value;
                if(self.currentModel >= models.length)self.currentModel = 0;
                if(self.currentModel < 0) self.currentModel = models.length - 1;
                response({ "action": "model", "model": models[self.currentModel].id, "name": models[self.currentModel].name });
            }

            if(data.action == "ping") {
                response({ action: "pong" });
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

}

module.exports = Controls;
