import { Component, EventEmitter, Input, Output } from '@angular/core';
import { Parameter } from '../models/parameter';

@Component({
  selector: 'app-parameter',
  templateUrl: 'parameter.page.html',
  styleUrls: ['parameter.page.scss'],
})
export class ParameterPage {

  @Input() parameter?: Parameter;

  @Output() valueChanged = new EventEmitter<Parameter>();

  onValueChanged(ev: Event, parameter: Parameter) {
    this.valueChanged.emit(parameter);
  }

}
