

class Controls {

    serialUSB;
    paramChangedCallbacks;
    getImageCallback;
    currentModel = 0;
    currentIr = 0;
    getCallbacks;
    updateParameterQueue;
    updateTimer;

    constructor(serialUSB) {
        this.serialUSB = serialUSB;
        this.getCallbacks = {};
        this.paramChangedCallbacks = {};
        this.updateParameterQueue = [];
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
                if(self.currentModel < 0)self.currentModel = models.length - 1;
                response({ "action": "model", "id": models[self.currentModel].id, "name": models[self.currentModel].name, "image": models[self.currentModel].image });
            }

            if(data.action == "getir") {
                const irs = self.getCallbacks["Ir"]();
                if(!irs || irs.length == 0)return;
                if(self.currentIr >= irs.length)self.currentIr = 0;
                if(self.currentIr < 0)self.currentIr = irs.length - 1;
                response({ "action": "ir", "id": irs[self.currentIr].id, "name": irs[self.currentIr].name, "image": irs[self.currentIr].image });
            }

            if(data.action == "modelchange") {
                const models = self.getCallbacks["Model"]();
                if(!models || models.length == 0)return;
                if(data.model) {
                    const newIndex = models.findIndex(function(model) { return model.id == data.model; });
                    if(newIndex != -1)self.currentModel = newIndex;
                    else console.log("Requested modelr " + data.model + " not found");
                } else if(data.value) {
                    self.currentModel += data.value;
                    if(self.currentModel >= models.length)self.currentModel = 0;
                    if(self.currentModel < 0) self.currentModel = models.length - 1;
                }
                self.paramChangedCallbacks["Model"](models[self.currentModel].id);
                response({ "action": "model", "id": models[self.currentModel].id, "name": models[self.currentModel].name, "image": models[self.currentModel].image });
            }

            if(data.action == "irchange") {
                const irs = self.getCallbacks["Ir"]();
                if(!irs || irs.length == 0)return;
                if(data.ir) {
                    const newIndex = irs.findIndex(function(ir) { return ir.id == data.ir; });
                    if(newIndex != -1)self.currentIr = newIndex;
                    else console.log("Requested ir " + data.ir + " not found");
                } else {
                    self.currentIr += data.value;
                    if(self.currentIr >= irs.length)self.currentIr = 0;
                    if(self.currentIr < 0)self.currentIr = irs.length - 1;
                }
                self.paramChangedCallbacks["Ir"](irs[self.currentIr].id);
                response({ "action": "ir", "id": irs[self.currentIr].id, "name": irs[self.currentIr].name, "image": irs[self.currentIr].image });
            }

            if(data.action == "getmodels") {
                const models = self.getCallbacks["Model"]();
                if(data.id !== undefined || data.currentSelection !== undefined) {
                    const index = data.currentSelection || models.findIndex(function(model) { return model.id == data.id; });
                    const startIndex = Math.floor(index / data.rowsPerPage) * data.rowsPerPage;
                    const numModels = Math.min(data.rowsPerPage, models.length - startIndex);
                    const result = {
                        currentSelection: index,
                        startIndex: startIndex,
                        size: models.length,
                        numModels: numModels
                    };
                    const toSend = [];
                    for(let i = 0; i < numModels; i++)toSend.push(models[startIndex + i]);
                    response(JSON.stringify(result) + "\n" + toSend.map(function(model) { return model.id + "\n" + model.name }).join("\n"), "string");
                } else {
                    response(models.length + "\n" + models.map(function(model) { return model.id + "\n" + model.name }).join("\n"), "string");
                }
            }

            if(data.action == "getirs") {
                const irs = self.getCallbacks["Ir"]();
                if(data.id !== undefined || data.currentSelection !== undefined) {
                    const index = data.currentSelection || irs.findIndex(function(ir) { return ir.id == data.id; });
                    const startIndex = Math.floor(index / data.rowsPerPage) * data.rowsPerPage;
                    const numModels = Math.min(data.rowsPerPage, irs.length - startIndex);
                    const result = {
                        currentSelection: index,
                        startIndex: startIndex,
                        size: irs.length,
                        numModels: numModels
                    };
                    const toSend = [];
                    for(let i = 0; i < numModels; i++)toSend.push(irs[startIndex + i]);
                    response(JSON.stringify(result) + "\n" + toSend.map(function(ir) { return ir.id + "\n" + ir.name }).join("\n"), "string");
                } else {
                  response(irs.length + "\n" + irs.map(function(ir) { return ir.id + "\n" + ir.name }).join("\n"), "string");
                }
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

    setParameter(parameter, value) {
        this.updateParameterQueue.push({ parameter: parameter, value: value });

        var self = this;
        clearTimeout(this.updateTimer);
        this.updateTimer = setTimeout(function() { self.sendParametersInternal(); }, 0);
    }

    sendParametersInternal() {
        var self = this;

        this.updateParameterQueue.forEach(function(p) {
            var parameter = p.parameter;
            var value = p.value;

            if(parameter == "Model") {
                const models = self.getCallbacks["Model"]();
                if(!models || models.length == 0)return;
                self.currentModel = models.findIndex(function(m) { return m.id == value; });
                self.serialUSB.send({ "action": "model", "id": models[self.currentModel].id, "name": models[self.currentModel].name, "image": models[self.currentModel].image });
                console.log("Model sent to controls: " + models[self.currentModel].id);
            } else if(parameter == "Ir") {
                const irs = self.getCallbacks["Ir"]();
                if(!irs || irs.length == 0)return;
                self.currentIr = irs.findIndex(function(i) { return i.id == value });
                self.serialUSB.send({ "action": "ir", "id": irs[self.currentIr].id, "name": irs[self.currentIr].name, "image": irs[self.currentIr].image });
                console.log("Ir sent to controls: " + irs[self.currentIr].id);
            } else {
                self.serialUSB.send({ "action": parameter, "value": value });
                console.log(parameter + " sent to controls: " + value);
            }
        });

        this.updateParameterQueue = [];
    }

}

module.exports = Controls;
