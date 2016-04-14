# idlpgr

**IDL interface for Point Grey cameras**

IDL is the Interactive Data Language, and is a product of
[Harris Geospatial Solutions](http://www.harrisgeospatial.com/)

[Point Grey](http://www.ptgrey.com) manufactures digital cameras,
including scientific video cameras.

*idlpgr* is licensed under the
[GPLv3](http://www.gnu.org/licenses/licenses.html#GPL).

## What it does

This package provides an IDL interface for
PointGrey cameras based on the FlyCapture2 API.
The preferred interface is through the `DGGhwPointGrey`
object:

    camera = DGGhwPointGrey() ; object associated with first camera
    tvscl, camera.read()      ; display the next image

To use this package, make sure that your `IDL_PATH` includes
`/usr/local/IDL/idlpgr`.

This package is written and maintained by David G. Grier
(david.grier@nyu.edu)

## INSTALLATION

### Optional: Install the FlyCapture2 API
FlyCapture2 is available from the
[PointGrey download site](http://www.ptgrey.com/support/downloads).
Follow their instructions to install for your platform.

### Install idlpgr

1. unpack the distribution in a convenient directory.
2. `cd idlpgr`
3. `make`
4. `make install`

Installation requires super-user privileges.

## UNINSTALLATION

1. `cd idlpgr`
2. `make uninstall`
