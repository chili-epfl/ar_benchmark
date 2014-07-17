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

The base image has a resolution of 800x1131 pixels (A4 at ~96dpi).

We use default configurations for the libraries, but when they expose different
presets, each of them has been tested. Please feel free to send over an
[email](mailto:severin.lemaignan.youknowthatthispartdoesnotneedtobethere@epfl.ch)
if you want to suggest more appropriate settings.

Detection speed results
-----------------------

|Library| Average processing time (ms) | Std deviation | Missed markers |
|-------|-----------------------------:|--------------:|----------------|
| `alvar` (default) | 10.2186 |        0.7 | 672,  |
| `artoolkitplus` (default) | 8.9 |        0.2 |  |
| `aruco` (default) | 8.7 |        2.4 | 272,  |
| `chilitags` (`ROBUST` preset) | 44.5 |        0.6 |  |
| `chilitags` (`FAST` preset) | 20.7 |        0.4 |  |
| `chilitags` (`FASTER` preset) | 10.2 |        0.3 |  |


Next steps
----------

- test at different resolutions
- use non-synthetic datasets
- benchmark robustness

