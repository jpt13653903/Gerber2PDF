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

#include "GerberMacro.h"
//------------------------------------------------------------------------------

#define pi 3.141592653589793238463
//------------------------------------------------------------------------------

GerberMacro::GerberMacro(){
    Name           = 0;
    Primitives     = 0;
    PrimitivesLast = 0;
    RenderList     = 0;
    RenderLast     = 0;
    Modifiers      = 0;
    Inches         = true;
    Exposure       = true;
    NewModifiers   = false;
}
//------------------------------------------------------------------------------

GerberMacro::~GerberMacro(){
    if(Name        ) delete   Name;
    if(Primitives  ) delete   Primitives;
    if(NewModifiers) delete[] Modifiers;
}
//------------------------------------------------------------------------------

GerberMacro::OPERATOR_ITEM::OPERATOR_ITEM(){
    Operator = opLiteral;
    Left     = 0;
    Right    = 0;
    Index    = 0;
    Value    = 0;
}
//------------------------------------------------------------------------------

GerberMacro::OPERATOR_ITEM::~OPERATOR_ITEM(){
    if(Left ) delete Left;
    if(Right) delete Right;
}
//------------------------------------------------------------------------------

GerberMacro::PRIMITIVE_ITEM::PRIMITIVE_ITEM(){
    Primitive     = pEndOfFile;
    Modifier      = 0;
    ModifierCount = 0;
    Index         = 0;
    Next          = 0;
}
//------------------------------------------------------------------------------

GerberMacro::PRIMITIVE_ITEM::~PRIMITIVE_ITEM(){
    int j;
    for(j = 0; j < ModifierCount; j++){
        if(Modifier[j]) delete Modifier[j];
    }

    if(Next) delete Next; // Hopefully this is a short list :-)
}
//------------------------------------------------------------------------------

void GerberMacro::Add(GerberRender* Render){
    if(RenderList){
        RenderLast->Next = Render;
    }else{
        RenderList = Render;
    }
    RenderLast = Render;
}
//------------------------------------------------------------------------------

double GerberMacro::Evaluate(OPERATOR_ITEM* Root){
    double Left, Right;

    if(!Root) return 0.0;

    switch(Root->Operator){
        case opAdd:
            Left  = Evaluate(Root->Left);
            Right = Evaluate(Root->Right);
            return Left + Right;

        case opSubtract:
            Left  = Evaluate(Root->Left);
            Right = Evaluate(Root->Right);
            return Left - Right;

        case opMultiply:
            Left  = Evaluate(Root->Left);
            Right = Evaluate(Root->Right);
            return Left * Right;

        case opDivide:
            Left  = Evaluate(Root->Left);
            Right = Evaluate(Root->Right);
            return Left / Right;

        case opVariable:
            if(Root->Index > 0 && Root->Index <= ModifierCount){
                return Modifiers[Root->Index-1];
            }else{
                return 0.0;
            }

        case opLiteral:
            return Root->Value;

        default:
            return 0.0;
    }
}
//------------------------------------------------------------------------------

double GerberMacro::Get_mm(double Number){
    if(Inches) Number *= 25.4;
    return Number;
}
//------------------------------------------------------------------------------

void GerberMacro::RenderLine(
    double x1, double y1,
    double x2, double y2,
    double x3, double y3,
    double x4, double y4,
    double xR, double yR,
    double A
){
    double r, a;

    GerberRender* Render;

    // Translate to center
    x1 -= xR;
    y1 -= yR;
    x2 -= xR;
    y2 -= yR;
    x3 -= xR;
    y3 -= yR;
    x4 -= xR;
    y4 -= yR;

    // Rotate the corners
    r  = sqrt (y1*y1 + x1*x1);
    a  = atan2(y1    , x1   ) + A;
    x1 = xR + r*cos(a);
    y1 = yR + r*sin(a);

    r  = sqrt (y2*y2 + x2*x2);
    a  = atan2(y2    , x2   ) + A;
    x2 = xR + r*cos(a);
    y2 = yR + r*sin(a);

    r  = sqrt (y3*y3 + x3*x3);
    a  = atan2(y3    , x3   ) + A;
    x3 = xR + r*cos(a);
    y3 = yR + r*sin(a);

    r  = sqrt (y4*y4 + x4*x4);
    a  = atan2(y4    , x4   ) + A;
    x4 = xR + r*cos(a);
    y4 = yR + r*sin(a);

    // Draw the line
    Render = new GerberRender;
    Render->Command = gcBeginLine;
    Render->X       = Get_mm(x1);
    Render->Y       = Get_mm(y1);
    Add(Render);

    Render = new GerberRender;
    Render->Command = gcLine;
    Render->X       = Get_mm(x2);
    Render->Y       = Get_mm(y2);
    Add(Render);

    Render = new GerberRender;
    Render->Command = gcLine;
    Render->X       = Get_mm(x3);
    Render->Y       = Get_mm(y3);
    Add(Render);

    Render = new GerberRender;
    Render->Command = gcLine;
    Render->X       = Get_mm(x4);
    Render->Y       = Get_mm(y4);
    Add(Render);

    Render = new GerberRender;
    Render->Command = gcClose;
    Add(Render);
}
//------------------------------------------------------------------------------

