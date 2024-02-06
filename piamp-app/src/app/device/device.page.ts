import { Component, OnInit } from '@angular/core';
import { DataService } from '../services/data.service';
import { IdName } from '../models/id.name';
import { Parameter } from '../models/parameter';
import { ParameterGroup } from '../models/parameter.group';

@Component({
  selector: 'app-device',
  templateUrl: 'device.page.html',
  styleUrls: ['device.page.scss'],
})
export class DevicePage implements OnInit {

  listValues: { [key: string]: Promise<IdName[]> } = {
    Model: this.data.getListData("Model"),
    Ir: this.data.getListData("Ir")
  };
  
  delayParameter: Parameter =   { id: "Delay",   label: "Delay",   value: 0, type: "slider" };
  chorusParameter: Parameter =  { id: "Chorus",  label: "Chorus",  value: 0, type: "slider" };
  flangerParameter: Parameter = { id: "Flanger", label: "Flanger", value: 0, type: "slider" };
  reverbParameter: Parameter =  { id: "Reverb",  label: "Reverb",  value: 0, type: "slider" };

  parameters: ParameterGroup[] = [
    { name: "Equalizer", parameters: [
      { id: "Gain",     label: "Gain",             value: 50, type: "slider"   },
      { id: "Master",   label: "Master",           value: 50, type: "slider"   },
      { id: "Bass",     label: "Bass",             value: 50, type: "slider"   },
      { id: "Mid",      label: "Mid",              value: 50, type: "slider"   },
      { id: "Treble",   label: "Treble",           value: 50, type: "slider"   },
      { id: "Presence", label: "Presence",         value: 50, type: "slider"   },
    ] },
    { name: "Model", parameters: [
      { id: "Model",    label: "Model",            value: "", type: "dropdown", listValues: this.listValues["Model"] },
      { id: "Ir",       label: "Impulse response", value: "", type: "dropdown", listValues: this.listValues["Ir"]    },
    ] },
    { name: "FX", parameters: [
      this.delayParameter,
      this.chorusParameter,
      this.flangerParameter,
      this.reverbParameter
    ] }
  ];

  advancedParameters: ParameterGroup[] = [
    { name: "Delay", parameters: [
      this.delayParameter,
      { id: "DelayWetLevel",      label: "Wet level",      value: 0,    type: "slider" },
      { id: "DelayTime",          label: "Time",           value: 0,    type: "slider" },
      { id: "DelayFeedback",      label: "Feedback",       value: 0,    type: "slider" }
    ] },
    { name: "Chorus", parameters: [
      this.chorusParameter,
      { id: "ChorusMix",          label: "Mix",            value: 0,    type: "slider" },
      { id: "ChorusRate",         label: "Rate",           value: 0,    type: "slider" },
      { id: "ChorusDepth",        label: "Depth",          value: 0,    type: "slider" },
      { id: "ChorusCentreDelay",  label: "Centre delay",   value: 0,    type: "slider" },
      { id: "ChorusFeedback",     label: "Feedback",       value: 0,    type: "slider" }
    ] },
    { name: "Flanger", parameters: [
      this.flangerParameter,
      { id: "FlangerMix",         label: "Mix",            value: 0,    type: "slider" },
      { id: "FlangerRate",        label: "Rate",           value: 0,    type: "slider" },
      { id: "FlangerDepth",       label: "Depth",          value: 0,    type: "slider" },
      { id: "FlangerCentreDelay", label: "Centre delay",   value: 0,    type: "slider" },
      { id: "FlangerFeedback",    label: "Feedback",       value: 0,    type: "slider" }
    ] },
    { name: "Reverb", parameters: [
      this.reverbParameter,
      { id: "ReverbWetLevel",     label: "Wet level",      value: 0,    type: "slider" },
      { id: "ReverbDamping",      label: "Damping",        value: 0,    type: "slider" },
      { id: "ReverbRoomSize",     label: "Room size",      value: 0,    type: "slider" }
    ] },
    { name: "Amp State", parameters: [
      { id: "AmpState",           label: "Amp enabled",    value: true, type: "switch" },
      { id: "LSTMState",          label: "Neural network", value: true, type: "switch" },
      { id: "IrState",            label: "Ir enabled",     value: true, type: "switch" }
    ] }
  ];

  isHTTPBackend: Promise<boolean>;
  deviceName: Promise<string>;

  constructor(
    private data: DataService
  ) {
    this.isHTTPBackend = data.isHTTPBackend();
    this.deviceName = data.getDeviceName();
  }

  async ngOnInit() {
    for(let i = 0; i < this.parameters.length; i++) {
      for(let j = 0; j < this.parameters[i].parameters.length; j++) {
        const p = this.parameters[i].parameters[j];
        await this.loadParameter(p);
      }
    }

    for(let i = 0; i < this.advancedParameters.length; i++) {
      for(let j = 0; j < this.advancedParameters[i].parameters.length; j++) {
        const p = this.advancedParameters[i].parameters[j];
        await this.loadParameter(p);
      }
    }
  }

  async loadParameter(parameter: Parameter) {
    const v = await this.data.readParameterValue(parameter.id);
    if(v !== undefined) {
      if(parameter.type == "switch") {
        if(v == "true" || v == 1 || v == "1")parameter.value = true;
        else parameter.value = false;
      } else if(parameter.type == "slider") {
        parameter.value = (v as number) * 100;
      } else {
        parameter.value = v;
      }
    }
  }

  async onValueChanged(parameter: Parameter) {
    let stringValue;
    if(parameter.type == "switch") {
      stringValue = parameter.value ? "1" : "0";
    } else if(parameter.type == "slider") {
      stringValue = "" + (parameter.value as number / 100);
    } else {
      stringValue = "" + parameter.value;
    }
    await this.data.sendParameterValue(parameter.id, stringValue);
  }

}
