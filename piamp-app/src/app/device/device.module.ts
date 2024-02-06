import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { IonicModule } from '@ionic/angular';
import { FormsModule } from '@angular/forms';

import { DevicePage } from './device.page';
import { DevicePageRoutingModule } from './device-routing.module';
import { ParameterPage } from '../parameter/parameter.page';

@NgModule({
  imports: [
    CommonModule,
    FormsModule,
    IonicModule,
    DevicePageRoutingModule
  ],
  declarations: [
    DevicePage,
    ParameterPage
  ]
})
export class DevicePageModule {}