bool GerberMacro::RenderCircle(PRIMITIVE_ITEM* Primitive){
    int j;
    GerberRender* Render;

    const int ModifierCount = 5;

    double Modifier[ModifierCount];

    for(j = 0; j < ModifierCount; j++){
        if(j < Primitive->ModifierCount){
            Modifier[j] = Evaluate(Primitive->Modifier[j]);
        }else{
            Modifier[j] = 0.0;
        }
    }

    if      (Modifier[0] == 0.0){
        Exposure = false;
    }else if(Modifier[0] == 1.0){
        Exposure = true;
    }else{
        Exposure = !Exposure;
    }

    double d = Modifier[1];
    double x = Modifier[2];
    double y = Modifier[3];
    double a = Modifier[4] * pi/180.0;
    double s = sin(a);
    double c = cos(a);

    Render = new GerberRender;
    Render->Command = gcCircle; // Circles are rendered CCW
    Render->X       = Get_mm(c*x - s*y);
    Render->Y       = Get_mm(s*x + c*y);
    Render->W       = Get_mm(d);
    Add(Render);

    Render = new GerberRender;
    if(Exposure) Render->Command = gcFill;
    else         Render->Command = gcErase;
    Add(Render);

    return true;
}
//------------------------------------------------------------------------------

bool GerberMacro::RenderLineVector(PRIMITIVE_ITEM* Primitive){
    int j;
    GerberRender* Render;

    const int ModifierCount = 7;

    double Modifier[ModifierCount];

    for(j = 0; j < ModifierCount; j++){
        if(j < Primitive->ModifierCount){
            Modifier[j] = Evaluate(Primitive->Modifier[j]);
        }else{
            Modifier[j] = 0.0;
        }
    }

    if      (Modifier[0] == 0.0){
        Exposure = false;
    }else if(Modifier[0] == 1.0){
        Exposure = true;
    }else{
        Exposure = !Exposure;
    }

    double w , a ; // Width; Angle
    double a1, a2; // Angles of Start and End
    double r1, r2; // Radii of Start and End
    double x1, y1; // Start
    double x2, y2; // End
    double x3, y3; // Unit vector perpendicular to the line
    double x4, y4; // Corner1
    double x5, y5; // Corner2
    double x6, y6; // Corner3
    double x7, y7; // Corner4

    // Give the modifiers some names
    w  = Modifier[1];
    a  = Modifier[6] * pi/180.0;
    x1 = Modifier[2];
    y1 = Modifier[3];
    x2 = Modifier[4];
    y2 = Modifier[5];

    // Calculate the actual start and end points
    r1 = sqrt (y1*y1 + x1*x1);
    a1 = atan2(y1,     x1) + a;
    x1 = r1*cos(a1);
    y1 = r1*sin(a1);

    r2 = sqrt (y2*y2 + x2*x2);
    a2 = atan2(y2,     x2) + a;
    x2 = r2*cos(a2);
    y2 = r2*sin(a2);

    // Calculate the unit-vector perpendicular to the line
    a  = atan2(y2-y1, x2-x1);
    a += pi/2.0;
    x3 = cos(a);
    y3 = sin(a);

    // Scale the unit vector with the line width
    w  /= 2.0;
    x3 *= w;
    y3 *= w;

    // Calculate the corners
    x4  = x1 - x3;
    y4  = y1 - y3;
    x5  = x2 - x3;
    y5  = y2 - y3;
    x6  = x2 + x3;
    y6  = y2 + y3;
    x7  = x1 + x3;
    y7  = y1 + y3;

    // Draw the line
    Render = new GerberRender;
    Render->Command = gcBeginLine;
    Render->X       = Get_mm(x4);
    Render->Y       = Get_mm(y4);
    Add(Render);

    Render = new GerberRender;
    Render->Command = gcLine;
    Render->X       = Get_mm(x5);
    Render->Y       = Get_mm(y5);
    Add(Render);

    Render = new GerberRender;
    Render->Command = gcLine;
    Render->X       = Get_mm(x6);
    Render->Y       = Get_mm(y6);
    Add(Render);

    Render = new GerberRender;
    Render->Command = gcLine;
    Render->X       = Get_mm(x7);
    Render->Y       = Get_mm(y7);
    Add(Render);

    Render = new GerberRender;
    Render->Command = gcClose;
    Add(Render);

    Render = new GerberRender;
    if(Exposure) Render->Command = gcFill;
    else         Render->Command = gcErase;
    Add(Render);

    return true;
}
//------------------------------------------------------------------------------

bool GerberMacro::RenderLineCenter(PRIMITIVE_ITEM* Primitive){
    int j;

    const int ModifierCount = 6;

    double Modifier[ModifierCount];

    for(j = 0; j < ModifierCount; j++){
        if(j < Primitive->ModifierCount){
            Modifier[j] = Evaluate(Primitive->Modifier[j]);
        }else{
            Modifier[j] = 0.0;
        }
    }

    if      (Modifier[0] == 0.0){
        Exposure = false;
    }else if(Modifier[0] == 1.0){
        Exposure = true;
    }else{
        Exposure = !Exposure;
    }

    double a     ; // Rotation
    double w , h ; // Width and Height
    double x0, y0; // Center
    double x1, y1; // Corner 1
    double x2, y2; // Corner 2
    double x3, y3; // Corner 3
    double x4, y4; // Corner 4

    // Give the modifiers some names
    w  = Modifier[1];
    h  = Modifier[2];
    x0 = Modifier[3];
    y0 = Modifier[4];
    a  = Modifier[5] * pi/180.0;

    // Calculate the corners without rotation
    w /= 2.0;
    h /= 2.0;

    x1 = x0 - w;
    y1 = y0 - h;
    x2 = x0 + w;
    y2 = y0 - h;
    x3 = x0 + w;
    y3 = y0 + h;
    x4 = x0 - w;
    y4 = y0 + h;

    RenderLine(x1, y1, x2, y2, x3, y3, x4, y4, 0, 0, a);

    GerberRender* Render = new GerberRender;
    if(Exposure) Render->Command = gcFill;
    else         Render->Command = gcErase;
    Add(Render);

    return true;
}
//------------------------------------------------------------------------------

