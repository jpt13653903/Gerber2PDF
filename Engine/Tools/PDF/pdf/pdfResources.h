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

#ifndef PDFRESOURCES_H
#define PDFRESOURCES_H
//------------------------------------------------------------------------------

#include "pdfFont.h"
#include "pdfImage.h"
#include "pdfStream.h"
#include "pdfOpaque.h"
#include "pdfDictionary.h"
//------------------------------------------------------------------------------

class pdfForm;
class pdfResources : public pdfDictionary{
    private:
        pdfDictionary Fonts;
        pdfDictionary XObjects;
        pdfDictionary ExtGState;

        void Update();

    public:
        pdfResources();

        void AddFont  (pdfFont*   Font);
        void AddForm  (pdfForm*   Form);
        void AddImage (pdfImage*  Image);
        void AddOpaque(pdfOpaque* Opaque);
};
//------------------------------------------------------------------------------

#endif
//------------------------------------------------------------------------------

