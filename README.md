# MicroW

>  The Coxswain's voice is law.

MicroW is a *wireless* device that transmits the coxswain's voice in a rowing boat. The coxswain has a *wireless* microphone, and several *wireless* speakers are in the boat.

<p align="center">
  <img src="https://github.com/sonibla/MicroW/blob/master/images/logo_small.png" alt="MicroW logo" height="90"/>
</p>

<p align="center">
    <a href="http://creativecommons.org/licenses/by/4.0/"><img src="https://img.shields.io/badge/License-CC%20BY%204.0-lightgrey.svg" alt="License: CC BY 4.0"/></a>
    <a href="https://opensource.org/licenses/BSD-3-Clause"><img src="https://img.shields.io/badge/License-BSD%203--Clause-blue.svg" alt="License: BSD-3-Clause"/></a>
</p>
<p align="center">
    <img src="https://img.shields.io/badge/Open%20Source-%20%E2%99%A5%20-brightgreen" alt="Open Source"/>
    <img src="https://img.shields.io/badge/Open%20Hardware-%20%E2%99%A5%20-brightgreen" alt="Open Hardware"/>
</p>

## Overview

MicroW consists of several waterproof devices placed inside a rowing boat. The coxswain has an emitter module, and one or several receiver modules are in the boat. Instead of wires, MicroW uses worldwide approved 2.4 GHz radio signal.

![overview boat](images/Overview_boat.png "MicroW overview: boat scale")

Coxswain's voice is processed by analog and numerical circuits and encrypted with 128-bit AES before radio transmission. Receiver modules shouldn't be more than 30 meters away from the emitter, which is enough compared to the lengh of a rowing boat (about 20 meters).

Here is an overview of the inside of each module :

![overview modules](images/Overview_modules.png "MicroW overview: modules")

Details about electrical circuits will be in [analog](analog) folder. Embedded C codes, radio transmission and everything digital related will be in [digital](digital) folder.

## Current project state

Currently, only [embedded C codes](https://github.com/sonibla/MicroW/tree/master/digital/STM32F429ZI%20Source%20codes) are available.
Analog circuits, test tools and methods and more documentation soon available

## License

The content of this project itself is licensed under the [Creative Commons Attribution 4.0 International license](https://creativecommons.org/licenses/by/4.0/), and the underlying source code used to format 
and display that content is licensed under the [3-Clause BSD License](https://opensource.org/licenses/BSD-3-Clause).

<a href="http://creativecommons.org/licenses/by/4.0/"><img src="https://img.shields.io/badge/License-CC%20BY%204.0-lightgrey.svg" alt="License: CC BY 4.0"/></a>
[![License](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)

## Contributors

Current maintainers:

* **Matthieu Planas**
* [**Alban Benmouffek**](https://github.com/sonibla)

People who previously worked on this project:

* **Guillaume Bachelard**
* **Martin Combeuil**
* **Nicolas Kervinio**
* **Victoire Malinovsky**
* **Breval Mesureur**
* **Alexia Vanson**

Acknowledgements to:

* **Florent Goutailler**
* **Thomas Tang**
* **Nicolas Simond**
* **Vincent Ricchi**
