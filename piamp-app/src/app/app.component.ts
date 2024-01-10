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

    try {
      await BleClient.initialize(/*{ androidNeverForLocation: true }*/);
    } catch(e) {
      console.log("Bluetooth not available");
    }
  }
  
}
