echo off

"..\..\Source\bin\Gerber2pdf.exe"                                        ^
  -nowarnings                                                            ^
  -output=Output                                                         ^
    Gerber_PCB_Data_-_Example_1_Legend$Top.gbr                           ^
    Gerber_PCB_Data_-_Example_1_Legend$Bot.gbr                           ^
    Gerber_PCB_Data_-_Example_1_Copper$L1.gbr                            ^
    Gerber_PCB_Data_-_Example_1_Copper$L2.gbr                            ^
    Gerber_PCB_Data_-_Example_1_Copper$L3.gbr                            ^
    Gerber_PCB_Data_-_Example_1_Copper$L4.gbr                            ^
    Gerber_PCB_Data_-_Example_1_Soldermask$Top.gbr                       ^
    Gerber_PCB_Data_-_Example_1_Soldermask$Bot.gbr                       ^
    Gerber_PCB_Data_-_Example_1_PTH$Drill.gbr                            ^
    Gerber_PCB_Data_-_Example_1_NPTH$Drill.gbr                           ^
    Gerber_PCB_Data_-_Example_1_NPTH$Rout.gbr                            ^
    Gerber_PCB_Data_-_Example_1_Profile.gbr                              ^
  -combine                                                               ^
    -colour=255,0,0       Gerber_PCB_Data_-_Example_1_Copper$L1.gbr      ^
    -colour=0,128,0,200   Gerber_PCB_Data_-_Example_1_Soldermask$Top.gbr ^
    -colour=191,191,0,200 Gerber_PCB_Data_-_Example_1_Legend$Top.gbr     ^
    -colour=0,128,255     Gerber_PCB_Data_-_Example_1_PTH$Drill.gbr      ^
    -colour=0,0,255       Gerber_PCB_Data_-_Example_1_NPTH$Drill.gbr     ^
                          Gerber_PCB_Data_-_Example_1_NPTH$Rout.gbr      ^
                          Gerber_PCB_Data_-_Example_1_Profile.gbr        ^
  -combine -mirror                                                       ^
    -colour=255,0,0       Gerber_PCB_Data_-_Example_1_Copper$L4.gbr      ^
    -colour=0,128,0,200   Gerber_PCB_Data_-_Example_1_Soldermask$Bot.gbr ^
    -colour=191,191,0,200 Gerber_PCB_Data_-_Example_1_Legend$Bot.gbr     ^
    -colour=0,128,255     Gerber_PCB_Data_-_Example_1_PTH$Drill.gbr      ^
    -colour=0,0,255       Gerber_PCB_Data_-_Example_1_NPTH$Drill.gbr     ^
                          Gerber_PCB_Data_-_Example_1_NPTH$Rout.gbr      ^
                          Gerber_PCB_Data_-_Example_1_Profile.gbr

explorer Output.pdf

