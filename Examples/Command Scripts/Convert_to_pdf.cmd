::==============================================================================
:: Copyright (C) Robert Offner
::
:: This file is free software: you can redistribute it and/or modify
:: it under the terms of the GNU General Public License as published by
:: the Free Software Foundation, either version 3 of the License, or
:: (at your option) any later version.
::
:: This program is distributed in the hope that it will be useful,
:: but WITHOUT ANY WARRANTY; without even the implied warranty of
:: MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
:: GNU General Public License for more details.
::
:: You should have received a copy of the GNU General Public License
:: along with this program.  If not, see <http://www.gnu.org/licenses/>
::============================================================================== 

:: v1.2
:: Changed comments around project name and removed ""
:: Changed ECHO in check for drl. and .NPTH file.
:: Changed output filename

:: V1.0
:: Initial Release

:: Convert_to_pdf.bat
:: This script uses Gerber2pdf to convert multiple gerber files from kicad into one pdf. Useful for etching
:: It checks for a bottom layer and for a top layer
:: It also checks for a drill file (Excellon) and a NPTH file (non plated through hole).
:: Colours and mirroring can be adjusted in Line 57 and 58 but it should work as it is.
:: **********************************************
:: YOU HAVE TO CHANGE THE PROJECTNAME IN LINE 46 *
:: **********************************************
ECHO OFF
SETLOCAL ENABLEEXTENSIONS
SET me=%~n0      REM This is the scripts name without .cmd
SET parent=%~dp0 REM This is the path where the script is in

cls
:: Path to program
SET g2p="C:\Program Files (x86)\Gerber2pdf\Gerber2pdf.exe"
SET d2g="C:\Program Files (x86)\Gerber2pdf\Drill2Gerber.exe"
::SET d2g="D:\my_documents\Downloads\Drill2Gerber\bin\Drill2Gerber.exe"

REM +++++++++ CHANGE ME ++++++++++
SET projectname=Lauflicht
REM +++++++++ CHANGE ME ++++++++++

SET ol=%projectname%-Edge.Cuts.gbr
SET bot=%projectname%-B.Cu.gbr
SET topl=%projectname%-F.Cu.gbr
SET drl=%projectname%.drl
SET npth=%projectname%-NPTH.drl
SET drlg=%projectname%.drl.grb
SET npthg=%projectname%-NPTH.drl.grb
SET outputname="printout_%projectname%"
SET g2pb=%g2p% -silentexit -output=%outputname%_BOT -background=255,255,255 -combine -nomirror -colour=0,0,0 %ol% -colour=0,0,0 %bot%
SET g2pt=%g2p% -silentexit -output=%outputname%_TOP -background=255,255,255 -combine -mirror -colour=0,0,0 %ol% -colour=0,0,0 %topl%
ECHO "Starting Conversion......"
:: Converting Excellon drillfiles to gerber
IF EXIST %drl% (
	ECHO "Found %drl% file"
	%d2g% %drl%
)
IF EXIST %npth% (
	ECHO "Found %npth% file"
	%d2g% %npth%	
)
:: EXIT

:: Merging all the gerbers and converting them to pdf.
IF EXIST %bot% (
	ECHO "Found Bottom Layer"
	IF EXIST %drlg% (
	REM we have a drill file
		IF EXIST %npthg% (
		REM we have a npth file too
			ECHO "Merging Bottom, Outline, Drill and NPTH Files"
			%g2pb% -colour=255,255,255 %drlg% -colour=0,0,0 %npthg% 
		) ELSE (
		REM we just have the drill file
			ECHO "Merging Bottom, Outline and Drill Files"
			%g2pb% -colour=255,255,255 %drlg%
		)
	) ELSE (
		IF EXIST %npthg% (
			REM we have just a npth file 
			ECHO "Merging Bottom, Outline and NPTH Files"
			%g2pb% -colour=0,0,0 %npthg% 
		) ELSE (
			REM we have NO drill files
			ECHO "Merging Bottom and Outline Files"
			%g2pb%
		)
	)
)
:: Check if there is a Top Layer, if yes convert it too.
IF EXIST %topl% (
	ECHO "Found TOP Layer"
	IF EXIST %drlg% (
		REM we have a drill file
		IF EXIST %npthg% (
			REM we have a npth file too
			ECHO "Merging Top, Outline, Drill and NPTH Files"
			%g2pt% -colour=255,255,255 %drlg% -colour=0,0,0 %npthg% 
		) ELSE (
			REM we just have the drill file
			ECHO "Merging Top, Outline and Drill Files"
			%g2pt% -colour=255,255,255 %drlg%
		)
	) ELSE (
		IF EXIST %npthg% (
			REM we have just a npth file 
			ECHO "Merging Top, Outline and NPTH Files"
			%g2pt% -colour=0,0,0 %npthg% 
		) ELSE (
			REM we have NO drill files
			ECHO "Merging Top and Outline Files"
			%g2pt%
		)
	)
)
