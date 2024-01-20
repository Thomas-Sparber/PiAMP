#include <ArduinoJson.h>

#include "channel.h"

Channel channels[] = {
  Channel(
    "CH1",  //name
    11,     //switchPin
    10,     //ledPin
    true,   //hasSubChannel
    false   //isFxChannel
  ),
  Channel(
    "CH2",  //name
    9,      //switchPin
    8,      //ledPin
    true,   //hasSubChannel
    false   //isFxChannel
  ),
  Channel(
    "CH3",  //name
    7,      //switchPin
    6,      //ledPin
    true,   //hasSubChannel
    false   //isFxChannel
  ),
  Channel(
    "CH4",  //name
    13,     //switchPin
    12,     //ledPin
    true,   //hasSubChannel
    false   //isFxChannel
  ),

  Channel(
    "FX1",  //name
    4,      //switchPin
    3,      //ledPin
    false,  //hasSubChannel
    true    //isFxChannel
  ),
  Channel(
    "FX2",  //name
    2,      //switchPin
    5,      //ledPin
    false,  //hasSubChannel
    true    //isFxChannel
  )
};

//Defines the time in milliseconds how long the led
//should be on and off for the sub channel blinking
int subBlinkTimeMillis = 200;

//Defines how long the switch needs to be pressed to save the channel
int saveTimeMillis = 5000;

//When the switch is pressed the saving action is indicated by fast led blinking.
//The blinking time is indicated by this variable
int saveBlinkTimeMillis = 20;

//The save-blinking should not start right away but after the following time
int saveTimeStartMillis = 1000;

int channelsCount = sizeof(channels) / sizeof(struct Channel);
int lastTimeMillis;
String lastSentCommand;
String currentCommand;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  for(int i = 0; i < channelsCount; ++i) {
    pinMode(channels[i].switchPin, INPUT_PULLUP);
    pinMode(channels[i].ledPin, OUTPUT);

    digitalWrite(channels[i].ledPin, LOW);
  }

  lastTimeMillis = millis();
}

void sendCommand(String command) {
  lastSentCommand = command;
  Serial.println(command);
}

void switchOffChannelsExcept(String name) {
  for(int i = 0; i < channelsCount; ++i) {
    if(channels[i].name != name && !channels[i].isFxChannel) {
      channels[i].previousState = channels[i].state;
      channels[i].state = 0;
    }
  }
}

bool handleChannel(Channel &ch, int diffMillis, int currentTimeMillis) {
  ch.currentSwitchState = digitalRead(ch.switchPin);


  if(ch.currentSwitchState != ch.previousSwitchState && (currentTimeMillis-ch.lastSwitched) > 100) {
    if(ch.currentSwitchState == LOW) {
      ch.pressedMillis = 0;

      if(ch.isFxChannel) {
        //FX channels can be switched on and off independently
        if(ch.state == 0) {
          ch.state = 1;
          sendCommand("{\"action\":\"fxon\",\"channel\":\"" + ch.name + "\"}");
        }
        else {
          ch.state = 0;
          sendCommand("{\"action\":\"fxoff\",\"channel\":\"" + ch.name + "\"}");
        }
        
      } else {
        //Non FX channels are dependent. Only one non-FX channel can be active
        //switch off all other channels
        switchOffChannelsExcept(ch.name);
        
        if(ch.state == 1 && ch.hasSubChannel) {
          ch.state = 2;
          ch.ledStateCounter = 0;
        } else if(ch.state == 2) {
          ch.state = 1;
        } else {
          if(ch.previousState == 0)ch.previousState = 1;
          ch.state = ch.previousState;
        }

        if(ch.state == 1) {
          sendCommand("{\"action\":\"channelswitch\",\"channel\":\"" + ch.name + "\"}");
        } else {
          sendCommand("{\"action\":\"channelswitch\",\"channel\":\"" + ch.name + "-sub\"}");
        }
      }
    }

    ch.previousSwitchState = ch.currentSwitchState;
  }


  if(ch.currentSwitchState == LOW) {
    ch.lastSwitched = currentTimeMillis;
  }


  //save blinking
  int saveState = HIGH;
  if(ch.currentSwitchState == LOW) {
    ch.pressedMillis += diffMillis;

    if(ch.pressedMillis >= saveTimeStartMillis) {
      if((ch.pressedMillis / saveBlinkTimeMillis) % 2 == 1) {
        saveState = LOW;
      }
    }

    if(ch.pressedMillis > saveTimeMillis) {
      if(ch.state == 2) {
        sendCommand("{\"action\":\"channelsave\",\"channel\":\"" + ch.name + "-sub\"}");
      } else {
        sendCommand("{\"action\":\"channelsave\",\"channel\":\"" + ch.name + "\"}");
      }
      
      ch.pressedMillis = 0;
    }
  }


  //sub channel blinking
  int subChannelState = HIGH;
  if (ch.state == 2) {
    ch.ledStateCounter += diffMillis;

    if((ch.ledStateCounter / subBlinkTimeMillis) % 2 == 1) {
      subChannelState = LOW;
    }
  }


  if(ch.state == 0 || saveState == LOW || subChannelState == LOW) {
    digitalWrite(ch.ledPin, LOW);
  } else {
    digitalWrite(ch.ledPin, HIGH);
  }

  
  return (ch.currentSwitchState == LOW);
}

