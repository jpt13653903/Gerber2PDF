echo off

"..\..\Source\bin\Gerber2pdf.exe"                                                                ^
  -nowarnings                                                                                    ^
  -silentexit                                                                                    ^
  -output=Output                                                                                 ^
    PCB_Fabrication_Data_in_Gerber_Example_2_Legend$Top.gbr                                      ^
    PCB_Fabrication_Data_in_Gerber_Example_2_Legend$Bot.gbr                                      ^
    PCB_Fabrication_Data_in_Gerber_Example_2_Copper$L1$Top.gbr                                   ^
    PCB_Fabrication_Data_in_Gerber_Example_2_Copper$L2$Inr.gbr                                   ^
    PCB_Fabrication_Data_in_Gerber_Example_2_Copper$L3$Inr.gbr                                   ^
    PCB_Fabrication_Data_in_Gerber_Example_2_Copper$L4$Inr.gbr                                   ^
    PCB_Fabrication_Data_in_Gerber_Example_2_Copper$L5$Inr.gbr                                   ^
    PCB_Fabrication_Data_in_Gerber_Example_2_Copper$L6$Inr.gbr                                   ^
    PCB_Fabrication_Data_in_Gerber_Example_2_Copper$L7$Inr.gbr                                   ^
    PCB_Fabrication_Data_in_Gerber_Example_2_Copper$L8$Inr.gbr                                   ^
    PCB_Fabrication_Data_in_Gerber_Example_2_Copper$L9$Inr.gbr                                   ^
    PCB_Fabrication_Data_in_Gerber_Example_2_Copper$L10$Bot.gbr                                  ^
    PCB_Fabrication_Data_in_Gerber_Example_2_Soldermask$Top.gbr                                  ^
    PCB_Fabrication_Data_in_Gerber_Example_2_Soldermask$Bot.gbr                                  ^
    PCB_Fabrication_Data_in_Gerber_Example_2_Plated$1$10$PTH$Drill.gbr                           ^
    PCB_Fabrication_Data_in_Gerber_Example_2_Plated$1$2$Blind$Drill.gbr                          ^
    PCB_Fabrication_Data_in_Gerber_Example_2_Plated$2$9$Buried$Drill.gbr                         ^
    PCB_Fabrication_Data_in_Gerber_Example_2_Plated$9$10$Blind$Drill.gbr                         ^
    PCB_Fabrication_Data_in_Gerber_Example_2_NonPlated$1$10$NPTH$Drill.gbr                       ^
    PCB_Fabrication_Data_in_Gerber_Example_2_Profile$NP.gbr                                      ^
  -combine                                                                                       ^
    -colour=255,0,0       PCB_Fabrication_Data_in_Gerber_Example_2_Copper$L1$Top.gbr             ^
    -colour=0,128,0,200   PCB_Fabrication_Data_in_Gerber_Example_2_Soldermask$Top.gbr            ^
    -colour=191,191,0,200 PCB_Fabrication_Data_in_Gerber_Example_2_Legend$Top.gbr                ^
    -colour=0,128,255     PCB_Fabrication_Data_in_Gerber_Example_2_Plated$1$10$PTH$Drill.gbr     ^
                          PCB_Fabrication_Data_in_Gerber_Example_2_Plated$1$2$Blind$Drill.gbr    ^
    -colour=0,0,255       PCB_Fabrication_Data_in_Gerber_Example_2_NonPlated$1$10$NPTH$Drill.gbr ^
                          PCB_Fabrication_Data_in_Gerber_Example_2_Profile$NP.gbr                ^
  -combine -mirror                                                                               ^
    -colour=255,0,0       PCB_Fabrication_Data_in_Gerber_Example_2_Copper$L10$Bot.gbr            ^
    -colour=0,128,0,200   PCB_Fabrication_Data_in_Gerber_Example_2_Soldermask$Bot.gbr            ^
    -colour=191,191,0,200 PCB_Fabrication_Data_in_Gerber_Example_2_Legend$Bot.gbr                ^
    -colour=0,128,255     PCB_Fabrication_Data_in_Gerber_Example_2_Plated$1$10$PTH$Drill.gbr     ^
                          PCB_Fabrication_Data_in_Gerber_Example_2_Plated$9$10$Blind$Drill.gbr   ^
    -colour=0,0,255       PCB_Fabrication_Data_in_Gerber_Example_2_NonPlated$1$10$NPTH$Drill.gbr ^
                          PCB_Fabrication_Data_in_Gerber_Example_2_Profile$NP.gbr                ^
  -combine -nomirror                                                                             ^
    -colour=0,255,0,192   PCB_Fabrication_Data_in_Gerber_Example_2_Copper$L10$Bot.gbr            ^
    -colour=0,0,255,192   PCB_Fabrication_Data_in_Gerber_Example_2_Copper$L9$Inr.gbr             ^
    -colour=0,128,255     PCB_Fabrication_Data_in_Gerber_Example_2_Plated$9$10$Blind$Drill.gbr   ^
    -colour=0,128,128,192 PCB_Fabrication_Data_in_Gerber_Example_2_Copper$L6$Inr.gbr             ^
    -colour=255,255,0,192 PCB_Fabrication_Data_in_Gerber_Example_2_Copper$L5$Inr.gbr             ^
    -colour=255,0,255,192 PCB_Fabrication_Data_in_Gerber_Example_2_Copper$L2$Inr.gbr             ^
    -colour=0,128,255     PCB_Fabrication_Data_in_Gerber_Example_2_Plated$2$9$Buried$Drill.gbr   ^
    -colour=255,0,0,192   PCB_Fabrication_Data_in_Gerber_Example_2_Copper$L1$Top.gbr             ^
    -colour=0,128,255     PCB_Fabrication_Data_in_Gerber_Example_2_Plated$1$2$Blind$Drill.gbr    ^
    -colour=0,128,255     PCB_Fabrication_Data_in_Gerber_Example_2_Plated$1$10$PTH$Drill.gbr     ^
    -colour=0,0,255       PCB_Fabrication_Data_in_Gerber_Example_2_NonPlated$1$10$NPTH$Drill.gbr ^
                          PCB_Fabrication_Data_in_Gerber_Example_2_Profile$NP.gbr                                        

explorer Output.pdf