bool GerberMacro::RenderLineLowerLeft(PRIMITIVE_ITEM* Primitive){
    int j;

    const int ModifierCount = 6;

    double Modifier[ModifierCount];

    for(j = 0; j < ModifierCount; j++){
        if(j < Primitive->ModifierCount){
            Modifier[j] = Evaluate(Primitive->Modifier[j]);
        }else{
            Modifier[j] = 0.0;
        }
    }

    if      (Modifier[0] == 0.0){
        Exposure = false;
    }else if(Modifier[0] == 1.0){
        Exposure = true;
    }else{
        Exposure = !Exposure;
    }

    double a     ; // Rotation
    double w , h ; // Lower Left
    double x1, y1; // Corner 1
    double x2, y2; // Corner 2
    double x3, y3; // Corner 3
    double x4, y4; // Corner 4

    // Give the modifiers some names
    w  = Modifier[1];
    h  = Modifier[2];
    x1 = Modifier[3];
    y1 = Modifier[4];
    a  = Modifier[5] * pi/180.0;

    // Calculate the corners without rotation
    x2 = x1 + w;
    y2 = y1;
    x3 = x1 + w;
    y3 = y1 + h;
    x4 = x1;
    y4 = y1 + h;

    RenderLine(x1, y1, x2, y2, x3, y3, x4, y4, 0, 0, a);

    GerberRender* Render = new GerberRender;
    if(Exposure) Render->Command = gcFill;
    else         Render->Command = gcErase;
    Add(Render);

    return true;
}
//------------------------------------------------------------------------------

bool GerberMacro::RenderOutline(PRIMITIVE_ITEM* Primitive){
    int j;
    GerberRender* Render;

    int ModifierCount = Primitive->ModifierCount;

    double* Modifier = new double[ModifierCount];

    for(j = 0; j < ModifierCount; j++){
        Modifier[j] = Evaluate(Primitive->Modifier[j]);
    }

    if      (Modifier[0] == 0.0){
        Exposure = false;
    }else if(Modifier[0] == 1.0){
        Exposure = true;
    }else{
        Exposure = !Exposure;
    }

    int     N = (ModifierCount - 5.0) / 2.0 + 1.0; // Total number of points
    double  A; // Rotation;

    double  r, a; // Temporary
    double* x = new double[N]; // Points on polygon
    double* y = new double[N];

    for(j = 0; j < N; j++){
        x[j] = Modifier[2*j+2];
        y[j] = Modifier[2*j+3];
    }
    A = Modifier[2*j+2] * pi/180.0;

    // Make sure the polygon is closed
    x[N-1] = x[0];
    y[N-1] = y[0];

    // Rotate the points
    for(j = 0; j < N; j++){
        r    = sqrt (y[j]*y[j] + x[j]*x[j]);
        a    = atan2(y[j]      , x[j]) + A;
        x[j] = r*cos(a);
        y[j] = r*sin(a);
    }

    // Determine the wind
    bool   CCW;
    double Area = 0.0; // Actually double the area
    for(j = 0; j < N-1; j++){
        Area += (x[j]-x[j+1])*(y[j]+y[j+1]);
    }

    if(Area > 0.0){
        CCW = true;
    }else{
        CCW = false;
    }

    if(CCW){
        Render = new GerberRender;
        Render->Command = gcBeginLine;
        Render->X       = Get_mm(x[0]);
        Render->Y       = Get_mm(y[0]);
        Add(Render);

        for(j = 1; j < N-1; j++){
            Render = new GerberRender;
            Render->Command = gcLine;
            Render->X       = Get_mm(x[j]);
            Render->Y       = Get_mm(y[j]);
            Add(Render);
        }

    }else{
        Render = new GerberRender;
        Render->Command = gcBeginLine;
        Render->X       = Get_mm(x[N-1]);
        Render->Y       = Get_mm(y[N-1]);
        Add(Render);

        for(j = N-2; j > 0; j--){
            Render = new GerberRender;
            Render->Command = gcLine;
            Render->X       = Get_mm(x[j]);
            Render->Y       = Get_mm(y[j]);
            Add(Render);
        }
    }

    Render = new GerberRender;
    Render->Command = gcClose;
    Add(Render);

    Render = new GerberRender;
    if(Exposure) Render->Command = gcFill;
    else         Render->Command = gcErase;
    Add(Render);

    delete[] x;
    delete[] y;
    delete[] Modifier;

    return true;
}
//------------------------------------------------------------------------------

