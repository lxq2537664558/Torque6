//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#include "io/stream.h"
#include "io/fileStream.h"
#include "io/memstream.h"
#include "graphics/gPalette.h"
#include "graphics/gBitmap.h"

#ifdef TORQUE_OS_WIN32
#define HAVE_BOOLEAN
#endif

#define XMD_H
#include "jpeglib.h"

U32 gJpegQuality = 90;


//-------------------------------------- Replacement I/O for standard LIBjpeg
//                                        functions.  we don't wanna use
//                                        FILE*'s...
static int jpegReadDataFn(void *client_data, unsigned char *data, int length)
{
   Stream *stream = (Stream*)client_data;
   AssertFatal(stream != NULL, "jpegReadDataFn::No stream.");
   int pos = stream->getPosition();
   if (stream->read(length, data))
      return length;

   if (stream->getStatus() == Stream::EOS)
      return (stream->getPosition()-pos);
   else
      return 0;
}


//--------------------------------------
static int jpegWriteDataFn(void *client_data, unsigned char *data, int length)
{
   Stream *stream = (Stream*)client_data;
   AssertFatal(stream != NULL, "jpegWriteDataFn::No stream.");
   if (stream->write(length, data))
      return length;
   else
      return 0;
}


//--------------------------------------
static int jpegFlushDataFn(void *)
{
   // do nothing since we can't flush the stream object
   return 0;
}


//--------------------------------------
static int jpegErrorFn(void *client_data)
{
   Stream *stream = (Stream*)client_data;
   AssertFatal(stream != NULL, "jpegErrorFn::No stream.");
   return (stream->getStatus() != Stream::Ok);
}


//--------------------------------------
bool GBitmap::readJPEG(Stream &stream)
{
   JFREAD  = jpegReadDataFn;
   JFERROR = jpegErrorFn;

   jpeg_decompress_struct cinfo;
   jpeg_error_mgr jerr;

   // We set up the normal JPEG error routines, then override error_exit.
   //cinfo.err = jpeg_std_error(&jerr.pub);
   //jerr.pub.error_exit = my_error_exit;

   // if (setjmp(jerr.setjmp_buffer))
   // {
   //    // If we get here, the JPEG code has signaled an error.
   //    // We need to clean up the JPEG object, close the input file, and return.
   //    jpeg_destroy_decompress(&cinfo);
   //    return false;
   // }


   cinfo.err = jpeg_std_error(&jerr);    // set up the normal JPEG error routines.
   cinfo.client_data = (void*)&stream;       // set the stream into the client_data

   // Now we can initialize the JPEG decompression object.
   jpeg_create_decompress(&cinfo);

   jpeg_stdio_src(&cinfo);

   // Read file header, set default decompression parameters
   jpeg_read_header(&cinfo, true);

   BitmapFormat format;
   switch (cinfo.out_color_space)
   {
      case JCS_GRAYSCALE:  format = Alpha; break;
      case JCS_RGB:        format = RGB;   break;
      default:
         jpeg_destroy_decompress(&cinfo);
         return false;
   }

   // Start decompressor
   jpeg_start_decompress(&cinfo);

   // allocate the bitmap space and init internal variables...
   allocateBitmap(cinfo.output_width, cinfo.output_height, false, format);

   // Set up the row pointers...
   U32 rowBytes = cinfo.output_width * cinfo.output_components;

   U8* pBase = (U8*)getBits();
   for (U32 i = 0; i < height; i++)
   {
      JSAMPROW rowPointer = pBase + (i * rowBytes);
      jpeg_read_scanlines(&cinfo, &rowPointer, 1);
   }

   // Finish decompression
   jpeg_finish_decompress(&cinfo);

   // Release JPEG decompression object
   // This is an important step since it will release a good deal of memory.
   jpeg_destroy_decompress(&cinfo);

   return true;
}


//--------------------------------------------------------------------------
bool GBitmap::writeJPEG(Stream& stream) const
{
   // JPEG format has no support for transparancy so any image
   // in Alpha format should be saved as a grayscale which coincides
   // with how the readJPEG function will read-in a JPEG. So the
   // only formats supported are RGB and Alpha, not RGBA.
   AssertFatal(getFormat() == RGB || getFormat() == Alpha,
                "GBitmap::writeJPEG: ONLY RGB bitmap writing supported at this time.");
   if (internalFormat != RGB && internalFormat != Alpha)
      return false;

   // maximum image size allowed
   #define MAX_HEIGHT 4096
   if (height >= MAX_HEIGHT)
      return false;

   // Bind our own stream writing, error, and memory flush functions
   // to the jpeg library interface
   JFWRITE = jpegWriteDataFn;
   JFFLUSH = jpegFlushDataFn;
   JFERROR = jpegErrorFn;

   // Allocate and initialize our jpeg compression structure and error manager
   jpeg_compress_struct cinfo;
   jpeg_error_mgr jerr;

   cinfo.err = jpeg_std_error(&jerr);    // set up the normal JPEG error routines.
   cinfo.client_data = (void*)&stream;   // set the stream into the client_data
   jpeg_create_compress(&cinfo);         // allocates a small amount of memory

   // specify the destination for the compressed data(our stream)
   jpeg_stdio_dest(&cinfo);

   // set the image properties
   cinfo.image_width = getWidth();           // image width
   cinfo.image_height = getHeight();         // image height
   cinfo.input_components = bytesPerPixel;   // samples per pixel(RGB:3, Alpha:1)
   switch (internalFormat)
   {
      case Alpha:  // no alpha support in JPEG format, so turn it into a grayscale
         cinfo.in_color_space = JCS_GRAYSCALE;
         break;
      case RGB:    // otherwise we are writing in RGB format
         cinfo.in_color_space = JCS_RGB;
         break;
      case Palettized:
      case Intensity:
      case RGBA:
      case RGB565:
      case RGB5551:
      case Luminance:
      case LuminanceAlpha:
#ifdef TORQUE_OS_IOS
      case PVR2:
      case PVR2A:
      case PVR4:
      case PVR4A:
#endif
       {
           // error, unhandled case
       }
   }
   // use default compression params(75% compression)
   jpeg_set_defaults(&cinfo);
   jpeg_set_quality(&cinfo, gJpegQuality, false);

   // begin JPEG compression cycle
   jpeg_start_compress(&cinfo, true);

   // Set up the row pointers...
   U32 rowBytes = cinfo.image_width * cinfo.input_components;

   U8* pBase = (U8*)getBits();
   for (U32 i = 0; i < height; i++)
   {
      // write the image data
      JSAMPROW rowPointer = pBase + (i * rowBytes);
      jpeg_write_scanlines(&cinfo, &rowPointer, 1);
   }

   // complete the compression cycle
   jpeg_finish_compress(&cinfo);

   // release the JPEG compression object
   jpeg_destroy_compress(&cinfo);

   // return success
   return true;
}

