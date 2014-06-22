Gerber2PDF is a command-line tool to convert Gerber files to PDF for 
proofing and hobbyist printing purposes.

Installation:
-------------

Download and run Gerber2pdf.exe.
This will display the copyright and usage information.

Usage:
------

A batch-file or script is recommended, but a simple "Open with..." link 
will also work, resulting in a single Gerber to be converted to a PDF 
with the same file-name as the Gerber.

Changes:
--------

2014-06-22
- Added a "-nocombine" option.

2014-06-21
- Added transparency to the layers.  Transparency is specified by adding an
  optional alpha value to the colour.
- Optimised for smaller PDF output by reusing previous Gerber conversions.
- Added warnings for deprecated features (optionally disabled by the
  "-nowarnings" option).
- Added an icon to Gerber2pdf.exe.
- Fixed step-and-repeat bug

2014-06-20
- Fixed bug regarding aperture macros and primitives with "exposure off".
- Added an option to change the layer colour.
- Added an option to combine layers onto a single page.
- Added an option to mirror the output.

Source:
-------

The source is available from the git repository:
<https://sourceforge.net/p/gerber2pdf/code/>