bool GerberMacro::RenderPolygon(PRIMITIVE_ITEM* Primitive){
    int j;
    GerberRender* Render;

    const int ModifierCount = 6;

    double Modifier[ModifierCount];

    for(j = 0; j < ModifierCount; j++){
        if(j < Primitive->ModifierCount){
            Modifier[j] = Evaluate(Primitive->Modifier[j]);
        }else{
            Modifier[j] = 0.0;
        }
    }

    if      (Modifier[0] == 0.0){
        Exposure = false;
    }else if(Modifier[0] == 1.0){
        Exposure = true;
    }else{
        Exposure = !Exposure;
    }

    int     N;
    double  X, Y, R, A;

    N = Modifier[1];
    X = Modifier[2];
    Y = Modifier[3];
    R = Modifier[4]/2.0;
    A = Modifier[5]*pi/180.0;

    double  r, a, da;
    double* x = new double[N];
    double* y = new double[N];

    da = 2.0*pi / N;

    r = sqrt (Y*Y + X*X);
    a = atan2(Y   , X  ) + A;
    X = r*cos(a);
    Y = r*sin(a);

    a = A;
    for(j = 0; j < N; j++){
        x[j] = X + R*cos(a);
        y[j] = Y + R*sin(a);
        a   += da;
    }

    Render = new GerberRender;
    Render->Command = gcBeginLine;
    Render->X       = Get_mm(x[0]);
    Render->Y       = Get_mm(y[0]);
    Add(Render);

    for(j = 1; j < N; j++){
        Render = new GerberRender;
        Render->Command = gcLine;
        Render->X       = Get_mm(x[j]);
        Render->Y       = Get_mm(y[j]);
        Add(Render);
    }

    Render = new GerberRender;
    Render->Command = gcClose;
    Add(Render);

    Render = new GerberRender;
    if(Exposure) Render->Command = gcFill;
    else         Render->Command = gcErase;
    Add(Render);

    delete[] x;
    delete[] y;

    return true;
}
//------------------------------------------------------------------------------

bool GerberMacro::RenderMoire(PRIMITIVE_ITEM* Primitive){
    int j;
    GerberRender* Render;

    const int ModifierCount = 9;

    double Modifier[ModifierCount];

    for(j = 0; j < ModifierCount; j++){
        if(j < Primitive->ModifierCount){
            Modifier[j] = Evaluate(Primitive->Modifier[j]);
        }else{
            Modifier[j] = 0.0;
        }
    }

    // Rings:
    double X, Y;      // Center
    double OD;        // Outer Diameter
    double Thickness; // Thickness
    double Gap;       // Gap
    int    N;         // Number

    // Cross hair:
    double Width;     // Thickness
    double Length;    // Length
    double Rotation;  // Rotation

    X         = Modifier[0];
    Y         = Modifier[1];
    OD        = Modifier[2];
    Thickness = Modifier[3];
    Gap       = Modifier[4];
    N         = Modifier[5];

    Width    = Modifier[6];
    Length   = Modifier[7];
    Rotation = Modifier[8];

    // Draw rings:
    double d = OD;
    for(j = 0; j < N; j++){
        if(d < Width) break;
        Render = new GerberRender;
        Render->Command = gcCircle;
        Render->X       = Get_mm(X);
        Render->Y       = Get_mm(Y);
        Render->W       = Get_mm(d);
        Add(Render);

        d -= Thickness*2.0;
        if(d < Width) break;
        Render = new GerberRender;
        Render->Command = gcBeginLine;
        Render->X       = Get_mm(X + d/2.0);
        Render->Y       = Get_mm(Y);
        Add(Render);
        Render = new GerberRender;
        Render->Command = gcArc;
        Render->X       = Get_mm(X);
        Render->Y       = Get_mm(Y);
        Render->A       = -360.0;
        Add(Render);

        d -= Gap*2.0;
    }

    // Draw the cross hairs
    double dx, dy;
    double x1, y1;
    double x2, y2;
    double x3, y3;
    double x4, y4;

    dx = Width /2.0;
    dy = Length/2.0;

    x1 = -dx;
    y1 = -dy;
    x2 = +dx;
    y2 = -dy;
    x3 = +dx;
    y3 = +dy;
    x4 = -dx;
    y4 = +dy;

    RenderLine(
        X+x1, Y+y1,
        X+x2, Y+y2,
        X+x3, Y+y3,
        X+x4, Y+y4,
        X   , Y   ,
        Rotation
    );
    RenderLine(
        X+x1, Y+y1,
        X+x2, Y+y2,
        X+x3, Y+y3,
        X+x4, Y+y4,
        X   , Y   ,
        Rotation+pi/2.0
    );

    Render = new GerberRender;
    Render->Command = gcFill;
    Add(Render);

    return true;
}
//------------------------------------------------------------------------------

