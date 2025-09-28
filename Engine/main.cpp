//==============================================================================
// Copyright (C) John-Philip Taylor
// jpt13653903@gmail.com
//
// This file is part of Gerber2PDF
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

#include "main.h"
//------------------------------------------------------------------------------

using namespace std;
//------------------------------------------------------------------------------

bool SilentExit = false;
//------------------------------------------------------------------------------

static int Pause(){
    if(SilentExit) return 0;
    printf("\nPress Enter to continue\n");
    char c;
    return scanf("%c", &c);
}
//------------------------------------------------------------------------------

static bool GetInt(const char* Buffer, int* Index, int* Result){
    *Result = 0;
    while(Buffer[*Index]){
        if(Buffer[*Index] >= '0' && Buffer[*Index] <= '9'){
            *Result = 10*(*Result) + Buffer[*Index] - '0';

        }else if(Buffer[*Index] == ','){
            (*Index)++;
            return true;

        }else{
            return false;
        }
        (*Index)++;
    }
    return true;
}
//------------------------------------------------------------------------------

static bool StringStart(const char* String, const char* Start){
    int j;
    for(j = 0; String[j] && Start[j]; j++){
        if(String[j] != Start[j]) return false;
    }
    return !Start[j];
}
//------------------------------------------------------------------------------

#ifdef NIX
    static unsigned long GetCurrentDirectory(
        unsigned long BufferLength,
        char*         Buffer
    ){
        if(!getcwd(Buffer, BufferLength)) return 0;
        return strlen(Buffer);
    }
#endif
//------------------------------------------------------------------------------

