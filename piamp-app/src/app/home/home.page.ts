import { Component } from '@angular/core';
import { Platform } from '@ionic/angular';
import { DataService } from '../services/data.service';
import { IdName } from '../models/id.name';

interface UIRequest {
  method?: "GET" | "POST";
  url: string;
  payload?: any;
}

interface Parameter {
  id: string;
  label: string;
  value: number | string;
  type: "slider" | "dropdown"
}

@Component({
  selector: 'app-home',
  templateUrl: 'home.page.html',
  styleUrls: ['home.page.scss'],
})
export class HomePage {
  
  parameters: Parameter[] = [{
    id: "Gain",
    label: "Gain",
    value: 0,
    type: "slider"
  }, {
    id: "Master",
    label: "Master",
    value: 0,
    type: "slider"
  }, {
    id: "Bass",
    label: "Bass",
    value: 0,
    type: "slider"
  }, {
    id: "Mid",
    label: "Mid",
    value: 0,
    type: "slider"
  }, {
    id: "Treble",
    label: "Treble",
    value: 0,
    type: "slider"
  }, {
    id: "Presence",
    label: "Presence",
    value: 0,
    type: "slider"
  }, {
    id: "Model",
    label: "Model",
    value: 0,
    type: "dropdown"
  }, {
    id: "Ir",
    label: "Ir",
    value: 0,
    type: "dropdown"
  }, {
    id: "Reverb",
    label: "Reverb",
    value: 0,
    type: "slider"
  }, {
    id: "Delay",
    label: "Delay",
    value: 0,
    type: "slider"
  }];

  listValues: { [key: string]: Promise<IdName[]> } = {
    Model: this.data.getListData("Model"),
    Ir: this.data.getListData("Ir")
  };

  isHTTPBackend: Promise<boolean>;

  constructor(
    private platform: Platform,
    private data: DataService
  ) {
    this.isHTTPBackend = data.isHTTPBackend();
  }

  async scanBLE() {
    this.data.scanBLEAndConnect();
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
