// g++ -o ttf2txf ttf2txf.cpp -I/usr/include/freetype2 -lfreetype
// ==>
/*============================================================================
//
// $Id: ttf2txf.cpp,v 1.1 2005/09/09 07:01:26 acornejo Exp $
//
// Converts fonts to TXF texture font format.
// Uses Freetype 2.0.
//
//==========================================================================*/


/*
 TODO:
    * Pack glyphs more efficiently.
    * Save as bitmap.
*/


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>
#include <vector>

using namespace std;

vector<wchar_t> charCodes;


bool _verbose = true;


//----------------------------------------------------------------------------


#define FT_PIXELS(x)  (x >> 6)


struct TexGlyphInfo
{
    unsigned short c;       /* Potentially support 16-bit glyphs. */
    unsigned char width;
    unsigned char height;
    signed char xoffset;
    signed char yoffset;
    signed char advance;
    char dummy;           /* Space holder for alignment reasons. */
    short x;
    short y;
};


struct TexFontWriter
{
    enum eFormat
    {
        TXF_FORMAT_BYTE   = 0,
        TXF_FORMAT_BITMAP = 1
    };

    TexFontWriter() : tgi(0) {}
    ~TexFontWriter();

    void setGlyphCount( int );
    void write( const char* filename );

    int format;
    int tex_width;
    int tex_height;
    int max_ascent;
    int max_descent;
    int num_glyphs;

    //int min_glyph;
    //int range;

    unsigned char* teximage;
    TexGlyphInfo* tgi;
};


TexFontWriter::~TexFontWriter()
{
    delete[] tgi;
}


void TexFontWriter::setGlyphCount( int n )
{
    num_glyphs = n;
    delete[] tgi;
    tgi = new TexGlyphInfo[ n ];
}


void TexFontWriter::write( const char* filename )
{
    FILE* fp;
    int endianness;

    assert( sizeof(int) == 4 );  /* Ensure external file format size. */

    fp = fopen( filename, "wb" );
    if( ! fp )
    {
        fprintf( stderr, "Failed to open %s\n", filename );
        return;
    }

    endianness = 0x12345678;

    fwrite( "\377txf", 1, 4, fp );
    fwrite( &endianness,  sizeof(int), 1, fp );
    fwrite( &format,      sizeof(int), 1, fp );
    fwrite( &tex_width,   sizeof(int), 1, fp );
    fwrite( &tex_height,  sizeof(int), 1, fp );
    fwrite( &max_ascent,  sizeof(int), 1, fp );
    fwrite( &max_descent, sizeof(int), 1, fp );
    fwrite( &num_glyphs,  sizeof(int), 1, fp );
    printf("num_glyphs = %d\n", num_glyphs);

    for( int i = 0; i < num_glyphs; ++i )
    {
        tgi[ i ].dummy = 0;
        fwrite( &tgi[ i ], sizeof(TexGlyphInfo), 1, fp );
    }

    if( format == TXF_FORMAT_BITMAP )
    {
        fprintf( stderr, "TXF_FORMAT_BITMAP not handled\n" );
    }
    else
    {
#if 1
        unsigned char* row = teximage + (tex_width * (tex_height - 1));
        for( int y = 0; y < tex_height; ++y )
        {
            fwrite( row, tex_width, 1, fp );
            row -= tex_width;
        }
#else
        fwrite( teximage, tex_width * tex_height, 1, fp );
#endif
    }

    fclose( fp );
}


//----------------------------------------------------------------------------


void printImg( unsigned char* buf, int pitch, int w, int h )
{
    int x, y;
    
    printf( "%d %d %d\n", pitch, w, h );
    /* Fit on 80 column terminal. */
    if( w > 39 )
        w = 39;

    for( y = 0; y < h; y++ )
    {
        for( x = 0; x < w; x++ )
            printf( "%02x", (int) *(buf + x) );
        printf( "\n" );
        buf += pitch;
    }
    printf( "\n" );
}


