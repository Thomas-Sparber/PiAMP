import { inject } from "@angular/core";
import { CanActivateFn, Router } from "@angular/router";
import { DataService } from "../services/data.service";

export  function HomeGuard(): CanActivateFn {
  return async () => {
    const dataService: DataService = inject(DataService);
    const router: Router = inject(Router);
    
    const http = await dataService.isHTTPBackend();
    if(!http)return true;
    return router.createUrlTree(["device"]);
  };
}