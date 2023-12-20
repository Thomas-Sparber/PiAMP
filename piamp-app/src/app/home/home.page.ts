import { Component, ElementRef, ViewChild } from '@angular/core';
import { Platform } from '@ionic/angular';
import { BleClient, BleDevice, numberToUUID } from '@capacitor-community/bluetooth-le';

interface UIRequest {
  method?: "GET" | "POST";
  url: string;
  payload?: any;
}

interface Parameter {
  label: string;
  value: number;
  characteristics: number;
}

@Component({
  selector: 'app-home',
  templateUrl: 'home.page.html',
  styleUrls: ['home.page.scss'],
})
export class HomePage {
  
  connectedDevice?: BleDevice;
  serviceUUID = "22222222-3333-4444-5555-666666666666";
  encoder = new TextEncoder();
  decoder = new TextDecoder();
  parameters: Parameter[] = [{
    label: "Gain",
    value: 0,
    characteristics: 0x0000,
  }, {
    label: "Master",
    value: 0,
    characteristics: 0x0001
  }, {
    label: "Bass",
    value: 0,
    characteristics: 0x0002
  }, {
    label: "Mid",
    value: 0,
    characteristics: 0x0003
  }, {
    label: "Treble",
    value: 0,
    characteristics: 0x0004
  }, {
    label: "Presence",
    value: 0,
    characteristics: 0x0005
  }, {
    label: "Model",
    value: 0,
    characteristics: 0x0006
  }, {
    label: "Ir",
    value: 0,
    characteristics: 0x0007
  }, {
    label: "Reverb",
    value: 0,
    characteristics: 0x0008
  }, {
    label: "Delay",
    value: 0,
    characteristics: 0x0009
  }];

  constructor(
    private platform: Platform
  ) {
  }

  async scanBLE() {
    let device: BleDevice;
    try {
      device = await BleClient.requestDevice({ services: [ this.serviceUUID ] });
    } catch (e) {
      console.log("Unable to get device ", e);
      return;
    }

    let connected = false;
    for(let i = 0; i < 5; i++) {
      try {
        await BleClient.connect(device.deviceId, (deviceId) => {
          console.log("Disconnected");
          this.connectedDevice = undefined;
        });
        connected = true;
        break;
      } catch (e) {
        console.log("Connection problem retrying...");
      }
    }

    if(!connected) {
      return;
    }

    console.log("Connected");
    this.connectedDevice = device;
  }

  onValueChanged(ev: Event, parameter: Parameter) {
    const stringValue = "" + (parameter.value / 100);
    const dv = new DataView(this.encoder.encode(stringValue).buffer);
    BleClient.write(this.connectedDevice!.deviceId, this.serviceUUID, numberToUUID(parameter.characteristics), dv);
  }

  async bleUIRequest(r: UIRequest) {
    const dv = new DataView(this.encoder.encode("uirequest " + JSON.stringify(r)).buffer);
    //await BleClient.write(this.connectedDevice!.deviceId, this.serviceUUID, this.characteristicsUUID, dv);
    //const response = await BleClient.read(this.connectedDevice!.deviceId, this.serviceUUID, this.characteristicsUUID);
    //return this.decoder.decode(response);
  }

}
