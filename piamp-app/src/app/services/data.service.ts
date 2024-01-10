import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { BleClient, BleDevice, numberToUUID } from '@capacitor-community/bluetooth-le';
import { catchError, first, lastValueFrom, of } from 'rxjs';
import { IdName } from '../models/id.name';

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
    Deplay: 0x0008,
    Reverb: 0x0009
  };

  listCharacteristics: { [key: string]: number } = {
    Ir: 0x000A,
    Model: 0x000b
  }

  constructor(
    private http: HttpClient
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

  async scanBLEAndConnect() {
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
    return lastValueFrom(this.http.post("/api/parameter/" + parameterId, value));
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
      const r = await BleClient.read(this.connectedDevice!.deviceId, this.serviceUUID, numberToUUID(characteristics));
      result = result + r;
    } while(!result.endsWith("\n"));

    return JSON.parse(result) as IdName[];
  }

  async getRESTListData(parameterId: string) {
    return lastValueFrom(this.http.get<IdName[]>("/api/parameter/list/" + parameterId));
  }
  
}
