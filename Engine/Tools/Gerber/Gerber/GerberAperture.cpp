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

#include "GerberAperture.h"
//------------------------------------------------------------------------------

#define pi 3.141592653589793238463
//------------------------------------------------------------------------------

GerberAperture::GerberAperture(){
    Code = 0;
    Type = tCircle;

    DimensionX = -1.0;
    DimensionY = -1.0;
    HoleX      = -1.0;
    HoleY      = -1.0;

    Left   =  1e3;
    Bottom =  1e3;
    Right  = -1e3;
    Top    = -1e3;

    SideCount = 0;
    Rotation  = 0.0;

    RenderList = 0;
    LastRender = 0;
}
//------------------------------------------------------------------------------

GerberAperture::~GerberAperture(){
    GerberRender* Temp;

    while(RenderList){
        Temp = RenderList;
        RenderList = RenderList->Next;
        delete Temp;
    }
}
//------------------------------------------------------------------------------

void GerberAperture::Circle(double d){
    Type       = tCircle;
    DimensionX = d;

    Left   = -d/2.0;
    Bottom = -d/2.0;
    Right  =  d/2.0;
    Top    =  d/2.0;
}
//------------------------------------------------------------------------------

void GerberAperture::Rectangle(double w, double h){
    Type       = tRectangle;
    DimensionX = w;
    DimensionY = h;

    Left   = -w/2.0;
    Bottom = -h/2.0;
    Right  =  w/2.0;
    Top    =  h/2.0;
}
//------------------------------------------------------------------------------

void GerberAperture::Obround(double w, double h){
    Type       = tObround;
    DimensionX = w;
    DimensionY = h;

    Left   = -w/2.0;
    Bottom = -h/2.0;
    Right  =  w/2.0;
    Top    =  h/2.0;
}
//------------------------------------------------------------------------------

void GerberAperture::Polygon(double w, int n, double a){
    Type       = tPolygon;
    DimensionX = w;
    SideCount  = n;
    Rotation   = a;

    Left   = -w/2.0;
    Bottom = -w/2.0;
    Right  =  w/2.0;
    Top    =  w/2.0;
}
//------------------------------------------------------------------------------

void GerberAperture::HoleCircle(double d){
    HoleX =  d;
    HoleY = -1.0;
}
//------------------------------------------------------------------------------

void GerberAperture::HoleRectangle(double w, double h){
    HoleX = w;
    HoleY = h;
}
//------------------------------------------------------------------------------

void GerberAperture::UseMacro(
    GerberMacro* Macro,
    double*      Modifiers,
    int          ModifierCount
){
    GerberRender* Render;
    double x, y, l, b, r, t;
    double c, d, e; // Temporary variables

    x = y = 0.0;

    Type       = tMacro;
    RenderList =  Macro->Render(Modifiers, ModifierCount);

    // Calculate bounding box
    Render = RenderList;
    while(Render){
        switch(Render->Command){
            case gcRectangle:
                l = Render->X;
                b = Render->Y;
                r = Render->X + Render->W;
                t = Render->Y + Render->H;
                x = l;
                y = b;
                break;

            case gcCircle:
                l = Render->X - Render->W/2.0;
                b = Render->Y - Render->W/2.0;
                r = Render->X + Render->W/2.0;
                t = Render->Y + Render->W/2.0;
                x = r;
                y = 0.0;
                break;

            case gcBeginLine:
                l = Render->X;
                b = Render->Y;
                r = Render->X;
                t = Render->Y;
                x = l;
                y = b;
                break;

            case gcLine:
                l = Render->X;
                b = Render->Y;
                r = Render->X;
                t = Render->Y;
                x = l;
                y = b;
                break;

            case gcArc:
                c = x - Render->X;   // Start relative to center
                d = y - Render->Y;   // End relative to center
                e = sqrt(c*c + d*d); // Radius
                c = atan2(d, c) + Render->A * pi/180.0; // Angle of end point

                // Play safe and assume it is a full circle
                l = Render->X - e;
                b = Render->Y - e;
                r = Render->X + e;
                t = Render->Y + e;

                x = Render->X + e*cos(c);
                y = Render->Y + e*sin(c);
                break;

            default:
                l = r = x;
                b = t = y;
                break;
        }

        if(Render == RenderList){
            Left   = l;
            Bottom = b;
            Right  = r;
            Top    = t;
        }else{
            if(Left   > l) Left   = l;
            if(Bottom > b) Bottom = b;
            if(Right  < r) Right  = r;
            if(Top    < t) Top    = t;
        }

        Render = Render->Next;
    }
}
//------------------------------------------------------------------------------

bool GerberAperture::SolidCircle(){
    return(Type == tCircle && HoleX < 0.0 && HoleY < 0.0);
}
//------------------------------------------------------------------------------

bool GerberAperture::SolidRectangle(){
    return(Type == tRectangle && HoleX < 0.0 && HoleY < 0.0);
}
//------------------------------------------------------------------------------

void GerberAperture::Add(GerberRender* Render){
    if(RenderList){
        LastRender->Next = Render;
    }else{
        RenderList = Render;
    }
    LastRender = Render;
}
//------------------------------------------------------------------------------

