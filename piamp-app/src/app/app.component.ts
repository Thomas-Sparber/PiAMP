import { Component } from '@angular/core';
import { Platform } from '@ionic/angular';

import { BleClient } from '@capacitor-community/bluetooth-le';

@Component({
  selector: 'app-root',
  templateUrl: 'app.component.html',
  styleUrls: ['app.component.scss'],
})
export class AppComponent {

  constructor(
    private platform: Platform
  ) {
    this.init();
  }

  async init() {
    console.log("App started");

    await this.platform.ready();

    console.log("Platform ready");
    await BleClient.initialize(/*{ androidNeverForLocation: true }*/);

    /*console.log("Scanning for device");
    const device = await BleClient.requestDevice({ services: [ "22222222-3333-4444-5555-666666666666" ] });

    console.log("Connecting to", device);

    for(let i = 0; i < 5; i++) {
      try {
        await BleClient.connect(device.deviceId, (deviceId) => {
          console.log("Disconnected");
        });
        break;
      } catch (e) {
        console.log("Connection problem retrying...");
      }
    }

    console.log("Connected");

    const services = await BleClient.getServices(device.deviceId);
    console.log("Services:", services);

    const encoder = new TextEncoder();
    const dv = new DataView(encoder.encode("help").buffer);
    await BleClient.write(device.deviceId, "22222222-3333-4444-5555-666666666666", "22222222-3333-4444-5555-666666666668", dv);
  
    console.log("Written");

    const response = await BleClient.read(device.deviceId, "22222222-3333-4444-5555-666666666666", "22222222-3333-4444-5555-666666666668");

    console.log("read", response);*/
  }
  
}
