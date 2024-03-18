import { Injectable } from '@angular/core';
import { IdName } from '../models/id.name';

@Injectable()
export abstract class DataService {

  abstract isHTTPBackend() : Promise<boolean>;

  abstract isConnected() : Promise<boolean>;

  abstract scanBLEAndConnect() : Promise<void>;

  abstract getDeviceName(): Promise<string>;

  abstract sendParameterValue(parameterId: string, value: string) : Promise<void>;

  abstract readParameterValue(parameterId: string) : Promise<string | number | undefined>;

  abstract getListData(parameterId: string) : Promise<IdName[]>;

}