bool GerberMacro::RenderThermal(PRIMITIVE_ITEM* Primitive){
    int j;
    GerberRender* Render;

    const int ModifierCount = 6;

    double Modifier[ModifierCount];

    for(j = 0; j < ModifierCount; j++){
        if(j < Primitive->ModifierCount){
            Modifier[j] = Evaluate(Primitive->Modifier[j]);
        }else{
            Modifier[j] = 0.0;
        }
    }

    double X, Y;
    double OD;
    double ID;
    double Gap;
    double Rot;

    X   = Modifier[0];
    Y   = Modifier[1];
    OD  = Modifier[2];
    ID  = Modifier[3];
    Gap = Modifier[4];
    Rot = Modifier[5];

    double x1, y1;
    double x2, y2;
    double a1, a2; // Inner and outer angles
    double r , t ;

    OD  /= 2.0;
    ID  /= 2.0;
    Gap /= 2.0;
    Rot *= pi/180.0;

    x1 = sqrt(OD*OD - Gap*Gap);
    y1 = Gap;
    x2 = Gap;
    y2 = sqrt(ID*ID - Gap*Gap);

    a1 = -2.0*atan2(y1, x1) + pi/2.0;
    a2 =  2.0*atan2(y2, x2) - pi/2.0;

    r  = sqrt (y1*y1 + x1*x1);
    t  = atan2(y1    , x1   ) + Rot;
    x1 = r*cos(t);
    y1 = r*sin(t);

    r  = sqrt (y2*y2 + x2*x2);
    t  = atan2(y2    , x2   ) + Rot;
    x2 = r*cos(t);
    y2 = r*sin(t);

    a1 *= 180.0/pi;
    a2 *= 180.0/pi;

    for(j = 0; j < 4; j++){
        Render = new GerberRender;
        Render->Command = gcBeginLine;
        Render->X       = Get_mm(X+x1);
        Render->Y       = Get_mm(Y+y1);
        Add(Render);

        Render = new GerberRender;
        Render->Command = gcArc;
        Render->X       = Get_mm(X);
        Render->Y       = Get_mm(Y);
        Render->A       = a1;
        Add(Render);

        Render = new GerberRender;
        Render->Command = gcLine;
        Render->X       = Get_mm(X+x2);
        Render->Y       = Get_mm(Y+y2);
        Add(Render);

        Render = new GerberRender;
        Render->Command = gcArc;
        Render->X       = Get_mm(X);
        Render->Y       = Get_mm(Y);
        Render->A       = -a2;
        Add(Render);

        Render = new GerberRender;
        Render->Command = gcClose;
        Add(Render);

        // Rotate 90 deg
        t  =  x1;
        x1 = -y1;
        y1 =   t;
        t  =  x2;
        x2 = -y2;
        y2 =   t;
    }

    Render = new GerberRender;
    Render->Command = gcFill;
    Add(Render);

    return true;
}
//------------------------------------------------------------------------------

bool GerberMacro::RenderAssignment(PRIMITIVE_ITEM* Primitive){
    int     j, t;
    double* Temp;

    if(Primitive->ModifierCount < 1) return false;

    if(Primitive->Index > ModifierCount){
        t             = ModifierCount;
        ModifierCount = Primitive->Index;

        Temp = new double[ModifierCount];
        for(j = 0; j < t; j++){
            Temp[j] = Modifiers[j];
        }
        for(; j < ModifierCount; j++){
            Temp[j] = 0.0;
        }

        if(NewModifiers) delete[] Modifiers;
        Modifiers    = Temp;
        NewModifiers = true;
    }

    Modifiers[Primitive->Index-1] = Evaluate(Primitive->Modifier[0]);

    return true;
}
//------------------------------------------------------------------------------

GerberRender* GerberMacro::Render(double* Modifiers, int ModifierCount){
    RenderList = 0;
    RenderLast = 0;

    if(NewModifiers) delete[] GerberMacro::Modifiers;
    NewModifiers = false;

    Exposure     = true;

    GerberMacro::Modifiers     = Modifiers;
    GerberMacro::ModifierCount = ModifierCount;

    PRIMITIVE_ITEM* Primitive = Primitives;
    while(Primitive){
        switch(Primitive->Primitive){
            case pCircle:
                if(!RenderCircle(Primitive)) return 0;
                break;

            case pLineVector:
            case pLineVector2:
                if(!RenderLineVector(Primitive)) return 0;
                break;

            case pLineCenter:
                if(!RenderLineCenter(Primitive)) return 0;
                break;

            case pLineLowerLeft:
                if(!RenderLineLowerLeft(Primitive)) return 0;
                break;

            case pOutline:
                if(!RenderOutline(Primitive)) return 0;
                break;

            case pPolygon:
                if(!RenderPolygon(Primitive)) return 0;
                break;

            case pMoire:
                if(!RenderMoire(Primitive)) return 0;
                break;

            case pThermal:
                if(!RenderThermal(Primitive)) return 0;
                break;

            case pAssignment:
                if(!RenderAssignment(Primitive)) return 0;
                break;

            default:
                break;
        }
        Primitive = Primitive->Next;
    }

    return RenderList;
}
//------------------------------------------------------------------------------

void GerberMacro::Add(PRIMITIVE_ITEM* Primitive){
    if(!Primitive) return;

    if(Primitives){
        PrimitivesLast->Next = Primitive;
    }else{
        Primitives = Primitive;
    }

    PrimitivesLast = Primitive;
}
//------------------------------------------------------------------------------

