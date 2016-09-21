# Gerber2PDF

Gerber2PDF is a command-line tool to convert Gerber files to PDF for 
proofing and hobbyist printing purposes.

## Installation:

Download and run Gerber2pdf.exe.
This will display the copyright and usage information.

## Usage:

A batch-file or script is recommended, but a simple "Open with..." link 
will also work, resulting in a single Gerber to be converted to a PDF 
with the same file-name as the Gerber.

## Changes:

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
- Added warnings for deprecated features (optionally disabled by the
  "-nowarnings" option).
- Added an icon to Gerber2pdf.exe.
- Fixed step-and-repeat bug

#### 2014-06-20

- Fixed bug regarding aperture macros and primitives with "exposure off".
- Added an option to change the layer colour.
- Added an option to combine layers onto a single page.
- Added an option to mirror the output.

## Source:

The source is available from the git repository:
<https://sourceforge.net/p/gerber2pdf/code/>

