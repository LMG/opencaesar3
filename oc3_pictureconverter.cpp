// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.

#include "oc3_pictureconverter.hpp"

#include <SDL_image.h>

#include "oc3_picture.hpp"
#include "oc3_math.hpp"

void PictureConverter::rgbBalance( Picture& dst, const Picture& src, int lROffset, int lGOffset, int lBOffset )
{
    SDL_Surface* source = const_cast< Picture& >( src ).get_surface();

    SDL_Surface *target = SDL_CreateRGBSurface(SDL_SWSURFACE, source->w, source->h, 32,
        0x00ff0000, 0x0000ff00, 
        0x000000ff, 0xff000000);

    if (target == NULL) 
    {
        //cerr << "CreateRGBSurface for rgbbalance conversion failed: " << SDL_GetError() << endl;
        return;
    }

    SDL_LockSurface( source );
    SDL_LockSurface(target);
    Uint32* imgpixels = (Uint32*)source->pixels;
    for (int i=0; i<source->h; i++) 
    {
        Uint8 r, g, b, a;
        for (int j=0; j<source->w; j++) 
        {
            Uint32 pixel = imgpixels[ i * source->w + j ];

            a = (pixel >> 24) & 0xff;//gr * ((( clr >> 24 ) & 0xff ) / 255.f );
            r = (pixel >> 16) & 0xff;//* ((( clr >> 16 ) & 0xff ) / 255.f );
            g = (pixel >> 8 ) & 0xff;// * ((( clr >> 8  ) & 0xff ) / 255.f ); 
            b = (pixel & 0xff);

            r = math::clamp<int>( r + lROffset, 0, 255);
            g = math::clamp<int>( g + lGOffset, 0, 255);
            b = math::clamp<int>( b + lBOffset, 0, 255);

            ((Uint32*)target->pixels)[ i * source->w + j ] = ( a << 24 )+ ( r << 16) 
                                                              + ( g << 8 ) + ( b );
        }
    }
    SDL_UnlockSurface(target);
    SDL_UnlockSurface(source);

    if( dst.get_surface() )
    {
        SDL_FreeSurface( dst.get_surface() );
    }

    dst.init( target, src.get_xoffset(), src.get_yoffset() );   
}

void PictureConverter::convToGrayscale( Picture& dst, const Picture& src )
{
    SDL_Surface* source = const_cast< Picture& >( src ).get_surface();

    SDL_Surface *target = SDL_CreateRGBSurface(SDL_SWSURFACE, source->w, source->h, 32,
        0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
   
    if (target == NULL) 
    {
        //cerr << "CreateRGBSurface for grayscale conversion failed: " << SDL_GetError() << endl;
        return;
    }

    SDL_LockSurface( source );
    SDL_LockSurface(target);
    Uint32* imgpixels = (Uint32*)source->pixels;
    for (int i=0; i<source->h; i++) 
    {
        Uint8 r, g, b, a;
        for (int j=0; j<source->w; j++) 
        {
            Uint32 pixel = imgpixels[ i * source->w + j ];
            Uint8 gr = (Uint8)( (((pixel >> 16) & 0xff) * 0.32)
                                 + (((pixel >> 8 ) & 0xff) * 0.55) 
                                 + (((pixel & 0xff) * 0.071)) );
            
            r = gr;//gr * ((( clr >> 24 ) & 0xff ) / 255.f );
            g = gr;//* ((( clr >> 16 ) & 0xff ) / 255.f );
            b = gr;// * ((( clr >> 8  ) & 0xff ) / 255.f ); 
            a = ((pixel>>24)&0xff);// > 0 ? 0xff : 0;
            ((Uint32*)target->pixels)[ i * source->w + j ] = ( a << 24 ) + ( r << 16 )+ ( g << 16) 
                                                              + ( b << 8 );
        }
    }
    SDL_UnlockSurface(target);
    SDL_UnlockSurface(source);

    if( dst.get_surface() )
    {
        SDL_FreeSurface( dst.get_surface() );
    }

    dst.init( target, src.get_xoffset(), src.get_yoffset() );
}

PictureConverter::~PictureConverter()
{

}

PictureConverter::PictureConverter()
{

}

void PictureConverter::fill( Picture& pic, int color )
{
    SDL_Surface* source = pic.get_surface();

    SDL_LockSurface( source );
    SDL_FillRect(source, NULL, SDL_MapRGBA(source->format, (color>>24)&0xff, (color>>16)&0xff, 
                                                           (color>>8)&0xff, (color&0xff))); 
    SDL_UnlockSurface(source);
}