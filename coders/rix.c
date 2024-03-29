#include "MagickCore/studio.h"
#include "MagickCore/attribute.h"
#include "MagickCore/blob.h"
#include "MagickCore/blob-private.h"
#include "MagickCore/cache.h"
#include "MagickCore/color-private.h"
#include "MagickCore/colormap.h"
#include "MagickCore/colorspace.h"
#include "MagickCore/colorspace-private.h"
#include "MagickCore/exception.h"
#include "MagickCore/exception-private.h"
#include "MagickCore/image.h"
#include "MagickCore/image-private.h"
#include "MagickCore/list.h"
#include "MagickCore/magick.h"
#include "MagickCore/memory_.h"
#include "MagickCore/monitor.h"
#include "MagickCore/monitor-private.h"
#include "MagickCore/pixel-accessor.h"
#include "MagickCore/quantum-private.h"
#include "MagickCore/static.h"
#include "MagickCore/string_.h"
#include "MagickCore/module.h"
#include "MagickCore/utility.h"

static MagickBooleanType IsRIX(const unsigned char *magick, const size_t length)
{
  if (length < 4)
    return(MagickFalse);
  if (memcmp(magick, "RIX3", 4) == 0)
    return(MagickTrue);
  return(MagickFalse);
}

static Image *ReadRIXImage(const ImageInfo *image_info, ExceptionInfo *exception)
{
  Image *image;
  MagickBooleanType status;
  size_t length;
  unsigned char header[10], *palette_data = NULL, *pixel_indexes = NULL;
  ssize_t x, y;
  Quantum *q;
  unsigned int width, height, palette_length = 0, palette_type;


  /*
    Open image file.
  */
  image = AcquireImage(image_info, exception);
  status = OpenBlob(image_info, image, ReadBinaryBlobMode, exception);
  if (status == MagickFalse)
  {
    image = DestroyImageList(image);
    return (Image *) NULL;
  }

  /*
    Read the RIX header.
  */
  length = ReadBlob(image, 10, header);
  if (length != 10)
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");

  width = 0;
  height = 0;
  memcpy(&width, header + 4, 2);
  memcpy(&height, header + 6, 2);

  if (width <= 0 || height <= 0)
    ThrowReaderException(CorruptImageError,"NegativeOrZeroImageSize");

  image->columns=(size_t) width;
  image->rows=(size_t) height;

  palette_type = header[8];

  /*
    Determine palette length.
  */
  if (palette_type == 0xCB || palette_type == 0xAB)
  {
    /* EGA (0xCB) or Extended EGA (0xAB) */
    palette_length = 16;
  }
  else if (palette_type == 0xAF || palette_type == 0x9F)
  {
    /* VGA or IBM PGA */
    palette_length = 256;
  }
  else
  {
    ThrowReaderException(CorruptImageError,"UnsupportedPaletteType");
  }

  /*
    Read the palette data.
  */
  palette_data = (unsigned char *) AcquireQuantumMemory(palette_length, 3 * sizeof(*palette_data));
  if (palette_data == (unsigned char *) NULL)
    ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
  length = ReadBlob(image, 3 * palette_length, palette_data);
  if (length != (3 * palette_length))
  {
    palette_data = RelinquishMagickMemory(palette_data);
    ThrowReaderException(CorruptImageError,"UnableToReadImageHeader");
  }

  /*
    Read the image data.
  */
  pixel_indexes = (unsigned char *) AcquireQuantumMemory(image->columns, sizeof(*pixel_indexes));
  if (pixel_indexes == (unsigned char *) NULL)
  {
    palette_data = RelinquishMagickMemory(palette_data);
    ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
  }

  for (y = 0; y < (ssize_t)image->rows; y++)
  {
    ssize_t count = ReadBlob(image, image->columns, pixel_indexes);
    if (count != (ssize_t)image->columns)
    {
      palette_data = RelinquishMagickMemory(palette_data);
      pixel_indexes = RelinquishMagickMemory(pixel_indexes);
      ThrowReaderException(CorruptImageError,"UnexpectedEndOfFile");
    }

    q = QueueAuthenticPixels(image, 0, y, image->columns, 1, exception);
    if (q == (Quantum *) NULL) break;

    for (x = 0; x < (ssize_t)image->columns; x++)
    {
      unsigned char pixel_index = pixel_indexes[x];
      /* Ensure the index is within the bounds of the palette_data array */
      if (pixel_index >= palette_length) continue;

      /* Calculate the offset within the palette_data array */
      size_t palette_offset = 3 * pixel_index;
      SetPixelRed(image, ScaleCharToQuantum(palette_data[palette_offset]) * 4.05, q);
      SetPixelGreen(image, ScaleCharToQuantum(palette_data[palette_offset + 1]) * 4.05, q);
      SetPixelBlue(image, ScaleCharToQuantum(palette_data[palette_offset + 2]) * 4.05, q);

      q += GetPixelChannels(image);
    }
    if (SyncAuthenticPixels(image, exception) == MagickFalse) break;
  }  

  palette_data = RelinquishMagickMemory(palette_data);
  pixel_indexes = RelinquishMagickMemory(pixel_indexes);

  if (CloseBlob(image) == MagickFalse) {
    status = MagickFalse;
  }
  if (status == MagickFalse) {
    return(DestroyImageList(image));
  }
  return(GetFirstImageInList(image));
}

ModuleExport size_t RegisterRIXImage(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("RIX", "RIX",
    "ColoRIX VGA Paint Image");
  entry->decoder=(DecodeImageHandler *) ReadRIXImage;
  entry->encoder=(EncodeImageHandler *) NULL;
  entry->magick=(IsImageFormatHandler *) IsRIX;
  entry->flags|=CoderDecoderSeekableStreamFlag; // RIX format likely requires seekable stream for decoding.
  entry->flags^=CoderAdjoinFlag; // Remove if RIX images should not be treated as sequence.

  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}

ModuleExport void UnregisterRIXImage(void)
{
  (void) UnregisterMagickInfo("RIX");
}

