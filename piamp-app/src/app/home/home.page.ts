import { Component, ElementRef, ViewChild } from '@angular/core';
import { Platform } from '@ionic/angular';
import { BleClient, BleDevice } from '@capacitor-community/bluetooth-le';

interface UIRequest {
  method?: "GET" | "POST";
  url: string;
  payload?: any;
}

@Component({
  selector: 'app-home',
  templateUrl: 'home.page.html',
  styleUrls: ['home.page.scss'],
})
export class HomePage {
  
  @ViewChild("iframeObj", {static: false}) iframeObj?: ElementRef<HTMLIFrameElement>;
  connectedDevice?: BleDevice;
  serviceUUID = "22222222-3333-4444-5555-666666666666";
  characteristicsUUID = "22222222-3333-4444-5555-666666666668";
  encoder = new TextEncoder();
  decoder = new TextDecoder();

  constructor(
    private platform: Platform
  ) {
    platform.ready().then(() => {
      console.log(this.iframeObj);
    });
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

    this.loadUI();
  }

  async loadUI() {
    const index = await this.bleUIRequest({ url: "http://localhost:3600/index.html" });
    this.iframeObj?.nativeElement.contentWindow?.document.open();
    this.iframeObj?.nativeElement.contentWindow?.document.write(index);
    this.iframeObj?.nativeElement.contentWindow?.document.close();
  }

  async bleUIRequest(r: UIRequest) {
    const dv = new DataView(this.encoder.encode("uirequest " + JSON.stringify(r)).buffer);
    await BleClient.write(this.connectedDevice!.deviceId, this.serviceUUID, this.characteristicsUUID, dv);
    const response = await BleClient.read(this.connectedDevice!.deviceId, this.serviceUUID, this.characteristicsUUID);
    return this.decoder.decode(response);
  }

}
