# MicroW - Xbee configuration

The Xbee settings that differ from default configuration are in *Networking & Security* and *Serial interfacing*.

Serial interface settings should correspond to UART settings in the microcontrollers (230400 8N1 by default).

About networking, emitter module's Xbee must use broadcast destination adress (0xFFFF).
And every Xbee in the boat must have AES encryption enabled with the same encryption key.

Obviously, every Xbee module in the boat must be in the same network (*Channel* and *PAN ID* settings).

It's also a good idea to disable ACKs (set MAC Mode setting to *802.15.4 no ACKs [1]*), so that Xbee modules will only use RF interface to transmit the coxswain's voice.

In a future version, Xbee configuration will probably be automatically performed by embedded microcontrollers.

## License

<a rel="license" href="http://creativecommons.org/licenses/by/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by/4.0/88x31.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by/4.0/">Creative Commons Attribution 4.0 International License</a>.
