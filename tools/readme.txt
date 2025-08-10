# Tools for debugging stuff

##### ff_check.com

Attempts to read register 0xFF from the sound board (trying addresses 0x0088 - 0x0388) and printing results.

The expected responses according to documentation are:

YM2203: (undefined)
YM2608: 0x01

Testing results:

PC-9801DS/U2 w/ built-in YM2203: 0xDF
