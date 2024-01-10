import { Component } from '@angular/core';
import { Platform } from '@ionic/angular';
import { DataService } from '../services/data.service';
import { IdName } from '../models/id.name';
import { Router } from '@angular/router';

@Component({
  selector: 'app-home',
  templateUrl: 'home.page.html',
  styleUrls: ['home.page.scss'],
})
export class HomePage {

  isHTTPBackend: Promise<boolean>;

  constructor(
    private platform: Platform,
    private data: DataService,
    private router: Router
  ) {
    this.isHTTPBackend = data.isHTTPBackend();
  }

  async scanBLE() {
    await this.data.scanBLEAndConnect();
    this.router.navigate([ "device" ]);
  }

}
