# Gerber2PDF

```
Usage: Gerber2pdf [-silentexit] [-nowarnings] [-output=output_file_name] ...
       [-background=R,G,B[,A]] [-strokes2fills] ...
       [-page_size=extents|A3|A4|letter] ...
       file_1 [-combine] file_2 ... [-colour=R,G,B[,A]] [-mirror] ...
       [-nomirror] [-nocombine] ... file_N
       [-header=Size,Space,Text] [-footer=Size,Space,Text] 
       [-noheader] [-nofooter] 

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
         -header=10,5,HeaderText
         -footer=10,5,FooterText

The -silentexit option disables the pause on exit.
The -nowarnings option disables deprecated feature warnings.

The optional -background colour is either transparent or opaque.  The
threshold is A=128.  Set it just before the target page is created.  Take
care when using this option, because this background colour is used to
draw the copper pour cut-outs.  That same colour will therefore apply
for every subsequent use of that layer, irrespective of the "current"
background colour.  To work around this limitation, use separate Gerber
files for every different background colour.

The -strokes2fills option converts all strokes to fills for the next
file, thereby converting outlines to areas.  It resets to default
after that file.

The -page_size option takes global effect and can have one of 4 values:
  "extents", "A3", "A4" or "letter"

The -header and -footer options apply when creating a new page,
so these must be specified after the Gerber file, not before it.
Similarly, the -noheader and -nofooter options must be specified
after the first file that must not have a header and / or footer.
```

