//==============================================================================
// Copyright (C) John-Philip Taylor
// jpt13653903@gmail.com
//
// This file is part of a library that conforms to the ZLib and Deflate
// standards
//
// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>
//==============================================================================

#ifndef GerberLevel_h
#define GerberLevel_h
//------------------------------------------------------------------------------

#include <math.h>
//------------------------------------------------------------------------------

#include <stdio.h>
#include "General.h"
#include "GerberEnums.h"
#include "GerberRender.h"
//------------------------------------------------------------------------------

extern bool GerberWarnings;
//------------------------------------------------------------------------------

class GerberLevel{
  private: // Standard private members and function
    GerberRender* RenderList; // Linked list of render commands
    GerberRender* LastRender; // Last render command used for easy additions

    void          Add   (GerberRender*  Render );
    GerberRender* AddNew(GERBER_COMMAND Command);

    bool   Path; // Busy with path definition
    bool   OutlineFill;
    double pX, pY; // Previous point used for relative data
    double fX, fY; // First point of polygon

    double Get_mm(double Number);

    double GetAngle(
      double x1, double y1, // Start, relative to center
      double x2, double y2  // End, relative to center
    );

    void Move (unsigned LineNumber);
    void Line ();
    void Arc  ();
    void Flash();

    GerberAperture* CurrentAperture;
  //----------------------------------------------------------------------------

  private: // Specifically used by JoinSegments()
    class Segment{
      private:
        bool Closed = false; // true => closed; false => maybe closed

      public:
        GerberRender* CommandList = 0;
        GerberRender* LastCommand = 0;
        Segment*      Prev        = 0;
        Segment*      Next        = 0;

        void Add     (GerberRender* Command);
        bool IsClosed();
        void Reverse ();
        void Isolate ();
    };
    Segment* SegmentList = 0;
    Segment* LastSegment = 0;

    Segment* FindNeighbour(Segment* Current);

    void NewSegment     ();
    void ExtractSegments();
    void JoinSegments   ();
    void AddSegments    ();
  //----------------------------------------------------------------------------

  public: // Public interface
    GerberLevel(GerberLevel* PreviousLevel, GERBER_UNIT Units);
   ~GerberLevel();

    GerberLevel* Next;

    // Image bounding box
    double Left;
    double Bottom;
    double Right;
    double Top;

    // Step-and-Repeat
    int    CountX, CountY;
    double StepX , StepY;

    void SetName(const char* Name);

    char* Name; // null for default level
    bool  Negative;
    bool  Relative;
    bool  Incremental;
    bool  Multiquadrant;

    double X, Y, I, J;

    GERBER_UNIT          Units;
    GERBER_EXPOSURE      Exposure;
    GERBER_INTERPOLATION Interpolation;

    void ApertureSelect(GerberAperture* Aperture, unsigned LineNumber);

    void OutlineBegin(unsigned LineNumber);
    void OutlineEnd  (unsigned LineNumber);

    void Do(unsigned LineNumber);

    // Linked list of render commands
    // Memory freed automatically
    GerberRender* Render();

    // Forms a single area out of the various line and arc segments in the 
    // layer, typically used to obtain a solid board from an outline.
    void ConvertStrokesToFills();
};
//------------------------------------------------------------------------------

#endif
//------------------------------------------------------------------------------

