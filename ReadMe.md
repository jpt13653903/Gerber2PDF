# Gerber2PDF

Gerber2PDF is a command-line tool to convert Gerber files to PDF for proofing 
and hobbyist printing purposes.

## Installation:

To build from source for Windows, Linux, or macOS, see [Source](#source).

Alternatively, download and run
[Gerber2pdf.exe](https://sourceforge.net/projects/gerber2pdf/files/Gerber2pdf.exe/download) (Windows)
or [Gerber2pdf](https://sourceforge.net/projects/gerber2pdf/files/Gerber2pdf/download) (Linux 64-bit).
This will display the copyright and usage information.

## Usage:

A batch-file or script is recommended, but a simple "Open with..." link will 
also work, resulting in a single Gerber converted to a PDF with the same 
file-name as the Gerber.

## Malware Warning:

The SourceForge automated malware warnings are set up to be quite aggressive, 
with a high probability of false-positives.  My suggestion is to trust your 
personal antivirus software instead.

## Source:

The source is available from the
[git repository](https://sourceforge.net/p/gerber2pdf/code/) and
[GitHub](https://github.com/jpt13653903/Gerber2PDF).

To build on Linux, run `git submodule update --init --recursive && make`.

To build on macOS, run `git submodule update --init --recursive && make`.

To build on Windows, install [MinGW](http://tdm-gcc.tdragon.net/) and run 
`git submodule update --init --recursive && mingw32-make` from within a 
POSIX-like environment, such as [Git Bash](https://git-scm.com/).

### Building Issues

A common issue with building is the Git submodules.  If the `Engine/Toolbox` 
folder is empty, GNU make will issue a very cryptic "No rule to make target".

To solve the issue, run `git submodule update --init --recursive` so that all 
submodules are initialised and up to date.

You can also download the files manually from
[the Toolbox repo](https://github.com/jpt13653903/Toolbox/tree/master)
and save it in the `Engine/Toolbox` folder, but it is highly recommended to let 
Git sort it out for you.

## Changes:

#### 2022-12-02

- Version-bump to 1.8
- Added automatic translation to the output PDF to place the global PDF origin
  at the bottom-left of the page.

#### 2021-04-03

- Strokes2Fills is now tolerant to rounding errors in the Gerber.  Segments
  closer than 1 μm apart are considered to be coincident.

#### 2021-02-20

- Added the `-CMYK` option, which converts the output PDF
  to use the CMYK colour space.

#### 2021-02-06

- Version-bump to 1.7
- The `-strokes2fills` option now joins disjointed line and arc segments into
  a single polygon.
- The `-strokes2fills` option now supports holes in boards.

#### 2020-11-20

- Bugfix: The polygon aperture type expects an integer number of sides, but 
  some Gerber files specify it with a trailing ".0000".

#### 2020-11-11

- Bugfix: Gerbers that issue a move command before selecting a tool now 
  renders without problems.

#### 2020-05-09

- Empty coordinates, although not conforming to the Gerber file format 
  specification, now returns a warning instead of an error.

#### 2019-03-02

- Version-bump to 1.6
- Removed the background colour limitation.  The same Gerber can now be 
  rendered in different background colours on different pages.

#### 2019-02-04

- Added Unicode file-name support
- Various internal architecture improvements (see Git log for details)

#### 2019-01-13

- Version-bump to 1.5
- Removed the limitation where `-strokes2fills` applies to all instances of 
  the Gerber file

#### 2018-11-20

- Version-bump to 1.4
- Added the `-page_size` command-line option

#### 2017-12-20

- Fixed bug relating to explicit positive numbers

#### 2017-08-13

- Implemented a new "-strokes2fills" command-line option.
- Fixed bug relating to the Open Contour warning.

#### 2016-10-09

- Fixed bug relating to Gerbers that specify units by means of the deprecated 
  codes `G70` or `G71`, instead of the mandatory `%MO??*%` parameter.

#### 2016-09-25

- Added an option to change the background colour

#### 2016-09-24

- Fixed bug relating to arc bounding box calculation

#### 2016-09-21

- Fixed bug relating to the first coordinate of an imaging level

#### 2016-07-20

- Fixed bug relating to closed contour warnings

#### 2015-11-12

- Fixed bug relating to arc bounding box calculation

#### 2015-02-21

- Added silent exit feature

#### 2015-01-20

- Fixed bug where paths were handled wrong when terminated with a flash 
  instead of a move.

#### 2014-12-04

- Multiple instances of negative image Gerbers now render correctly.

#### 2014-08-28

- Fixed bug regarding the calculation of arc centres.
- Fixed bug where paths were sometimes drawn as regions.

#### 2014-06-22

- Added a "-nocombine" option.

#### 2014-06-21

- Added transparency to the layers.  Transparency is specified by adding an 
  optional alpha value to the colour.
- Optimised for smaller PDF output by reusing previous Gerber conversions.
- Added warnings for deprecated features (optionally disabled by
  the "-nowarnings" option).
- Added an icon to Gerber2pdf.exe.
- Fixed step-and-repeat bug

#### 2014-06-20

- Fixed bug regarding aperture macros and primitives with "exposure off".
- Added an option to change the layer colour.
- Added an option to combine layers onto a single page.
- Added an option to mirror the output.

