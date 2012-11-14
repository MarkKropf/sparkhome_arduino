# Important Note
This is a quick and dirty OSC -> LED Pixel controller for arduino. This code uses the timer() to produced timed effects instead of delay(). The result is roughly a 35fps speed out of a duemilanove arduino. I currently use MaceTech octobars with this code, but other RGB LED pixels could be used. I also now include the MAX/MSP patch to take sound from a laptop and perform realtime beat detection to send beat messages to the Arduino LED Pixel controller. This allows for some cool effects that move with the music and can also be used for syncing the speed of visual effects to music. 

https://vimeo.com/51333812 --> Video of the effects currently included in this project.

## Arduino Components:
* http://www.arduino.cc/en/Main/ArduinoBoardDuemilanove --> Arduino Duemilanove
* http://www.arduino.cc/en/Main/ArduinoEthernetShield --> Arduino Ethernet Shield
* http://macetech.com/store/index.php?main_page=product_info&products_id=35 --> Macetech Octobar
* http://macetech.com/store/index.php?main_page=product_info&cPath=1&products_id=34 --> Macetech Satellite Module

## OSC Configuration:
* I have my OSC clients send to the broadcast to allow multiple endpoints to receive the same OSC messages.
* http://hexler.net/software/touchosc --> is the iPad/iPhone app I use to manually control the LED Pixels. (TouchOSC Layout is Provided)
* https://www.ableton.com/en/ --> I use Ableton for live performacne software which loads a MaxMSP Patch to send beat detection info via OSC to the Arduino.
* http://cycling74.com/ --> MAX/MSP gives me the Ableton interface to speak OSC to the Arduino

## MAX/MSP
* A Patch and supporting Beat Detection library are now included in the MaxMSP folder. 
* The patch needs your host or broadcast IP in the "OSC Host Config" section to send OSC Beat messages properly.
* Memory, Max BPM and Min BPM can be configured by editing the patch outside of presentation mode.
