Benchmarking 2D marker tracking libraries
=========================================

This project aims at providing a rigorous framework to benchmark
the main open-source libraries for 2D marker detection and tracking.

![](data/chilitags/chilitags-benchmark-easy-320.png)

The contenders we have considered so far are:

- [ALVAR](http://virtual.vtt.fi/virtual/proj2/multimedia/alvar/) 2.0.0 (LGPL 2.1)
- [ARToolkitPlus](http://studierstube.icg.tu-graz.ac.at/handheld_ar/artoolkitplus.php) 2.1.1 (GPL)
- [Aruco](http://www.uco.es/investiga/grupos/ava/node/26) 1.2.5 (BSD)
- [chilitags](http://chili.epfl.ch/software) 2.0.0 (LGPL 3) **[disclaimer: we are the
  authors of chilitags!]**

(note that ARToolKitPlus is painful to compile and install. We relied on
[`robotpkg`](http://robotpkg.openrobots.org/robotpkg/image/artoolkitplus/index.html)
for that).

Methodology
-----------

Several criteria must be taken into account when comparing libraries:
performances, but also documentation, ease of use (build system, reliance on
standards, etc), community...

This benchmark currently only focuses on a single performance metric: detection
speed of markers in a static image.

We rely on a common layout of markers (pictured above), with markers adapted to
each libraries, then precisely time to **detection** duration (ie, without the
time to load the image or display the results), and finally average the result
on 20 runs.

Since we test the detection at various resolutions, it also gives a hint of the
library performances when detecting small markers.

We use default configurations for the libraries, but when they expose different
presets, each of them has been tested. Please feel free to send over an
[email](mailto:severin.lemaignan.youknowthatthispartdoesnotneedtobethere@epfl.ch)
if you want to suggest more appropriate settings.

Detection speed results
-----------------------

### Resolution 160x226

|Library| Average processing time (ms) | Std deviation | Missed markers |
|-------|-----------------------------:|--------------:|----------------|
| `alvar` (default) | 2.78548 |        0.5 | 73, 90, 124, 272, 672, 782, 865, 1018,  |
| `artoolkitplus` (default) | 1.5 |        0.1 | 0, 38, 73, 90, 124, 272, 865,  |
| `aruco` (default) | 2.9 |        1.9 | 272,  |
| `chilitags` (`ROBUST` preset) | 2.7 |        0.2 | 73, 90, 272,  |
| `chilitags` (`FAST` preset) | 2.7 |        0.1 | 73, 90, 272,  |
| `chilitags` (`FASTER` preset) | 1.9 |        0.1 | 50, 73, 90, 124, 272, 1018,  |

### Resolution 320x452

|Library| Average processing time (ms) | Std deviation | Missed markers |
|-------|-----------------------------:|--------------:|----------------|
| `alvar` (default) | 4.8 |        0.4 | 272, 672,  |
| `artoolkitplus` (default) | 2.5 |        0.2 |  |
| `aruco` (default) | 3.5 |        0.9 | 272,  |
| `chilitags` (`ROBUST` preset) | 9.6 |        0.4 |  |
| `chilitags` (`FAST` preset) | 6.3 |        0.3 |  |
| `chilitags` (`FASTER` preset) | 3.7 |        0.2 |  |

### Resolution 640x904

|Library| Average processing time (ms) | Std deviation | Missed markers |
|-------|-----------------------------:|--------------:|----------------|
| `alvar` (default) | 7.9 |        0.4 | 672,  |
| `artoolkitplus` (default) | 6.1 |        0.2 |  |
| `aruco` (default) | 6.7 |        1.3 | 272,  |
| `chilitags` (`ROBUST` preset) | 32.4 |        0.5 |  |
| `chilitags` (`FAST` preset) | 15.3 |        0.4 |  |
| `chilitags` (`FASTER` preset) | 8.4 |        0.3 |  |

### Resolution 800x1131

|Library| Average processing time (ms) | Std deviation | Missed markers |
|-------|-----------------------------:|--------------:|----------------|
| `alvar` (default) | 10.2 |        0.5 | 672,  |
| `artoolkitplus` (default) | 8.9 |        0.3 |  |
| `aruco` (default) | 8.1 |        0.5 | 272,  |
| `chilitags` (`ROBUST` preset) | 44.7 |        0.9 |  |
| `chilitags` (`FAST` preset) | 20.9 |        0.6 |  |
| `chilitags` (`FASTER` preset) | 10.5 |        0.7 |  |

### Resolution 1280x1809

|Library| Average processing time (ms) | Std deviation | Missed markers |
|-------|-----------------------------:|--------------:|----------------|
| `alvar` (default) | 22.0 |        0.5 | 672,  |
| `artoolkitplus` (default) | 20.4 |        0.2 | 36,  |
| `aruco` (default) | 15.3 |        0.6 |  |
| `chilitags` (`ROBUST` preset) | 123.9 |        1.2 |  |
| `chilitags` (`FAST` preset) | 45.2 |        1.0 |  |
| `chilitags` (`FASTER` preset) | 21.0 |        1.1 |  |


Next steps
----------

- test at different resolutions
- use non-synthetic datasets
- benchmark robustness

