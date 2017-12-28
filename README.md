DeLata Files

This repository hosts the files needed for the basic cansat setup. The following hardware is required:

(1) PHL-Microsat Comet Board
(2) Adafruit TTL Serial Camera

The PHL-Microsat Comet Board has built-in RFM69HCW transceiver and a SD card slot. The Comet Library (actually a wrapper of functions for image capture and storage, data acquisition and storage, and data transmission) is tailor-fitted for the pinouts of the Comet Board. Samples for using the library can be found on the Comet_Library. 

The Adafruit_VC0706 and RadioHead libraries are required to get the cansat setup running. This libraries support image capture and storage, and data transmission.

The Server file, cansat.exe can be found inside the Server folder. 