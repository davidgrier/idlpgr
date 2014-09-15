# idlpgr

**IDL interface for Point Grey cameras**

IDL is the Interactive Data Language, and is a product of
[Exelis Visual Information Solutions](http://www.exelisvis.com)

idlpgr is licensed under the GPLv3.

## What it does

This package provides a rudimentary IDL interface for
Point Gray cameras based on the FlyCapture2_C API.
The preferred interface is through the `DGGhwPointGrey`
object:

    camera = DGGhwPointGrey() ; object associated with first camera
    tvscl, camera.read()      ; display the next image

To use this package, make sure that your `IDL_PATH` includes
`/usr/local/IDL/idlpgr`.

This package is written and maintained by David G. Grier
(david.grier@nyu.edu)

## INSTALLATION

Requires the FlyCapture2 API to be installed, a working
installation of IDL, and administrator (sudo) priviledges.

1. unpack the distribution in a convenient directory.
2. `cd idlpgr`
3. `make`
4. `make install`

## UNINSTALLATION

Requires administrator (sudo) priviledges.

1. `cd idlpgr`
2. `make uninstall`
