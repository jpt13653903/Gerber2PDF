//==============================================================================
// Copyright (C) John-Philip Taylor
// jpt13653903@gmail.com
//
// This file is part of a library
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

#include "pdfHyperlink.h"
//------------------------------------------------------------------------------

pdfHyperlink::pdfHyperlink(){
    Subtype.Set("Link");

    Border.Add(&Zero);
    Border.Add(&Zero);
    Border.Add(&BorderWidth);

    Update();
}
//------------------------------------------------------------------------------

void pdfHyperlink::Update(){
    Clear();

                           AddEntry("Type"   , &Type   );
                           AddEntry("Subtype", &Subtype);
    if(Action.GetCount()){ AddEntry("A"      , &Action ); }
                           AddEntry("Rect"   , &Rect   );
    if(Dest  .GetCount()){ AddEntry("Dest"   , &Dest   ); }
                           AddEntry("Border" , &Border );
}
//------------------------------------------------------------------------------

void pdfHyperlink::DestFit(pdfPage* Page){
    Action.Clear();
    Dest  .Fit  (Page);
    Update();
}
//------------------------------------------------------------------------------

void pdfHyperlink::DestFitB(pdfPage* Page){
    Action.Clear();
    Dest  .FitB (Page);
    Update();
}
//------------------------------------------------------------------------------

void pdfHyperlink::DestFitH(pdfPage* Page, double Top){
    Action.Clear();
    Dest  .FitH (Page, Top);
    Update();
}
//------------------------------------------------------------------------------

void pdfHyperlink::DestFitV(pdfPage* Page, double Left){
    Action.Clear();
    Dest  .FitV (Page, Left);
    Update();
}
//------------------------------------------------------------------------------

void pdfHyperlink::DestFitBH(pdfPage* Page, double Top){
    Action.Clear();
    Dest  .FitBH(Page, Top);
    Update();
}
//------------------------------------------------------------------------------

void pdfHyperlink::DestFitBV(pdfPage* Page, double Left){
    Action.Clear();
    Dest  .FitBV(Page, Left);
    Update();
}
//------------------------------------------------------------------------------

void pdfHyperlink::DestXYZ(
    pdfPage* Page,
    double   Left,
    double   Top,
    double   Zoom
){
    Action.Clear();
    Dest  .XYZ  (Page, Left, Top, Zoom);
    Update();
}
//------------------------------------------------------------------------------

void pdfHyperlink::DestFitR(
    pdfPage* Page,
    double   Left,
    double   Bottom,
    double   Right,
    double   Top
){
    Action.Clear();
    Dest  .FitR (Page, Left, Bottom, Right, Top);
    Update();
}
//------------------------------------------------------------------------------

void pdfHyperlink::DestURI(const char* URI){
    Dest.Clear();

    URISubtype       .Set("URI");
    pdfHyperlink::URI.Set( URI );

    Action.AddEntry("S"  , & URISubtype);
    Action.AddEntry("URI", &(pdfHyperlink::URI));

    Update();
}
//------------------------------------------------------------------------------