void dumpCharMaps( FT_Face face )
{
    FT_CharMap charmap;
    int n;

    printf( "  CharMaps %d [\n", face->num_charmaps );
    for( n = 0; n < face->num_charmaps; n++ )
    {
        long enc;
        charmap = face->charmaps[ n ];
        enc = charmap->encoding;
        printf( "    %lx (%c%c%c%c)\n", enc,
                 (char) ((enc >> 24) & 0xff),
                 (char) ((enc >> 16) & 0xff),
                 (char) ((enc >> 8) & 0xff),
                 (char) (enc & 0xff) );
    }
    printf( "  ]\n" );
}


void blitGlyphToBitmap( FT_Bitmap* src, FT_Bitmap* dst, int x, int y )
{
    unsigned char* s;
    unsigned char* d;
    unsigned char* dend;
    int r;

    s = src->buffer;
    d = dst->buffer + (y * dst->pitch) + x;
    dend = dst->buffer + (dst->rows * dst->pitch);

    r = src->rows;
    while( r && (d < dend) )
    {
        memcpy( d, s, src->width );
        s += src->pitch;
        d += dst->pitch;
        r--;
    }
}


static FT_Error renderGlyph( FT_Bitmap* img, FT_GlyphSlot glyph,
                             int x_offset, int y_offset, bool antialias )
{
    /* first, render the glyph into an intermediate buffer */
    if( glyph->format != ft_glyph_format_bitmap )
    {
        FT_Error error = FT_Render_Glyph( glyph,
                antialias ? ft_render_mode_normal : ft_render_mode_mono );
        if( error )
            return error;
    }
 
#if 0
    printf( "KR offset %dx%d\n", x_offset, y_offset );
    printf( "KR left/top %d %d\n", glyph->bitmap_left, glyph->bitmap_top );
    printf( "KR metrics %ldx%ld %ldx%ld\n", 
            FT_PIXELS(glyph->metrics.width),
            FT_PIXELS(glyph->metrics.height),
            FT_PIXELS(glyph->metrics.horiBearingX),
            FT_PIXELS(glyph->metrics.horiBearingY) );
#endif
 
    /* Then, blit the image to the target surface */
    blitGlyphToBitmap( &glyph->bitmap, img,
                       x_offset + glyph->bitmap_left,
                       y_offset - glyph->bitmap_top );

    return 0;
}


/**
  Returns number of glyphs added or zero if fails.
*/

int buildTXF(TexFontWriter& fontw,
	     const char* file,
	     const vector<wchar_t>& codes,
	     FT_Bitmap* img,
	     int psize,
	     int gap,
	     bool asBitmap)
{
    FT_Library library;
    FT_Face face;
    FT_Error error;
    FT_GlyphSlot glyph;
    FT_Size size;
    FT_F26Dot6 start_x;
    FT_F26Dot6 step_y;
    FT_F26Dot6 x, y;
    int count = 0;
    int fail = 0;


    error = FT_Init_FreeType( &library );
    if( error )
    {
        return 0;
    }

    error = FT_New_Face( library, file, 0, &face );
    if( error )
    {
        return 0;
    }


    if( _verbose )
    {
        printf( "FT_Face [\n" );
        printf( "  family_name: \"%s\"\n", face->family_name );
        printf( "  style_name:  \"%s\"\n",  face->style_name );
        printf( "  num_glyphs:  %ld\n", face->num_glyphs );
        dumpCharMaps( face );
        printf( "]\n" );
    }


    error = FT_Set_Pixel_Sizes( face, psize, psize );
    if( error )
    {
        return 0;
    }


    glyph = face->glyph;
    size  = face->size;


    fontw.setGlyphCount( face->num_glyphs );
    //fontw.max_ascent  = size->metrics.y_ppem;
    // fontw.max_ascent  = FT_PIXELS(face->ascender);
    // fontw.max_descent = -FT_PIXELS(face->descender);
    fontw.max_ascent  = FT_PIXELS((int) (face->ascender * (float) psize / 30.0f));
    fontw.max_descent = -FT_PIXELS((int) (face->descender * (float) psize / 30.0f));


    /* Clear bitmap */
    memset( img->buffer, 0, img->rows * img->pitch );


    step_y = size->metrics.y_ppem + gap;

    start_x = gap;
    x = start_x;
    y = step_y;

    for (unsigned int i = 0; i < codes.size(); i++)
    {
        //int glyph_index = FT_Get_Char_Index( face, *ci );
	int glyph_index = FT_Get_Char_Index(face, codes[i]);
        if( glyph_index == 0 )
        {
            printf( "Code 0x%x is undefined\n", (int) codes[i]);
            continue;
        }

        error = FT_Load_Glyph( face, glyph_index, FT_LOAD_DEFAULT );
        if( ! error )
        {
            int nextX = x + FT_PIXELS(glyph->metrics.horiAdvance) + gap;

            if( nextX > img->width )
            {
                x  = start_x;
                y += step_y;

                //if( (y + step_y) >= img->rows )
                if( y >= img->rows )
                {
                    fprintf( stderr, "Texture too small for %dpt %s\n",
			     psize, file);
                    break;
                }

                nextX = x + FT_PIXELS(glyph->metrics.horiAdvance) + gap;
            }

            renderGlyph( img, glyph, x, y, ! asBitmap );

            TexGlyphInfo& tgi = fontw.tgi[ count ];
            count++;

            tgi.c       = codes[i];
            tgi.width   = FT_PIXELS(glyph->metrics.width);
            tgi.height  = FT_PIXELS(glyph->metrics.height);
            tgi.xoffset = FT_PIXELS(glyph->metrics.horiBearingX);
            tgi.yoffset = FT_PIXELS(glyph->metrics.horiBearingY) - tgi.height;
            tgi.advance = FT_PIXELS(glyph->metrics.horiAdvance);
            tgi.x       = x + tgi.xoffset;
            tgi.y       = fontw.tex_height - y + tgi.yoffset;

	    //printf("code: %04x  size=%dx%d\n", tgi.c, tgi.width, tgi.height);
            x = nextX;
        }
        else
        {
            fail++;
        }
    }

    if( fail )
        printf( "Failed to load %d glyphs.\n", fail );


    FT_Done_Face( face );
    FT_Done_FreeType( library );

    return count;
}


