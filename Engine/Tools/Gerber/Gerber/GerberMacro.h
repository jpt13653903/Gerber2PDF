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

// The EBNF used for parsing is:
//  Macro          = Primitive "*" {Primitive "*"};
//  Primitive      = Comment     | Circle         | Line_Vector |
//                   Line_Center | Line_LowerLeft | Outline     |
//                   Polygon     | Moire          | Thermal     |
//                   Assignment  ;
//  Comment        =  "0"         {"," Modifier};
//  Circle         =  "1"         {"," Modifier};
//  Line_Vector    = ("2" | "20") {"," Modifier};
//  Line_Center    =  "21"        {"," Modifier};
//  Line_LowerLeft =  "22"        {"," Modifier};
//  Outline        =  "4"         {"," Modifier};
//  Polygon        =  "5"         {"," Modifier};
//  Moire          =  "6"         {"," Modifier};
//  Thermal        =  "7"         {"," Modifier};
//  Assignment     = Variable "=" Modifier;
//  Modifier       = Term   {("+" | "-"      ) Term};
//  Term           = Factor {("x" | "X" | "/") Factor};
//  Factor         = ["+" | "-"] (("(" Modifier ")") | Variable | Float);
//  Variable       = "$" Integer;
//------------------------------------------------------------------------------

#ifndef GERBERMACRO_H
#define GERBERMACRO_H
//------------------------------------------------------------------------------

#include "GerberRender.h"
//------------------------------------------------------------------------------

class GerberRender;
class GerberMacro{
    private:
        enum PRIMITIVE{
            pComment       =  0,
            pCircle        =  1,
            pLineVector    =  2,
            pLineVector2   = 20,
            pLineCenter    = 21,
            pLineLowerLeft = 22,
            pEndOfFile     =  3,
            pOutline       =  4,
            pPolygon       =  5,
            pMoire         =  6,
            pThermal       =  7,
            pAssignment    = -1,
        };

        enum OPERATOR{
            opAdd,
            opSubtract,
            opMultiply,
            opDivide,

            opVariable,
            opLiteral
        };

        struct OPERATOR_ITEM{
            OPERATOR       Operator;
            OPERATOR_ITEM* Left;
            OPERATOR_ITEM* Right;

            union{
                int    Index; // Variable Index
                double Value; // Literal Value
            };

            OPERATOR_ITEM();
           ~OPERATOR_ITEM();
        };

        struct PRIMITIVE_ITEM{
            PRIMITIVE       Primitive;
            OPERATOR_ITEM** Modifier; // Modifier Tree for each modifier in the array
            int             ModifierCount;
            int             Index;    // Used for assignment primitives

            PRIMITIVE_ITEM* Next;

            PRIMITIVE_ITEM();
           ~PRIMITIVE_ITEM();
        };
        PRIMITIVE_ITEM* Primitives;
        PRIMITIVE_ITEM* PrimitivesLast;
        void Add(PRIMITIVE_ITEM* Primitive);

        GerberRender* RenderList;
        GerberRender* RenderLast;
        void Add(GerberRender* Render);

        double Evaluate(OPERATOR_ITEM* Root);

        void RenderLine(
            double x1, double y1,
            double x2, double y2,
            double x3, double y3,
            double x4, double y4,
            double xR, double yR, // Rotation Center
            double A
        );

        bool RenderCircle       (PRIMITIVE_ITEM* Primitive);
        bool RenderLineVector   (PRIMITIVE_ITEM* Primitive);
        bool RenderLineCenter   (PRIMITIVE_ITEM* Primitive);
        bool RenderLineLowerLeft(PRIMITIVE_ITEM* Primitive);
        bool RenderOutline      (PRIMITIVE_ITEM* Primitive);
        bool RenderPolygon      (PRIMITIVE_ITEM* Primitive);
        bool RenderMoire        (PRIMITIVE_ITEM* Primitive);
        bool RenderThermal      (PRIMITIVE_ITEM* Primitive);
        bool RenderAssignment   (PRIMITIVE_ITEM* Primitive);

        double* Modifiers;
        int     ModifierCount;
        bool    NewModifiers;
        bool    Exposure;

        const char* Buffer;
        unsigned    Length;
        unsigned    Index;
        bool        Inches;

        double Get_mm(double Number);

        bool Float     (double* Number);
        bool Integer   (int*    Integer);
        void WhiteSpace();

        OPERATOR_ITEM* Modifier  ();
        OPERATOR_ITEM* Term      ();
        OPERATOR_ITEM* Factor    ();
        OPERATOR_ITEM* Variable  ();

        bool Primitive     ();
        bool Comment       ();
        bool Circle        ();
        bool Line_Vector   ();
        bool Line_Center   ();
        bool Line_LowerLeft();
        bool Outline       ();
        bool Polygon       ();
        bool Moire         ();
        bool Thermal       ();
        bool Assignment    ();

    public:
        GerberMacro();
       ~GerberMacro();

        char* Name;

        // Modifiers is a null-terminated array
        // Returns a new list of render commands => The user must free the memory
        GerberRender* Render(double* Modifiers, int ModifierCount);

        bool LoadMacro(const char* Buffer, unsigned Length, bool Inches);
};
//------------------------------------------------------------------------------

#endif
//------------------------------------------------------------------------------

