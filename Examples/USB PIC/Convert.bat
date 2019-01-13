echo off


"..\..\Engine\bin\Gerber2pdf.exe"                  ^
  -nowarnings                                      ^
  -silentexit                                      ^
  -page_size=extents                               ^
  -output=Output                                   ^
  -combine                                         ^
    -colour=0,128,0         top_copper.grb         ^
    -colour=135,157,175,200 top_solder_mask.grb    ^
    -colour=191,191,0,200   top_silk.grb           ^
    -colour=255,255,255     drill_file.drl.grb     ^
    -colour=0,0,0           board_outline.grb      ^
  -combine -mirror                                 ^
    -colour=0,128,0         bottom_copper.grb      ^
    -colour=135,157,175,200 bottom_solder_mask.grb ^
    -colour=191,191,0,200   bottom_silk.grb        ^
    -colour=255,255,255     drill_file.drl.grb     ^
    -colour=0,0,0           board_outline.grb      ^
  -combine -nomirror                               ^
    -colour=0,128,0         top_copper.grb         ^
    -colour=135,157,175,200 top_solder_mask.grb    ^
    -colour=191,191,0,200   top_silk.grb           ^
    -colour=255,255,255     drill_file.drl.grb     ^
    -colour=0,0,0           board_outline.grb      ^
  -mirror                                          ^
    -colour=0,128,0         bottom_copper.grb      ^
    -colour=135,157,175,200 bottom_solder_mask.grb ^
    -colour=191,191,0,200   bottom_silk.grb        ^
    -colour=255,255,255     drill_file.drl.grb     ^
    -colour=0,0,0           board_outline.grb      ^
 "-header=10,5,The Header"                         ^
 "-footer=10,5,The Footer"                         ^
  -nomirror -nocombine                             ^
    top_silk.grb                                   ^
  -noheader -nofooter                              ^
    bottom_silk.grb                                ^
    top_copper.grb                                 ^
    bottom_copper.grb                              ^
    top_solder_mask.grb                            ^
    bottom_solder_mask.grb                         ^
    drill_file.drl.grb                             ^
    board_outline.grb                              ^
 "-header=15,3,The Header"                         ^
 "-footer=5,10,The Footer"

explorer Output.pdf

