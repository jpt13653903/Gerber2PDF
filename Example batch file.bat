echo off

"C:\Program Files\Gerber2PDF\bin\Drill2Gerber.exe" drill_file.drl

"C:\Program Files\Gerber2PDF\bin\Gerber2pdf.exe" -nowarnings -output=Combined top_silk.grb bottom_silk.grb top_copper.grb inner_copper_1.grb inner_copper_2.grb bottom_copper.grb top_solder_mask.grb bottom_solder_mask.grb drill_file.drl.grb board_outline.grb -combine -colour=255,0,0 top_copper.grb -colour=0,128,0,200 top_solder_mask.grb -colour=191,191,0,200 top_silk.grb -colour=0,0,255 drill_file.drl.grb board_outline.grb -combine -mirror -colour=255,0,0 bottom_copper.grb -colour=0,128,0,200 bottom_solder_mask.grb -colour=191,191,0,200 bottom_silk.grb -colour=0,0,255 drill_file.drl.grb board_outline.grb

