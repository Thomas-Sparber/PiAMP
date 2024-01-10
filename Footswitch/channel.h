#pragma once

struct Channel {

  Channel(String s_name, int i_switchPin, int i_ledPin, bool b_hasSubChannel, bool b_isFxChannel) :
    name(s_name),
    switchPin(i_switchPin),
    ledPin(i_ledPin),
    hasSubChannel(b_hasSubChannel),
    isFxChannel(b_isFxChannel),
    state(0),
    previousState(0),
    currentSwitchState(LOW),
    previousSwitchState(LOW),
    lastSwitched(0),
    pressedMillis(0),
    ledStateCounter(0)
  {}
  
  String name;
  int switchPin;
  int ledPin;
  bool hasSubChannel;

  /**
   * There can always only be one active non-fx channel.
   * FX Channels can be turned on an off. Multiple FX channels can be active at the same time
   */
  bool isFxChannel;

  /**
   * 0 = inactive
   * 1 = active
   * 2 = sub channel active
   */
  int state;
  int previousState;

  /**
   * The following 2 variables are internally used to determine wheter the switch is/was
   * pressed or not and when it is released
   */
  int currentSwitchState;
  int previousSwitchState;

  int lastSwitched;

  /**
   * To count the press time in millis - used for saving channel
   */
  int pressedMillis;

  /**
   * Used for sub channel blinking
   */
  int ledStateCounter;

}; //end struct Channel
