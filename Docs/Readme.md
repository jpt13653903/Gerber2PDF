# Gerber2PDF

```
Usage: Gerber2pdf [-silentexit] [-nowarnings] [-CMYK] ...
       [-output=output_file_name] ...
       [-background=R,G,B[,A]] [-backgroundCMYK=C,M,Y,K[,A]] ...
       [-strokes2fills] [-page_size=extents|A3|A4|letter] ...
       [-orientation=portrait|landscape] [-scale_to_fit] ...
       [-next_page_size=extents|A3|A4|letter] ...
       [-next_orientation=portrait|landscape] [-next_scale_to_fit] ...
       file_1 [-combine] file_2 file_3 file_4...
       [-colour=R,G,B[,A]] [-colourCMYK=C,M,Y,K[,A]] [-mirror] ...
       [-nomirror] [-nocombine] ... file_N

Example: Gerber2pdf -output=My_Project
         top_silk.grb bottom_silk.grb
         top_copper.grb inner_copper_1.grb
         inner_copper_2.grb bottom_copper.grb
         bottom_solder_mask.grb top_solder_mask.grb
         board_outline.grb
         -combine -mirror
         -colour=255,0,0     bottom_copper.grb
         -colour=0,128,0,200 bottom_solder_mask.grb
         -colour=0,0,255     board_outline.grb
         -combine -nomirror
         -colour=255,0,0     top_copper.grb
         -colour=0,128,0,200 top_solder_mask.grb
         -colour=0,0,255     board_outline.grb

The -silentexit option disables the pause on exit.
The -nowarnings option disables deprecated feature warnings.

The -CMYK option translates the output PDF to use the CMYK colour space.
The colours can still be specified using -colour and -background,
but this is not quite accurate.  For more accurate colour control,
use -colourCMYK and -backgroundCMYK instead.

The -colour (in RGB) uses components in the range 0 to 255, whereas
the -colourCMYK uses components in the range 0 to 100.  This is true
for the Alpha component as well.

The optional -background colour is either transparent or opaque.  The
threshold is 50% (i.e. A=128 for RGB and A=50 for CMYK).
Set it just before the target page is created.

The -strokes2fills option converts all strokes to fills for the next
file, thereby converting outlines to areas.  It resets to default
after that file.

The -page_size option takes global effect and can have one of 4 values:
  "extents", "A3", "A4" or "letter"

The -orientation and -scale_to_fit options only take effect
on standard paper sizes (i.e. A3, A4 and letter).

The -next_page_size, -next_orientation and -next_scale_to_fit options
only take effect for the next page created.  Define before calling the
first Gerber of that page.  These override the "global" options.
```