void handleCommand(String command) {
  JsonDocument doc;
  deserializeJson(doc, command);

  String action = doc["action"];
  
  String channel = doc["channel"];
  int indexSub = channel.lastIndexOf("-sub");
  if(indexSub != -1)channel = channel.substring(0, indexSub);
  
  int channelIndex = -1;
  for(int i = 0; i < channelsCount; ++i) {
    if(channels[i].name == channel) {
      channelIndex = i;
    }
  }

  if(channelIndex != -1) {
    if(action == "channelswitch") {
      if(!channels[channelIndex].isFxChannel) {
        switchOffChannelsExcept(channel);
        if(indexSub != -1)channels[channelIndex].state = 2;
        else channels[channelIndex].state = 1;
      } else {
        sendCommand("{\"log\":\"Channel " + channel + " as an fx channel. It can only be turned on and off\"}");
      }
    } else if(action == "fxon") {
      if(channels[channelIndex].isFxChannel) {
        channels[channelIndex].state = 1;
      } else {
        sendCommand("{\"log\":\"Channel " + channel + " as no fx channel.\"}");
      }
    } else if(action == "fxoff") {
      if(channels[channelIndex].isFxChannel) {
        channels[channelIndex].state = 0;
      } else {
        sendCommand("{\"log\":\"Channel " + channel + " as no fx channel.\"}");
      }
    } else {
      sendCommand("{\"log\":\"Unknown command " + action + "\"}");
    }
  } else {
    sendCommand("{\"log\":\"Channel not found: " + channel + "\"}");
  }
}

void handleSerial() {
  while(Serial.available() > 0) {
    int b = Serial.read();
    currentCommand += (char)b;

    if(b == '\n') {
      if(currentCommand == lastSentCommand) {
        lastSentCommand = "";
      } else {
        handleCommand(currentCommand);
      }
      
      currentCommand = "";
    }
  }
}

void loop() {
  int currentTimeMillis = millis();
  int diffMillis = currentTimeMillis - lastTimeMillis;
  lastTimeMillis = currentTimeMillis;

  int savingCounter = 0;

  for(int i = 0; i < channelsCount; ++i) {
    bool saving = handleChannel(channels[i], diffMillis, currentTimeMillis);
    
    if(saving) {
      savingCounter++;
    }
  }

  if(savingCounter > 1) {
    //more than 2 channels saving at the same time is not allowed
    for(int i = 0; i < channelsCount; ++i) {
      channels[i].pressedMillis = 0;
    }
  }

  handleSerial();
}