bool GerberMacro::Float(double* Number){
    int       Integer = 0;
    bool      Sign    = false;
    double    Scale   = 0.1;
    unsigned  i       = Index;

    // This could be an optional argument
    if(Index < Length && Buffer[Index] == '*'){
        return true;
    }

    if(Index < Length && Buffer[Index] == '-'){
        Sign = true;
        Index++;
    }else if(Index < Length && Buffer[Index] == '+'){
        Index++;
    }

    while(Index < Length){
        if(Buffer[Index] >= '0' && Buffer[Index] <= '9'){
            Integer *= 10;
            Integer += Buffer[Index] - '0';
            Index++;
        }else{
            break;
        }
    }

    *Number = Integer;

    if(Index < Length && Buffer[Index] == '.'){
        Index++;
        while(Index < Length){
            if(Buffer[Index] >= '0' && Buffer[Index] <= '9'){
                *Number += (Buffer[Index] - '0') * Scale;
                Scale *= 0.1;
                Index++;
            }else{
                break;
            }
        }
    }

    if(Index < Length){
        if(Sign) *Number *= -1.0;
        return (Index > i);
    }

    Index = i;
    return false;
}
//------------------------------------------------------------------------------

bool GerberMacro::Integer(int* Integer){
    bool     Sign = false;
    unsigned i    = Index;

    *Integer = 0;

    if(Index < Length && Buffer[Index] == '-'){
        Sign = true;
        Index++;
    }else if(Index < Length && Buffer[Index] == '+'){
        Index++;
    }

    while(Index < Length){
        if(Buffer[Index] >= '0' && Buffer[Index] <= '9'){
            *Integer *= 10;
            *Integer += Buffer[Index] - '0';
            Index++;
        }else{
            if(Sign) *Integer *= -1;
            return (Index > i);
        }
    }

    Index = i;
    return false;
}
//------------------------------------------------------------------------------

void GerberMacro::WhiteSpace(){
    while(Index < Length){
        switch(Buffer[Index]){
            case ' ' :
            case '\t':
            case '\r':
            case '\n':
                break;

            default:
                return;
        }
        Index++;
    }
}
//------------------------------------------------------------------------------

// Term {("+" | "-") Term};
GerberMacro::OPERATOR_ITEM* GerberMacro::Modifier(){
    OPERATOR       Operator;
    OPERATOR_ITEM* Left;
    OPERATOR_ITEM* Root;

    WhiteSpace();

    Left = Term();
    if(!Left) return 0;

    while(Index < Length){
        switch(Buffer[Index]){
            case '+':
                Operator = opAdd;
                break;

            case '-':
                Operator = opSubtract;
                break;

            default:
                return Left;
        }
        Index++;

        Root = new OPERATOR_ITEM;
        Root->Operator = Operator;
        Root->Left     = Left;
        Root->Right    = Term();

        Left = Root;
    }

    delete Left;

    return 0;
}
//------------------------------------------------------------------------------

// Factor {("x" | "X" | "/") Factor};
GerberMacro::OPERATOR_ITEM* GerberMacro::Term(){
    OPERATOR       Operator;
    OPERATOR_ITEM* Left;
    OPERATOR_ITEM* Root;

    WhiteSpace();

    Left = Factor();
    if(!Left) return 0;

    while(Index < Length){
        switch(Buffer[Index]){
            case 'x':
            case 'X':
                Operator = opMultiply;
                break;

            case '/':
                Operator = opDivide;
                break;

            default:
                return Left;
        }
        Index++;

        Root = new OPERATOR_ITEM;
        Root->Operator = Operator;
        Root->Left     = Left;
        Root->Right    = Factor();

        Left = Root;
    }

    delete Left;

    return 0;
}
//------------------------------------------------------------------------------

// ["+" | "-"] (("(" Modifier ")") | Variable | Float)
GerberMacro::OPERATOR_ITEM* GerberMacro::Factor(){
    bool           Negative = false;
    double         d;
    OPERATOR_ITEM* Item;

    WhiteSpace();

    if(Index >= Length) return 0;
    if(Buffer[Index] == '-'){
        Negative = true;
        Index++; WhiteSpace();

    }else if(Buffer[Index] == '+'){
        Index++; WhiteSpace();
    }

    if(Buffer[Index] == '('){
        Index++;

        Item = Modifier();
        if(!Item){
            printf("Error: Expression expected\n");
            return 0;
        }

        WhiteSpace();

        if(Buffer[Index] != ')'){
            printf("Error: ')' expected\n");
            delete Item;
            return 0;
        }
        Index++;

    }else{
        Item = Variable();
        if(!Item){
            d = 0.0; // Default in case the argument is optional
            if(!Float(&d)){
                printf("Error: Float expected\n");
                return 0;
            }
            Item = new OPERATOR_ITEM;
            Item->Operator = opLiteral;
            Item->Value    = d;
        }
    }

    if(Item && Negative){
        OPERATOR_ITEM* Root  = new OPERATOR_ITEM;
        Root->Operator       = opMultiply;
        Root->Left           = new OPERATOR_ITEM;
        Root->Left->Operator = opLiteral;
        Root->Left->Value    = -1;
        Root->Right          = Item;
        Item = Root;
    }

    return Item;
}
//------------------------------------------------------------------------------

// "$" Integer
GerberMacro::OPERATOR_ITEM* GerberMacro::Variable(){
    int            i;
    OPERATOR_ITEM* Item;

    WhiteSpace();

    if(Index >= Length)      return 0;
    if(Buffer[Index] != '$') return 0;
    Index++;

    if(!Integer(&i)){
        Index--;
        return 0;
    }

    Item = new OPERATOR_ITEM;
    Item->Operator = opVariable;
    Item->Index    = i;

    return Item;
}
//------------------------------------------------------------------------------