void GerberAperture::RenderHole(){
    GerberRender* Render;

    if(HoleX > 0.0){
        if(HoleY < 0.0){
            Render = new GerberRender;
            Render->Command = gcBeginLine;
            Render->X       = HoleX/2.0;
            Render->Y       = 0.0;
            Add(Render);

            Render = new GerberRender;
            Render->Command = gcArc;
            Render->X       =    0.0;
            Render->Y       =    0.0;
            Render->A       = -360.0;
            Add(Render);

        }else{
            Render = new GerberRender;
            Render->Command = gcBeginLine;
            Render->X       =  HoleX/2.0;
            Render->Y       = -HoleY/2.0;
            Add(Render);

            Render = new GerberRender;
            Render->Command = gcLine;
            Render->X       = -HoleX/2.0;
            Render->Y       = -HoleY/2.0;
            Add(Render);

            Render = new GerberRender;
            Render->Command = gcLine;
            Render->X       = -HoleX/2.0;
            Render->Y       =  HoleY/2.0;
            Add(Render);

            Render = new GerberRender;
            Render->Command = gcLine;
            Render->X       = HoleX/2.0;
            Render->Y       = HoleY/2.0;
            Add(Render);

            Render = new GerberRender;
            Render->Command = gcClose;
            Add(Render);
        }
    }
}
//------------------------------------------------------------------------------

void GerberAperture::RenderObround(){
    GerberRender* Render;

    double r, t;

    if(DimensionX > DimensionY){
        r = DimensionY/2.0;
        t = DimensionX/2.0 - r;

        Render = new GerberRender;
        Render->Command = gcBeginLine;
        Render->X       =  t;
        Render->Y       = -r;
        Add(Render);

        Render = new GerberRender;
        Render->Command = gcArc;
        Render->X       =     t;
        Render->Y       =   0.0;
        Render->A       = 180.0;
        Add(Render);

        Render = new GerberRender;
        Render->Command = gcLine;
        Render->X       = -t;
        Render->Y       =  r;
        Add(Render);

        Render = new GerberRender;
        Render->Command = gcArc;
        Render->X       =    -t;
        Render->Y       =   0.0;
        Render->A       = 180.0;
        Add(Render);

    }else{
        r = DimensionX/2.0;
        t = DimensionY/2.0 - r;

        Render = new GerberRender;
        Render->Command = gcBeginLine;
        Render->X       = r;
        Render->Y       = t;
        Add(Render);

        Render = new GerberRender;
        Render->Command = gcArc;
        Render->X       =   0.0;
        Render->Y       =     t;
        Render->A       = 180.0;
        Add(Render);

        Render = new GerberRender;
        Render->Command = gcLine;
        Render->X       = -r;
        Render->Y       = -t;
        Add(Render);

        Render = new GerberRender;
        Render->Command = gcArc;
        Render->X       =   0.0;
        Render->Y       =    -t;
        Render->A       = 180.0;
        Add(Render);
    }

    Render = new GerberRender;
    Render->Command = gcClose;
    Add(Render);

    RenderHole();

    Render = new GerberRender;
    Render->Command = gcFill;
    Add(Render);
}
//------------------------------------------------------------------------------

void GerberAperture::RenderCircle(){
    GerberRender* Render;

    Render = new GerberRender;
    Render->Command = gcCircle;
    Render->X       = 0.0;
    Render->Y       = 0.0;
    Render->W       = DimensionX;
    Add(Render);

    RenderHole();

    Render = new GerberRender;
    Render->Command = gcFill;
    Add(Render);
}
//------------------------------------------------------------------------------

void GerberAperture::RenderPolygon(){
    double        r, a, da, rot, lim;
    GerberRender* Render;

    // Get Rotation in range [0; 360)
    while(Rotation <    0.0) Rotation += 360;
    while(Rotation >= 360.0) Rotation -= 360;

    r   = DimensionX/2.0;
    da  = 2.0*pi / SideCount;
    rot = Rotation * pi/180.0;
    lim = 2.0*pi - da/2.0;

    Render = new GerberRender;
    Render->Command = gcBeginLine;
    Render->X       = r*cos(rot);
    Render->Y       = r*sin(rot);
    Add(Render);

    for(a = da; a < lim; a += da){
        Render = new GerberRender;
        Render->Command = gcLine;
        Render->X       = r*cos(a+rot);
        Render->Y       = r*sin(a+rot);
        Add(Render);
    }

    Render = new GerberRender;
    Render->Command = gcClose;
    Add(Render);

    RenderHole();

    Render = new GerberRender;
    Render->Command = gcFill;
    Add(Render);
}
//------------------------------------------------------------------------------

void GerberAperture::RenderRectangle(){
    GerberRender* Render;

    Render = new GerberRender;
    Render->Command = gcRectangle;
    Render->X       = -DimensionX/2.0;
    Render->Y       = -DimensionY/2.0;
    Render->W       =  DimensionX;
    Render->H       =  DimensionY;
    Add(Render);

    RenderHole();

    Render = new GerberRender;
    Render->Command = gcFill;
    Add(Render);
}
//------------------------------------------------------------------------------

void GerberAperture::RenderAperture(){
    switch(Type){
        case tCircle:
            RenderCircle();
            break;

        case tRectangle:
            RenderRectangle();
            break;

        case tObround:
            RenderObround();
            break;

        case tPolygon:
            RenderPolygon();
            break;

        case tMacro:
        default:
            break;
    }
}
//------------------------------------------------------------------------------

GerberRender* GerberAperture::Render(){
    if(!RenderList) RenderAperture();
    return RenderList;
}
//------------------------------------------------------------------------------
