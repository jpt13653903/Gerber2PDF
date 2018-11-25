echo off

"..\..\Source\bin\Gerber2pdf.exe"                                                     ^
  -nowarnings                                                                         ^
  -silentexit                                                                         ^
  -output=Output                                                                      ^
    PCB_Fabrication_Data_in_Gerber_Example_1_Legend$Top.gbr                           ^
    PCB_Fabrication_Data_in_Gerber_Example_1_Legend$Bot.gbr                           ^
    PCB_Fabrication_Data_in_Gerber_Example_1_Copper$L1.gbr                            ^
    PCB_Fabrication_Data_in_Gerber_Example_1_Copper$L2.gbr                            ^
    PCB_Fabrication_Data_in_Gerber_Example_1_Copper$L3.gbr                            ^
    PCB_Fabrication_Data_in_Gerber_Example_1_Copper$L4.gbr                            ^
    PCB_Fabrication_Data_in_Gerber_Example_1_Soldermask$Top.gbr                       ^
    PCB_Fabrication_Data_in_Gerber_Example_1_Soldermask$Bot.gbr                       ^
    PCB_Fabrication_Data_in_Gerber_Example_1_PTH$Drill.gbr                            ^
    PCB_Fabrication_Data_in_Gerber_Example_1_NPTH$Drill.gbr                           ^
    PCB_Fabrication_Data_in_Gerber_Example_1_NPTH$Rout.gbr                            ^
    PCB_Fabrication_Data_in_Gerber_Example_1_Profile.gbr                              ^
  -combine                                                                            ^
    -colour=255,0,0       PCB_Fabrication_Data_in_Gerber_Example_1_Copper$L1.gbr      ^
    -colour=0,128,0,200   PCB_Fabrication_Data_in_Gerber_Example_1_Soldermask$Top.gbr ^
    -colour=191,191,0,200 PCB_Fabrication_Data_in_Gerber_Example_1_Legend$Top.gbr     ^
    -colour=0,128,255     PCB_Fabrication_Data_in_Gerber_Example_1_PTH$Drill.gbr      ^
    -colour=0,0,255       PCB_Fabrication_Data_in_Gerber_Example_1_NPTH$Drill.gbr     ^
                          PCB_Fabrication_Data_in_Gerber_Example_1_Profile.gbr        ^
  -combine -mirror                                                                    ^
    -colour=255,0,0       PCB_Fabrication_Data_in_Gerber_Example_1_Copper$L4.gbr      ^
    -colour=0,128,0,200   PCB_Fabrication_Data_in_Gerber_Example_1_Soldermask$Bot.gbr ^
    -colour=191,191,0,200 PCB_Fabrication_Data_in_Gerber_Example_1_Legend$Bot.gbr     ^
    -colour=0,128,255     PCB_Fabrication_Data_in_Gerber_Example_1_PTH$Drill.gbr      ^
    -colour=0,0,255       PCB_Fabrication_Data_in_Gerber_Example_1_NPTH$Drill.gbr     ^
                          PCB_Fabrication_Data_in_Gerber_Example_1_Profile.gbr        ^
  -combine -nomirror                                                                  ^
    -colour=0,255,0       PCB_Fabrication_Data_in_Gerber_Example_1_Copper$L4.gbr      ^
    -colour=255,0,0       PCB_Fabrication_Data_in_Gerber_Example_1_Copper$L1.gbr      ^
    -colour=0,128,0,200   PCB_Fabrication_Data_in_Gerber_Example_1_Soldermask$Top.gbr ^
    -colour=191,191,0,200 PCB_Fabrication_Data_in_Gerber_Example_1_Legend$Top.gbr     ^
    -colour=0,128,255     PCB_Fabrication_Data_in_Gerber_Example_1_PTH$Drill.gbr      ^
    -colour=0,0,255       PCB_Fabrication_Data_in_Gerber_Example_1_NPTH$Drill.gbr     ^
    -colour=0,0,255,128   PCB_Fabrication_Data_in_Gerber_Example_1_Profile.gbr

explorer Output.pdf