bool GerberMacro::Comment(){
    while(Index < Length){
        if(Buffer[Index] == '*') return true;
        Index++;
    }

    return false;
}
//------------------------------------------------------------------------------

bool GerberMacro::Circle(){
    PRIMITIVE_ITEM* Item;

    int  j;
    bool b = true;

    WhiteSpace();

    const int ModifierCount = 5;

    Item = new PRIMITIVE_ITEM;
    Item->Primitive     = pCircle;
    Item->ModifierCount = ModifierCount;
    Item->Modifier      = new OPERATOR_ITEM*[ModifierCount];

    for(j = 0; j < ModifierCount; j++){
        Item->Modifier[j] = 0;
    }

    WhiteSpace();

    j = 0;
    while(b && Index < Length && j < ModifierCount){
        if(Buffer[Index] == ','){
            Index++;
            b = (Item->Modifier[j++] = Modifier());
        }else{
            break; // It is legal to specify fewer modifiers than the maximum
        }
        WhiteSpace();
    }

    Add(Item);

    return b;
}
//------------------------------------------------------------------------------

bool GerberMacro::Line_Vector(){
    PRIMITIVE_ITEM* Item;

    int  j;
    bool b = true;

    WhiteSpace();

    const int ModifierCount = 7;

    Item = new PRIMITIVE_ITEM;
    Item->Primitive     = pLineVector;
    Item->ModifierCount = ModifierCount;
    Item->Modifier      = new OPERATOR_ITEM*[ModifierCount];

    for(j = 0; j < ModifierCount; j++){
        Item->Modifier[j] = 0;
    }

    WhiteSpace();

    j = 0;
    while(b && Index < Length && j < ModifierCount){
        if(Buffer[Index] == ','){
            Index++;
            b = (Item->Modifier[j++] = Modifier());
        }else{
            break;
        }
        WhiteSpace();
    }

    Add(Item);

    return b;
}
//------------------------------------------------------------------------------

bool GerberMacro::Line_Center(){
    PRIMITIVE_ITEM* Item;

    int  j;
    bool b = true;

    WhiteSpace();

    const int ModifierCount = 6;

    Item = new PRIMITIVE_ITEM;
    Item->Primitive     = pLineCenter;
    Item->ModifierCount = ModifierCount;
    Item->Modifier      = new OPERATOR_ITEM*[ModifierCount];

    for(j = 0; j < ModifierCount; j++){
        Item->Modifier[j] = 0;
    }

    WhiteSpace();

    j = 0;
    while(b && Index < Length && j < ModifierCount){
        if(Buffer[Index] == ','){
            Index++;
            b = (Item->Modifier[j++] = Modifier());
        }else{
            break;
        }
        WhiteSpace();
    }

    Add(Item);

    return b;
}
//------------------------------------------------------------------------------

bool GerberMacro::Line_LowerLeft(){
    PRIMITIVE_ITEM* Item;

    int  j;
    bool b = true;

    WhiteSpace();

    const int ModifierCount = 6;

    Item = new PRIMITIVE_ITEM;
    Item->Primitive     = pLineLowerLeft;
    Item->ModifierCount = ModifierCount;
    Item->Modifier      = new OPERATOR_ITEM*[ModifierCount];

    for(j = 0; j < ModifierCount; j++){
        Item->Modifier[j] = 0;
    }

    WhiteSpace();

    j = 0;
    while(b && Index < Length && j < ModifierCount){
        if(Buffer[Index] == ','){
            Index++;
            b = (Item->Modifier[j++] = Modifier());
        }else{
            break;
        }
        WhiteSpace();
    }

    Add(Item);

    return b;
}
//------------------------------------------------------------------------------

bool GerberMacro::Outline(){
    PRIMITIVE_ITEM* Item;

    int  j;
    bool b = true;

    int ModifierCount;
    int Exposure;
    int N;

    WhiteSpace();

    if(Index >= Length)      return false;
    if(Buffer[Index] != ',') return false;
    Index++;

    WhiteSpace();

    if(!Integer(&Exposure)){
        printf("Error: Integer exposure expected\n");
        return false;
    }

    WhiteSpace();

    if(Index >= Length)      return false;
    if(Buffer[Index] != ',') return false;
    Index++;

    WhiteSpace();

    if(!Integer(&N)){
        printf("Error: Integer number of outline points expected\n");
        return false;
    }

    ModifierCount = 5 + 2*N;

    Item = new PRIMITIVE_ITEM;
    Item->Primitive     = pOutline;
    Item->ModifierCount = ModifierCount;
    Item->Modifier      = new OPERATOR_ITEM*[ModifierCount];

    for(j = 0; j < ModifierCount; j++){
        Item->Modifier[j] = 0;
    }

    Item->Modifier[0] = new OPERATOR_ITEM;
    Item->Modifier[0]->Operator = opLiteral;
    Item->Modifier[0]->Value    = Exposure;

    Item->Modifier[1] = new OPERATOR_ITEM;
    Item->Modifier[1]->Operator = opLiteral;
    Item->Modifier[1]->Value    = N;

    WhiteSpace();

    j = 2;
    while(b && Index < Length && j < ModifierCount){
        if(Buffer[Index] == ','){
            Index++;
            b = (Item->Modifier[j++] = Modifier());
        }else{
            break;
        }
        WhiteSpace();
    }

    Add(Item);

    return b;
}
//------------------------------------------------------------------------------

