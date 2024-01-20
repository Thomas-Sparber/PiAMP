import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { BleClient, BleDevice, numberToUUID } from '@capacitor-community/bluetooth-le';
import { catchError, first, lastValueFrom, map, of } from 'rxjs';
import { IdName } from '../models/id.name';
import { AlertController } from '@ionic/angular';

interface Value {
  value: string | number | undefined;
}

@Injectable({
  providedIn: 'root'
})
export class DataService {

  serviceUUID = "22222222-3333-4444-5555-666666666666";
  connectedDevice?: BleDevice;
  encoder = new TextEncoder();
  decoder = new TextDecoder();
  httpBackend?: Promise<boolean>;

  parameterCharacteristics: { [key: string]: number } = {
    Gain: 0x0000,
    Master: 0x0001,
    Bass: 0x0002,
    Mid: 0x0003,
    Treble: 0x0004,
    Presence: 0x0005,
    Model: 0x0006,
    Ir: 0x0007,
    Delay: 0x0008,
    Reverb: 0x0009,
    Chorus: 0x000C,
    Flanger: 0x000D
  };

  listCharacteristics: { [key: string]: number } = {
    Ir: 0x000A,
    Model: 0x000B
  }

  constructor(
    private http: HttpClient,
    private alertController: AlertController
  ) { }

  isHTTPBackend() {
    if(!this.httpBackend) {
      this.httpBackend = new Promise<boolean>((resolve, reject) => {
        this.http.get("/api/version")
          .pipe(first())
          .pipe(catchError(err => {
            return of(false);
          }))
          .subscribe(result => {
            console.log("Check is http backend: ", result);
            resolve(result !== false);
          });
      });
    }

    return this.httpBackend;
  }

  async isConnected() {
    const http = await this.isHTTPBackend();
    if(http)return true;
    return (this.connectedDevice != undefined);
  }

  async getDeviceName() {
    const http = await this.isHTTPBackend();

    if(http) {
      return lastValueFrom(this.http.get<string>("/api/name"));
    } else {
      return this.connectedDevice?.name!;
    }
  }

  async scanBLEAndConnect() {
    let device = await BleClient.requestDevice({ services: [ this.serviceUUID ] });
    return this.connect(device);
  }

  async connect(device: BleDevice) {
    let connected = false;
    for(let i = 0; i < 5; i++) {
      try {
        await BleClient.connect(device.deviceId, async (deviceId) => {
          console.log("Disconnected");
          this.connectedDevice = undefined;

          const alert = await this.alertController.create({
            header: 'Device disconnected',
            message: device.name + ' disconnected. Please check device',
            buttons: ['OK'],
          });
      
          await alert.present();
        });
        connected = true;
        break;
      } catch (e) {
        console.log("Connection problem retrying...");
      }
    }

    if(!connected) {
      const alert = await this.alertController.create({
        header: 'Unable to connect',
        message: "Unable to connect " + device.name + ' after 5 attempts',
        buttons: ['OK'],
      });
  
      await alert.present();

      throw "Unable to connect after 5 attempts";
    }

    console.log("Connected");
    this.connectedDevice = device;
  }

  async sendParameterValue(parameterId: string, value: string) {
    const http = await this.isHTTPBackend();

    if(http) {
      return this.sendRESTParameterValue(parameterId, value);
    } else {
      return this.sendBLEParameterValue(parameterId, value);
    }
  }

  async sendBLEParameterValue(parameterId: string, value: string) {
    const dv = new DataView(this.encoder.encode(value).buffer);
    const characteristics = this.parameterCharacteristics[parameterId];
    if(!characteristics)console.log("No characteristics found for parameter id " + parameterId, this.parameterCharacteristics);
    await BleClient.write(this.connectedDevice!.deviceId, this.serviceUUID, numberToUUID(characteristics), dv);
  }

  async sendRESTParameterValue(parameterId: string, value: string) {
    return lastValueFrom(this.http.post("/api/parameter/" + parameterId, { value: value }));
  }

  async readParameterValue(parameterId: string) {
    const http = await this.isHTTPBackend();

    if(http) {
      return this.readRESTParameterValue(parameterId);
    } else {
      return this.readBLEParameterValue(parameterId);
    }
  }

  async readBLEParameterValue(parameterId: string) {
    const characteristics = this.parameterCharacteristics[parameterId];
    if(!characteristics)console.log("No characteristics found for parameter id " + parameterId, this.parameterCharacteristics);
    const dv = await BleClient.read(this.connectedDevice!.deviceId, this.serviceUUID, numberToUUID(characteristics));
    return this.decoder.decode(dv);
  }

  async readRESTParameterValue(parameterId: string) {
    return lastValueFrom(this.http.get<Value>("/api/parameter/" + parameterId).pipe(map(v => v.value)));
  }

  async getListData(parameterId: string) {
    const http = await this.isHTTPBackend();

    if(http) {
      return this.getRESTListData(parameterId);
    } else {
      return this.getBLEListData(parameterId);
    }
  }

  async getBLEListData(parameterId: string) {
    const characteristics = this.parameterCharacteristics[parameterId];
    if(!characteristics)console.log("No characteristics found for parameter id " + parameterId, this.parameterCharacteristics);

    let result = "";
    do {
      const dv = await BleClient.read(this.connectedDevice!.deviceId, this.serviceUUID, numberToUUID(characteristics));
      result = result + this.decoder.decode(dv);
    } while(!result.endsWith("\n"));

    return JSON.parse(result) as IdName[];
  }

  async getRESTListData(parameterId: string) {
    return lastValueFrom(this.http.get<IdName[]>("/api/parameter/list/" + parameterId));
  }
  
}