//----------------------------------------------------------------------------


FT_Bitmap _txf;


#ifdef DISPLAY
#include <GL/glut.h>


// Uncomment to debug
#undef GL_EXT_paletted_texture

GLuint _tid;


void build_font_texture( FT_Bitmap* img )
{
    //glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

    glGenTextures( 1, &_tid );
    glBindTexture( GL_TEXTURE_2D, _tid );

    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

#ifdef GL_EXT_paletted_texture
    if( glutExtensionSupported( "GL_EXT_paletted_texture" ) )
    {
        GLubyte cmap[ 256 * 4 ];
        GLubyte* cp = cmap;
        int i;
        for( i = 0; i < 256; i++ )
        {
            *cp++ = i;
            *cp++ = i;
            *cp++ = i;
            *cp++ = i;
        }

        glColorTableEXT( GL_TEXTURE_2D, GL_RGBA, 256,
                         GL_RGBA, GL_UNSIGNED_BYTE, cmap );

        glTexImage2D( GL_TEXTURE_2D, 0, GL_COLOR_INDEX8_EXT,
                      img->width, img->rows, 0,
                      GL_COLOR_INDEX, GL_UNSIGNED_BYTE, img->buffer );
    }
    else
#endif
    {
        /*
        // Doesn't work!
        GLfloat fmap[ 256 ];
        GLfloat* fp = fmap;
        GLfloat i;
        for( i = 0; i < 256; i++ )
        {
            *fp++ = i / 255.0;
        }

        glPixelMapfv( GL_PIXEL_MAP_I_TO_R, 256, fmap );
        glPixelMapfv( GL_PIXEL_MAP_I_TO_G, 256, fmap );
        glPixelMapfv( GL_PIXEL_MAP_I_TO_B, 256, fmap );
        glPixelMapfv( GL_PIXEL_MAP_I_TO_A, 256, fmap );

        glTexImage2D( GL_TEXTURE_2D, 0, 1, img->width, img->rows, 0,
                      GL_COLOR_INDEX, GL_UNSIGNED_BYTE, img->buffer );
        */
#if 1
        int i;
        long size = img->width * img->rows;
        GLubyte* rgba = (GLubyte*) malloc( size * 2 );
        GLubyte* src = img->buffer;
        GLubyte* dst = rgba;
        for( i = 0; i < size; i++ )
        {
            *dst++ = *src;
            *dst++ = *src++;
        }
        glTexImage2D( GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA,
                      img->width, img->rows, 0,
                      GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, rgba );
#else
        int i;
        long size = img->width * img->rows;
        GLubyte* rgba = (GLubyte*) malloc( size * 4 );
        GLubyte* src = img->buffer;
        GLubyte* dst = rgba;
        for( i = 0; i < size; i++ )
        {
            *dst++ = *src;
            *dst++ = *src;
            *dst++ = *src;
            *dst++ = *src;
            ++src;
        }

        glTexImage2D( GL_TEXTURE_2D, 0, 4, img->width, img->rows, 0,
                      GL_RGBA, GL_UNSIGNED_BYTE, rgba );
        free( rgba );
#endif
    }
}


