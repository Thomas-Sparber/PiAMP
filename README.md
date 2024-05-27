# PiAMP

<img src="https://github.com/Thomas-Sparber/PiAMP/blob/master/Drawings/logo/logo.svg" width="400">

PiAMP is a DIY guitar amplifier which uses a neural network to emulate the sound of *any* guitar amplifier or pedal.

It consists of the following components
 - **Raspberry PI** which processes the audio, provides bluetooth connection and receives signals from the controls and footswitch
 - **HifibBerry DAC+ ADC** is the audio interface used for high quality audio
 - **Power amp** which converts the low level audio signal into a signal which can be fed into a speaker
 - **Speakers/Cabinet**
 - **Screen and controls** which allow to set the current Amp model, cabinet and other controls (gain, bas, mid, etc.)
 - **Footswitch** which allows to quickly switch between saved channels
 - **Bluetooth app** which allows to connect to the PiAMP and set its parameters via phone

Here a picture of my PiAMP with my DIY guitar

![app](https://github.com/Thomas-Sparber/PiAMP/blob/master/pictures/IMG_20240307_130941.jpg)

This is how it looks like when its turned on

![app](https://github.com/Thomas-Sparber/PiAMP/blob/master/pictures/IMG_20240515_171510.jpg)

And here is a picture of the foot switch

![app](https://github.com/Thomas-Sparber/PiAMP/blob/master/pictures/IMG_20240515_171545.jpg)

## Disclaimer

This project is open source and everyone is very welcome to build its own PiAMP. Also everyone is very welcome to contribute to this project!

The following instructions are not (yet) meant to be complete, I'm just writing down what I remember from building my amp.
I am very happy to help you if you want to build your own amp - it would be great if you could contribute in creating a more comprehensive guide!

## Schematics and components used

Here is a very crude schematics of what's going on inside the PiAMP:

![app](https://github.com/Thomas-Sparber/PiAMP/blob/master/Drawings/Schematics.svg)

The Raspberry PI takes the input signals (guitar + line in) from the HifiBerry shield and processes the signals using the [NeuralPi](https://github.com/Thomas-Sparber/NeuralPi) [sushi](https://github.com/elk-audio/sushi) plugin. The processed audio signal leaves the PI again via the HifiBerry shield and enters the power amp which amplifies the signal and drives the speakers.

On the Raspberry PI there is also a [nodejs](https://nodejs.org/) service running which does the following things:
 - Controls the NeuralPi plugin using [OSC](https://de.wikipedia.org/wiki/Open_Sound_Control)
 - Provides bluetooth connection to allow connecting via app
 - Connects to an arduino mega via USB + serial to receive commands about changing parameters
 - Connects to the footswitch via USB + serial to receive commands about switching and saving channels

Here is the list of components I used. Please note that I don't want to promote products/sellers; these are just the components I used and most of them can be replaced with similar components:
 - [Raspberry PI 4](https://www.raspberrypi.com/products/raspberry-pi-4-model-b/)
 - [HifiBerry DAC* ADC](https://www.hifiberry.com/shop/boards/hifiberry-dac-adc/)
 - [Raspberry power supply](https://www.berrybase.at/offizielles-raspberry-pi-usb-c-netzteil-5-1v/3-0a-eu-schwarz?c=348)
 - [the t.amp TA50](https://www.thomann.de/at/the_tamp_ta50.htm)
 - 2x [Celestion BL15-300X 4 Ohm](https://www.thomann.de/at/celestion_bl15_300x_4_ohm.htm)

These were the mandatory components, now the optional components
 - [Arduino mega](https://store.arduino.cc/products/arduino-mega-2560-rev3) for the controls (screen + knobs)
 - [3.5inch Arduino display](http://www.lcdwiki.com/3.5inch_Arduino_Display-UNO)
 - [KY-040 rotary encoder](https://sensorkit.joy-it.net/en/sensors/ky-040)
 - [Turnigy TGY-1551A Analog Micro Servo](https://hobbyking.com/de_de/turnigy-tgy-1551a-analog-micro-servo-1-0kg-0-08sec-5g.html) for the knobs (gain, bass, mid etc.)
 - [Raspberry PI pico](https://www.raspberrypi.com/products/raspberry-pi-pico/) to run the foot switch. A small arduino should also be fine

Then you will also need a lot of accessoires and connectors. This depends a bit on how you want to build it, e.g.
 - A [cable](https://www.ebay.at/itm/381639249289?mkcid=16&mkevt=1&mkrid=5221-175123-2357-0&ssspo=-1ao0QHWTmW&sssrc=2047675&ssuid=FjqDye_rTFu&widget_ver=artemis&media=COPY) to connect the HifiBerry shield to the power amp
 - [Jack connector](https://www.ebay.at/itm/402632877457?mkcid=16&mkevt=1&mkrid=5221-175123-2357-0&ssspo=fewUSpItTeq&sssrc=2047675&ssuid=FjqDye_rTFu&widget_ver=artemis&media=COPY) for your guitar input. I also used these to connect the power amp output to the speakers
 - [Footswitch switches](https://www.ebay.at/itm/325064963804)
 - [XLR male](https://www.ebay.at/itm/360663764984) and [female](https://www.ebay.at/itm/200930409418) to connect the raspberry to the footswitch
 - [USB to TTL adapter](https://www.ebay.at/itm/255323594516?mkcid=16&mkevt=1&mkrid=5221-175123-2357-0&ssspo=kSLcqpXwSAK&sssrc=2047675&ssuid=FjqDye_rTFu&widget_ver=artemis&media=COPY) to connect the raspberry to arduino + footswitch
 - LEDs, switches, cables, etc.
 - Wood or other material to build the frame, footswitch etc.
 - Several 3d printed parts (STL files in this repo)

## Setting up the Raspberry PI

Please follow the guide on Elk-Audio to set up your raspberry pi: [Guide](https://elk-audio.github.io/elk-docs/html/intro/getting_started_with_raspberry.html)

At the end you should have a Raspberry PI up and running elk os.

You need to have a `/udata/libs` folder on your pi. If not, create it by running

    sudo mkdir/udata/libs

Next go to [Releases](https://github.com/Thomas-Sparber/PiAMP/releases), and download all files to your PI, e.g. by using wget

    cd /udata/libs
    wget https://github.com/Thomas-Sparber/PiAMP/releases/download/1.0/NeuralPi.vst3.tgz
    wget ...

Next, install the sushi plugin by running

    tar xf NeuralPi.vst3.tgz
    mv NeuralPi.vst3 ~/plugins/NeuralPi.vst3

Copy the config file to its correct folder by running

    cp config_neuralpi.json ~/config_files/config_neuralpi.json

Finally, extract the tones, irs and images to its correct folders by running

    mkdir -p ~/Documents/GuitarML/NeuralPi
    tar xf images.tgz
    mv images ~/Documents/GuitarML/NeuralPi
    tar xf tones.tgz
    mv tones ~/Documents/GuitarML/NeuralPi
    tar xf irs.tgz
    mv irs ~/Documents/GuitarML/NeuralPi

The next command enables the NeuralPi plugin on startup, see [here](https://elk-audio.github.io/elk-docs/html/documents/working_with_elk_board.html#configuring-automatic-startup)

    sudo cp sushi.service /lib/systemd/system/sushi.service
    sudo systemctl enable sushi

Then install the PiAMP server by running

    tar xf node-v20.9.0-linux-arm64.tar.xz
    tar xf piamp.tgz
    sudo cp piamp.service /lib/systemd/system/piamp.service
    sudo systemctl enable piamp

Now, as everything is installed you can start it by restarting you pi or running

    sudo systemctl start sushi
    sudo systemctl start piamp

Now as everything is up and running you can access the web interface of PiAMP by opening `http://<raspberry-ip-address>:3600/` in your browser.

Alternatively (/additionally) you can of yourse also install the mobile app on your phone by installing `app-debug.apk` on your phone. Please note that is app is not (yet) available from any app store (If PiAMP gets popular I will probably publish it), so you need to manually install it. Please check [online](https://stackoverflow.com/questions/15743391/i-want-to-install-an-unsigned-apk-file-on-my-mobile-what-to-do) how to install unsigned apk files on your phone.