bool GerberMacro::Polygon(){
    PRIMITIVE_ITEM* Item;

    int  j;
    bool b = true;

    const int ModifierCount = 6;

    Item = new PRIMITIVE_ITEM;
    Item->Primitive     = pPolygon;
    Item->ModifierCount = ModifierCount;
    Item->Modifier      = new OPERATOR_ITEM*[ModifierCount];

    for(j = 0; j < ModifierCount; j++){
        Item->Modifier[j] = 0;
    }

    WhiteSpace();

    j = 0;
    while(b && Index < Length && j < ModifierCount){
        if(Buffer[Index] == ','){
            Index++;
            b = (Item->Modifier[j++] = Modifier());
        }else{
            break;
        }
        WhiteSpace();
    }

    Add(Item);

    return b;
}
//------------------------------------------------------------------------------

bool GerberMacro::Moire(){
    PRIMITIVE_ITEM* Item;

    int  j;
    bool b = true;

    const int ModifierCount = 9;

    Item = new PRIMITIVE_ITEM;
    Item->Primitive     = pMoire;
    Item->ModifierCount = ModifierCount;
    Item->Modifier      = new OPERATOR_ITEM*[ModifierCount];

    for(j = 0; j < ModifierCount; j++){
        Item->Modifier[j] = 0;
    }

    WhiteSpace();

    j = 0;
    while(b && Index < Length && j < ModifierCount){
        if(Buffer[Index] == ','){
            Index++;
            b = (Item->Modifier[j++] = Modifier());
        }else{
            break;
        }
        WhiteSpace();
    }

    Add(Item);

    return b;
}
//------------------------------------------------------------------------------

bool GerberMacro::Thermal(){
    PRIMITIVE_ITEM* Item;

    int  j;
    bool b = true;

    const int ModifierCount = 6;

    Item = new PRIMITIVE_ITEM;
    Item->Primitive     = pThermal;
    Item->ModifierCount = ModifierCount;
    Item->Modifier      = new OPERATOR_ITEM*[ModifierCount];

    for(j = 0; j < ModifierCount; j++){
        Item->Modifier[j] = 0;
    }

    WhiteSpace();

    j = 0;
    while(b && Index < Length && j < ModifierCount){
        if(Buffer[Index] == ','){
            Index++;
            b = (Item->Modifier[j++] = Modifier());
        }else{
            break;
        }
        WhiteSpace();
    }

    Add(Item);

    return b;
}
//------------------------------------------------------------------------------

bool GerberMacro::Assignment(){
    PRIMITIVE_ITEM* Item;

    int VarIndex;

    if(!Integer(&VarIndex)) return false;

    WhiteSpace();

    if(Index > Length)       return false;
    if(Buffer[Index] != '=') return false;
    Index++;

    Item = new PRIMITIVE_ITEM;
    Item->Primitive     = pAssignment;
    Item->ModifierCount = 1;
    Item->Modifier      = new OPERATOR_ITEM*[1];
    Item->Modifier[0]   = Modifier();
    Item->Index         = VarIndex;

    if(!Item->Modifier[0]){
        delete Item;
        return false;
    }

    WhiteSpace();

    Add(Item);

    return true;
}
//------------------------------------------------------------------------------

bool GerberMacro::Primitive(){
    int Identifier;

    WhiteSpace();

    if(Index >= Length) return false;

    if(!Integer(&Identifier)){
        if(Buffer[Index] == '$'){
            Index++;
            Identifier = pAssignment;
        }else{
            printf("Error: Macro primitive expected\n");
            return false;
        }
    };

    switch(Identifier){
        case pComment:
            return Comment();

        case pCircle:
            return Circle();

        case pLineVector:
        case pLineVector2:
            return Line_Vector();

        case pLineCenter:
            return Line_Center();

        case pLineLowerLeft:
            return Line_LowerLeft();

        case pEndOfFile:
            return true;

        case pOutline:
            return Outline();

        case pPolygon:
            return Polygon();

        case pMoire:
            return Moire();

        case pThermal:
            return Thermal();

        case pAssignment:
            return Assignment();

        default:
            printf("Error: Unknown Macro Primitive: %d\n", Buffer[Index]);
            return false;
    }

    printf("Error: End of block expected\n");
    return false;
}
//------------------------------------------------------------------------------

bool GerberMacro::LoadMacro(const char* Buffer, unsigned Length, bool Inches){
    GerberMacro::Buffer = Buffer;
    GerberMacro::Length = Length;
    GerberMacro::Inches = Inches;
    GerberMacro::Index  = 0;

    if(!Primitive())         return false;
    WhiteSpace();
    if(Index >= Length)      return false;
    if(Buffer[Index] != '*') return false;
    Index++;
    WhiteSpace();

    while(Primitive()){
        WhiteSpace();
        if(Index >= Length)      return false;
        if(Buffer[Index] != '*') return false;
        Index++;
        WhiteSpace();
    }

    return Index == Length;
}
//------------------------------------------------------------------------------
