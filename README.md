# Important Note
This is a quick and dirty OSC -> LED Pixel controller for arduino. This code uses the timer() to produced timed effects instead of delay(). The result is roughly a 35fps speed out of a duemilanove arduino. I currently use MaceTech octobars with this code, but other RGB LED pixels could be used.

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