void display()
{
    glClear( GL_COLOR_BUFFER_BIT );

    glDisable( GL_BLEND );
    glDisable( GL_TEXTURE_2D );
    glBegin( GL_QUADS );
    glColor3ub( 255, 0, 0 );   glVertex2f( 0, 0 );
    glColor3ub( 0, 255, 0 );   glVertex2f( 0, _txf.rows );
    glColor3ub( 0, 0, 255 );   glVertex2f( _txf.width, _txf.rows );
    glColor3ub( 0, 0, 0 );     glVertex2f( _txf.width, 0 );
    glEnd();

    glBindTexture( GL_TEXTURE_2D, _tid );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glEnable( GL_TEXTURE_2D );
    glColor3ub( 255, 255, 255 );

    glBegin( GL_QUADS );
    glTexCoord2f( 0.0, 1.0 );   glVertex2f( 0, 0 );
    glTexCoord2f( 0.0, 0.0 );   glVertex2f( 0, _txf.rows );
    glTexCoord2f( 1.0, 0.0 );   glVertex2f( _txf.width, _txf.rows );
    glTexCoord2f( 1.0, 1.0 );   glVertex2f( _txf.width, 0 );
    glEnd();

    glutSwapBuffers();
}


void keyFunc( unsigned char key, int, int )
{
    if( key == 'q' || key == 27 )
        exit( 0 );
}


void reshape( int w, int h )
{
    glViewport(0, 0, w, h);

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluOrtho2D( 0, w, 0, h );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
}
#endif


char _version[] = "1.0";
char _default_codes[] = " ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890abcdefghijklmnopqrstuvwxyz?.;,!*:\"/+-|'@#$%^&<>()[]{}_";


void usage()
{
    printf( "ttf2tex version %s (%s)\n\n", _version, __DATE__ );
    printf( "Useage: ttf2tex [options] <TrueType Font>\n\n" );
    printf( "Options:\n" );
    printf( "  -w <width>      Texture width  (default 256)\n" );
    printf( "  -h <height>     Texture height (default 256)\n" );
    //printf( "  -b              Create bitmap texture\n" );
    printf( "  -f <filename>   File containing character codes to convert\n");
    printf( "  -c <string>     Characters to convert\n" );
    printf( "  -g <gap>        Space between glyphs (default 1)\n" );
    printf( "  -s <size>       Font point size (default 20)\n" );
    printf( "  -o <filename>   Output file for textured font\n" );
    printf( "  -q              Quiet; no output\n" );
}


bool loadCharCodesFile(const string& filename)
{
    FILE* fp;
    fp = fopen(filename.c_str(), "r");
    if (fp == NULL)
	return false;

    for (;;)
    {
	unsigned int i;
	if (fscanf(fp, " %x", &i) != 1)
	    break;
	charCodes.insert(charCodes.end(), i);
    }

    fclose(fp);

    return true;
}


