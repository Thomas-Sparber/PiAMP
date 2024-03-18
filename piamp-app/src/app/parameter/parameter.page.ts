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

  async next() {
    const listValues = await this.parameter!.listValues!;
    let index = listValues.findIndex(value => (value.id == this.parameter!.value));

    if(index == -1) {
      index = 0;
    } else if(index == listValues.length - 1) {
      index = 0;
    } else {
      index++;
    }

    this.parameter!.value = listValues[index].id;
  }

  async previous() {
    const listValues = await this.parameter!.listValues!;
    let index = listValues.findIndex(value => (value.id == this.parameter!.value));

    if(index == -1) {
      index = listValues.length - 1;
    } else if(index == 0) {
      index = listValues.length - 1;
    } else {
      index--;
    }

    this.parameter!.value = listValues[index].id;
  }

}
