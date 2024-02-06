import { ComponentFixture, TestBed } from '@angular/core/testing';
import { RouterModule } from '@angular/router';
import { IonicModule } from '@ionic/angular';

import { ParameterPage } from './parameter.page';

describe('ParameterPage', () => {
  let component: ParameterPage;
  let fixture: ComponentFixture<ParameterPage>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ParameterPage],
      imports: [IonicModule.forRoot(), RouterModule.forRoot([])]
    }).compileComponents();

    fixture = TestBed.createComponent(ParameterPage);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
