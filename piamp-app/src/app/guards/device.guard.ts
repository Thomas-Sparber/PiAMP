import { inject } from "@angular/core";
import { CanActivateFn, Router } from "@angular/router";
import { DataService } from "../services/data.service";

export  function DeviceGuard(): CanActivateFn {
  return async () => {
    const dataService: DataService = inject(DataService);
    const router: Router = inject(Router);
    
    const connected = await dataService.isConnected();
    if(connected)return true;
    return router.createUrlTree(["home"]);
  };
}