int main( int argc, char** argv )
{
    TexFontWriter fontw;
    int i;
    int gap = 1;
    int size = 20;
    bool asBitmap = false;
    char* infile = 0;
    char outfile[FILENAME_MAX];
    string codesfile;
    char* codes = _default_codes;

    if( argc < 2 )
    {
        usage();
        return 0;
    }

    outfile[ 0 ] = '\0';

    fontw.format     = TexFontWriter::TXF_FORMAT_BYTE;
    fontw.tex_width  = 256;
    fontw.tex_height = 256;

    for( i = 1; i < argc; i++ )
    {
        if( *argv[i] == '-' )
        {
            char* cp = argv[i] + 1;

            if( *cp == 'w' )
            {
                i++;
                if( i >= argc ) break;
                fontw.tex_width = atoi(argv[i]);
            }
            else if( *cp == 'h' )
            {
                i++;
                if( i >= argc ) break;
                fontw.tex_height = atoi(argv[i]);
            }
            else if( *cp == 'c' )
            {
                i++;
                if( i >= argc ) break;
                codes = argv[i];
		printf("codes: %s\n", codes);
            }
            else if( *cp == 'b' )
            {
                asBitmap = true;
            }
            else if( *cp == 'g' )
            {
                i++;
                if( i >= argc ) break;
                gap = atoi(argv[i]);
            }
            else if( *cp == 's' )
            {
                i++;
                if( i >= argc ) break;
                size = atoi(argv[i]);
            }
            else if( *cp == 'o' )
            {
                i++;
                if( i >= argc ) break;
                strcpy( outfile, argv[i] );
            }
            else if( *cp == 'q' )
            {
                _verbose = false;
            }
	    else if (*cp == 'f')
	    {
		i++;
		if (i >= argc)
		    break;
		codesfile = argv[i];
	    }
        }
        else
        {
            infile = argv[i];
        }
    }

    if( ! infile )
    {
        usage();
        exit( -1 );
    }

    if( outfile[ 0 ] == '\0' )
    {
        // Set outfile to base infile and append ".txf"

        char* src = infile;
        char* dst = outfile;
        while( *src )
        {
            if( *src == '/' || *src == '\\' )
            {
                // Reset to strip path.
                dst = outfile;
            }
            else if( *src == '.' )
            {
                break;
            }
            else
            {
                *dst++ = *src;
            }
            ++src;
        }
        strcpy( dst, ".txf" ); 
    }

    _txf.width  = fontw.tex_width;
    _txf.rows   = fontw.tex_height;
    _txf.pitch  = _txf.width;
    _txf.buffer = (unsigned char*) malloc( _txf.pitch * _txf.rows );

    // Populate the list of character codes
    if (!codesfile.empty())
    {
	loadCharCodesFile(codesfile);
    }
    else
    {
	int i = 0;
	while (codes[i] != '\0')
        {
	    charCodes.insert(charCodes.end(), (wchar_t) codes[i]);
	    i++;
	}
    }

    fontw.num_glyphs = buildTXF( fontw, infile, charCodes, &_txf, size, gap,
                                 asBitmap );
    if( ! fontw.num_glyphs )
        return -1;

    if( _verbose )
    {
        printf( "TexFont [\n" );
        printf( "  format:      %d\n", fontw.format );
        printf( "  tex_width:   %d\n", fontw.tex_width );
        printf( "  tex_height:  %d\n", fontw.tex_height );
        printf( "  max_ascent:  %d\n", fontw.max_ascent );
        printf( "  max_descent: %d\n", fontw.max_descent );
        printf( "  num_glyphs:  %d\n", fontw.num_glyphs );
        printf( "]\n" );
    }

    fontw.teximage = _txf.buffer;
    fontw.write( outfile );
    //printImg( _txf.buffer, 256, 256, 100 );


#ifdef DISPLAY
    glutInit( &argc, argv );
    // GLUT_SINGLE does not seem to work with NVidia's OpenGL on GeForce2.
    //glutInitDisplayMode( GLUT_RGB | GLUT_SINGLE );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE );

    glutInitWindowSize( _txf.width, _txf.rows );
    glutCreateWindow( infile );
    glutReshapeFunc( reshape );
    glutDisplayFunc( display );
    glutKeyboardFunc( keyFunc );

    glClearColor( 0.2, 0.2, 0.2, 1.0 );

    build_font_texture( &_txf );

    free( _txf.buffer );

    glutMainLoop();
#else
    free( _txf.buffer );
#endif

    return 0;
}


/*EOF*/
