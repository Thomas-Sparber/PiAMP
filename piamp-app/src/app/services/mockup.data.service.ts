import { Injectable } from '@angular/core';
import { IdName } from '../models/id.name';
import { DataService } from './data.service';

@Injectable()
export class MockupDataService extends DataService {

  connected = false;
  parameters: { [key: string]: string | number | undefined } = {};

  isHTTPBackend() {
    return Promise.resolve(false);
  }

  isConnected() {
    return Promise.resolve(this.connected);
  }
  
  scanBLEAndConnect() {
    this.connected = true;
    return Promise.resolve();
  }

  getDeviceName() {
    return Promise.resolve("Mockup device");
  }

  sendParameterValue(parameterId: string, value: string) {
    this.parameters[parameterId] = value;
    return Promise.resolve();
  }

  readParameterValue(parameterId: string) {
    return Promise.resolve(this.parameters[parameterId]);
  }

  getListData(parameterId: string) {
    return Promise.resolve([{
      id: "test1",
      name: "Test 1"
    }, {
      id: "test2",
      name: "Test 2"
    }]);
  }

}