#ifdef WINVER
    int wmain(int argc, const wchar_t** wargv){
        if(argc > 0x1000) argc = 0x1000;
        string argv_string[0x1000];
        const char* argv[0x1000];
        for(int n = 0; n < argc; n++){
            argv_string[n] = utfConverter.toUtf8((const char16_t*)wargv[n]);
            argv       [n] = argv_string[n].c_str();
        }
#else
    int main(int argc, const char** argv){
#endif

    // No longer required on newer Windows terminals
    // setupTerminal();

    ENGINE Engine;

    int j;
    int Result;

    string OutputFileName;
    string FileName;

    int arg;

    if(argc < 2){
        printf(
            "Gerber2PDF, Version %d.%d\n"
            "Built on " __DATE__ " at " __TIME__ "\n"
            "\n"
            "Copyright (C) John-Philip Taylor\n"
            "jpt13653903@gmail.com\n"
            "\n"
            "This program is free software: you can redistribute it and/or modify\n"
            "it under the terms of the GNU General Public License as published by\n"
            "the Free Software Foundation, either version 3 of the License, or\n"
            "(at your option) any later version.\n"
            "\n"
            "This program is distributed in the hope that it will be useful,\n"
            "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
            "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
            "GNU General Public License for more details.\n"
            "\n"
            "You should have received a copy of the GNU General Public License\n"
            "along with this program.  If not, see <http://www.gnu.org/licenses/>\n"
            "\n"
            "Usage: Gerber2pdf [-silentexit] [-nowarnings] [-CMYK] ...\n"
            "       [-output=output_file_name] ...\n"
            "       [-background=R,G,B[,A]] [-backgroundCMYK=C,M,Y,K[,A]] ...\n"
            "       [-strokes2fills] [-name_is_filename] ...\n"
            "       [-page_size=extents|A3|A4|letter] ...\n"
            "       [-orientation=portrait|landscape] [-scale_to_fit] ...\n"
            "       [-next_page_size=extents|A3|A4|letter] ...\n"
            "       [-next_orientation=portrait|landscape] [-next_scale_to_fit] ...\n"
            "       file_1 [-combine] file_2 file_3 file_4...\n"
            "       [-colour=R,G,B[,A]] [-colourCMYK=C,M,Y,K[,A]] [-mirror] ...\n"
            "       [-nomirror] [-nocombine] ... file_N\n"
            "\n"
            "Example: Gerber2pdf -output=My_Project\n"
            "         top_silk.grb bottom_silk.grb\n"
            "         top_copper.grb inner_copper_1.grb\n"
            "         inner_copper_2.grb bottom_copper.grb\n"
            "         bottom_solder_mask.grb top_solder_mask.grb\n"
            "         board_outline.grb\n"
            "         -combine -mirror\n"
            "         -colour=255,0,0     bottom_copper.grb\n"
            "         -colour=0,128,0,200 bottom_solder_mask.grb\n"
            "         -colour=0,0,255     board_outline.grb\n"
            "         -combine -nomirror\n"
            "         -colour=255,0,0     top_copper.grb\n"
            "         -colour=0,128,0,200 top_solder_mask.grb\n"
            "         -colour=0,0,255     board_outline.grb\n"
            "\n"
            "The -silentexit option disables the pause on exit.\n"
            "The -nowarnings option disables deprecated feature warnings.\n"
            "\n"
            "The -CMYK option translates the output PDF to use the CMYK colour space.\n"
            "The colours can still be specified using -colour and -background,\n"
            "but this is not quite accurate.  For more accurate colour control,\n"
            "use -colourCMYK and -backgroundCMYK instead.\n"
            "\n"
            "The -colour (in RGB) uses components in the range 0 to 255, whereas\n"
            "the -colourCMYK uses components in the range 0 to 100.  This is true\n"
            "for the Alpha component as well.\n"
            "\n"
            "The optional -background colour is either transparent or opaque.  The\n"
            "threshold is 50%% (i.e. A=128 for RGB and A=50 for CMYK).\n"
            "Set it just before the target page is created.\n"
            "\n"
            "The -strokes2fills option converts all strokes to fills for the next\n"
            "file, thereby converting outlines to areas.  It resets to default\n"
            "after that file.\n"
            "\n"
            "The -name_is_filename option forces the PDF bookmarks to use\n"
            "the filename instead of the IN or LN commands.\n"
            "\n"
            "The -page_size option takes global effect and can have one of 4 values:\n"
            "  \"extents\", \"A3\", \"A4\" or \"letter\"\n"
            "\n"
            "The -orientation and -scale_to_fit options only take effect\n"
            "on standard paper sizes (i.e. A3, A4 and letter).\n"
            "\n"
            "The -next_page_size, -next_orientation and -next_scale_to_fit options\n"
            "only take effect for the next page created.  Define before calling the\n"
            "first Gerber of that page.  These override the \"global\" options.\n",
            MAJOR_VERSION, MINOR_VERSION // These are defined in the Makefile
        );
        Pause();
        return 0;
    }

    #if defined(WINVER)
        wchar_t Path[0x100];
        GetCurrentDirectory(0x100, Path);
        for(j = 0; Path[j]; j++);

        if(Path[j-1] != L'\\'){
            Path[j++] = L'\\';
            Path[j  ] = 0;
        }
    #elif defined(NIX)
        char Path[0x100];
        GetCurrentDirectory(0x100, Path);
        for(j = 0; Path[j]; j++);

        if(Path[j-1] != '/'){
            Path[j++] = '/';
            Path[j  ] = 0;
        }
    #endif

    // For each argument...
    for(arg = 1; arg < argc; arg++){
        // Check for options
        if(argv[arg][0] == '-'){
            if(StringStart(argv[arg]+1, "output=")){
                OutputFileName.assign(argv[arg]+8);

            }else if(StringStart(argv[arg]+1, "colour=")){
                int i = 8;
                int R, G, B, A;
                if(!GetInt(argv[arg], &i, &R)) continue;
                if(!GetInt(argv[arg], &i, &G)) continue;
                if(!GetInt(argv[arg], &i, &B)) continue;
                if(argv[arg][i]){
                    if(!GetInt(argv[arg], &i, &A)) continue;
                }else{
                    A = 255;
                }
                if(R < 0 || R > 255) continue;
                if(G < 0 || G > 255) continue;
                if(B < 0 || B > 255) continue;
                if(A < 0 || A > 255) continue;
                Engine.Dark.UseCMYK = false;
                Engine.Dark.R = R/255.0;
                Engine.Dark.G = G/255.0;
                Engine.Dark.B = B/255.0;
                Engine.Dark.A = A/255.0;

            }else if(StringStart(argv[arg]+1, "colourCMYK=")){
                int i = 12;
                int C, M, Y, K, A;
                if(!GetInt(argv[arg], &i, &C)) continue;
                if(!GetInt(argv[arg], &i, &M)) continue;
                if(!GetInt(argv[arg], &i, &Y)) continue;
                if(!GetInt(argv[arg], &i, &K)) continue;
                if(argv[arg][i]){
                    if(!GetInt(argv[arg], &i, &A)) continue;
                }else{
                    A = 100;
                }
                if(C < 0 || C > 100) continue;
                if(M < 0 || M > 100) continue;
                if(Y < 0 || Y > 100) continue;
                if(K < 0 || K > 100) continue;
                if(A < 0 || A > 100) continue;
                Engine.Dark.UseCMYK = true;
                Engine.Dark.C = C/100.0;
                Engine.Dark.M = M/100.0;
                Engine.Dark.Y = Y/100.0;
                Engine.Dark.K = K/100.0;
                Engine.Dark.A = A/100.0;

            }else if(StringStart(argv[arg]+1, "background=")){
                int i = 12;
                int R, G, B, A;
                if(!GetInt(argv[arg], &i, &R)) continue;
                if(!GetInt(argv[arg], &i, &G)) continue;
                if(!GetInt(argv[arg], &i, &B)) continue;
                if(argv[arg][i]){
                    if(!GetInt(argv[arg], &i, &A)) continue;
                }else{
                    A = 255;
                }
                if(R < 0 || R > 255) continue;
                if(G < 0 || G > 255) continue;
                if(B < 0 || B > 255) continue;
                if(A < 0 || A > 255) continue;
                Engine.Light.UseCMYK = false;
                if(A < 128){
                    Engine.Light.R = 1.0;
                    Engine.Light.G = 1.0;
                    Engine.Light.B = 1.0;
                    Engine.Light.A = 0.0;
                }else{
                    Engine.Light.R = R/255.0;
                    Engine.Light.G = G/255.0;
                    Engine.Light.B = B/255.0;
                    Engine.Light.A =     1.0;
                }

            }else if(StringStart(argv[arg]+1, "backgroundCMYK=")){
                int i = 16;
                int C, M, Y, K, A;
                if(!GetInt(argv[arg], &i, &C)) continue;
                if(!GetInt(argv[arg], &i, &M)) continue;
                if(!GetInt(argv[arg], &i, &Y)) continue;
                if(!GetInt(argv[arg], &i, &K)) continue;
                if(argv[arg][i]){
                    if(!GetInt(argv[arg], &i, &A)) continue;
                }else{
                    A = 100;
                }
                if(C < 0 || C > 100) continue;
                if(M < 0 || M > 100) continue;
                if(Y < 0 || Y > 100) continue;
                if(K < 0 || K > 100) continue;
                if(A < 0 || A > 100) continue;
                Engine.Light.UseCMYK = true;
                if(A < 50){
                    Engine.Light.C = 0.0;
                    Engine.Light.M = 0.0;
                    Engine.Light.Y = 0.0;
                    Engine.Light.K = 0.0;
                    Engine.Light.A = 0.0;
                }else{
                    Engine.Light.C = C/100.0;
                    Engine.Light.M = M/100.0;
                    Engine.Light.Y = Y/100.0;
                    Engine.Light.K = K/100.0;
                    Engine.Light.A = 1.0;
                }

            }else if(!strcmp(argv[arg]+1, "combine")){
                Engine.Combine = true;
                Engine.NewPage = true;

            }else if(!strcmp(argv[arg]+1, "nocombine")){
                Engine.Combine = false;

            }else if(!strcmp(argv[arg]+1, "mirror")){
                Engine.Mirror = true;

            }else if(!strcmp(argv[arg]+1, "nomirror")){
                Engine.Mirror = false;

            }else if(!strcmp(argv[arg]+1, "nowarnings")){
                GerberWarnings = false; // Defined in JGerber.h

            }else if(!strcmp(argv[arg]+1, "silentexit")){
                SilentExit = true;

            }else if(!strcmp(argv[arg]+1, "CMYK")){
                Engine.UseCMYK = true;

            }else if(!strcmp(argv[arg]+1, "strokes2fills")){
                Engine.ConvertStrokesToFills = true;

            }else if(!strcmp(argv[arg]+1, "name_is_filename")){
                Engine.NameIsFilename = true;

            }else if(StringStart(argv[arg]+1, "page_size=")){
                if     (!strcmp(argv[arg]+11, "extents")) Engine.PageSize = ENGINE::PS_Extents;
                else if(!strcmp(argv[arg]+11, "A3"     )) Engine.PageSize = ENGINE::PS_A3;
                else if(!strcmp(argv[arg]+11, "A4"     )) Engine.PageSize = ENGINE::PS_A4;
                else if(!strcmp(argv[arg]+11, "letter" )) Engine.PageSize = ENGINE::PS_Letter;
                else printf("Error: Only \"extents\", \"A3\", \"A4\" and \"letter\"\n"
                                        "       page sizes are supported\n");

            }else if(StringStart(argv[arg]+1, "orientation=")){
                if     (!strcmp(argv[arg]+13, "portrait" )) Engine.Orientation = ENGINE::PO_Portrait;
                else if(!strcmp(argv[arg]+13, "landscape")) Engine.Orientation = ENGINE::PO_Landscape;
                else printf("Error: Only \"portrait\" and \"landscape\"\n"
                                        "       orientations are supported\n");

            }else if(!strcmp(argv[arg]+1, "scale_to_fit")){
                Engine.ScaleToFit = true;

            }else if(StringStart(argv[arg]+1, "next_page_size=")){
                if     (!strcmp(argv[arg]+16, "extents")) Engine.NextPageSize = ENGINE::PS_Extents;
                else if(!strcmp(argv[arg]+16, "A3"     )) Engine.NextPageSize = ENGINE::PS_A3;
                else if(!strcmp(argv[arg]+16, "A4"     )) Engine.NextPageSize = ENGINE::PS_A4;
                else if(!strcmp(argv[arg]+16, "letter" )) Engine.NextPageSize = ENGINE::PS_Letter;
                else printf("Error: Only \"extents\", \"A3\", \"A4\" and \"letter\"\n"
                                        "       page sizes are supported\n");

            }else if(StringStart(argv[arg]+1, "next_orientation=")){
                if     (!strcmp(argv[arg]+18, "portrait" )) Engine.NextOrientation = ENGINE::PO_Portrait;
                else if(!strcmp(argv[arg]+18, "landscape")) Engine.NextOrientation = ENGINE::PO_Landscape;
                else printf("Error: Only \"portrait\" and \"landscape\"\n"
                                        "       orientations are supported\n");

            }else if(!strcmp(argv[arg]+1, "next_scale_to_fit")){
                Engine.NextScaleToFit = true;
            }
            continue; // handle the next argument
        }

        // Read the gerber
        FileName.assign(argv[arg]);
        if(FileName.length() < 2){
            Engine.ConvertStrokesToFills = false;
            continue;
        }
        #if defined(WINVER)
            if(FileName[1] != '\\' && FileName[1] != ':'){
                FileName.insert(0, utfConverter.toUtf8((const char16_t*)Path));
            }
        #elif defined(NIX)
            if(FileName[0] != '/'){
                FileName.insert(0, Path);
            }
        #endif

        Result = Engine.Run(FileName.c_str(), argv[arg]);
        if(Result){
            Pause();
            return Result;
        }
    }

    if(!OutputFileName.length()){
        OutputFileName.assign(FileName.c_str());
    }else{
        #if defined(WINVER)
            if(OutputFileName[1] != '\\' && OutputFileName[1] != ':'){
                OutputFileName.insert(0, utfConverter.toUtf8((const char16_t*)Path));
            }
        #elif defined(NIX)
            if(OutputFileName[0] != '/'){
                OutputFileName.insert(0, Path);
            }
        #endif
    }
    OutputFileName.append(".pdf");

    Engine.Finish(OutputFileName.c_str());

    Pause();
    return 0;
}
//------------------------------------------------------------------------------

