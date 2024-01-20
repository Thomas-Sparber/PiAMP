import { Component, OnInit } from '@angular/core';
import { Platform } from '@ionic/angular';
import { DataService } from '../services/data.service';
import { IdName } from '../models/id.name';

interface Parameter {
  id: string;
  label: string;
  value: number | string;
  type: "slider" | "dropdown"
}

@Component({
  selector: 'app-device',
  templateUrl: 'device.page.html',
  styleUrls: ['device.page.scss'],
})
export class DevicePage implements OnInit {
  
  parameters: Parameter[] = [
    { id: "Gain",     label: "Gain",     value: 0,  type: "slider"   },
    { id: "Master",   label: "Master",   value: 0,  type: "slider"   },
    { id: "Bass",     label: "Bass",     value: 0,  type: "slider"   },
    { id: "Mid",      label: "Mid",      value: 0,  type: "slider"   },
    { id: "Treble",   label: "Treble",   value: 0,  type: "slider"   },
    { id: "Presence", label: "Presence", value: 0,  type: "slider"   },
    { id: "Model",    label: "Model",    value: "", type: "dropdown" },
    { id: "Ir",       label: "Ir",       value: "", type: "dropdown" },
    { id: "Reverb",   label: "Reverb",   value: 0,  type: "slider"   },
    { id: "Delay",    label: "Delay",    value: 0,  type: "slider"   },
    { id: "Chorus",   label: "Chorus",   value: 0,  type: "slider"   },
    { id: "Flanger",  label: "Flanger",  value: 0,  type: "slider"   }
  ];

  listValues: { [key: string]: Promise<IdName[]> } = {
    Model: this.data.getListData("Model"),
    Ir: this.data.getListData("Ir")
  };

  isHTTPBackend: Promise<boolean>;
  deviceName: Promise<string>;

  constructor(
    private platform: Platform,
    private data: DataService
  ) {
    this.isHTTPBackend = data.isHTTPBackend();
    this.deviceName = data.getDeviceName();
  }

  async ngOnInit() {
    for(let i = 0; i < this.parameters.length; i++) {
      const v = await this.data.readParameterValue(this.parameters[i].id);
      if(v !== undefined) {
        if(!isNaN(v as number)) {
          this.parameters[i].value = (v as number) * 100;
        } else {
          this.parameters[i].value = v;
        }
      }
    }
  }

  async onValueChanged(ev: Event, parameter: Parameter) {
    let stringValue;
    if(isNaN(parameter.value as number)) {
      stringValue = "" + parameter.value;
    } else {
      stringValue = "" + (parameter.value as number / 100);
    }
    await this.data.sendParameterValue(parameter.id, stringValue);
  }

}
