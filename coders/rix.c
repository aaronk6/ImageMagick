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

/*
  Forward declarations.
*/
static MagickBooleanType
  WriteRIXImage(const ImageInfo *, Image *, ExceptionInfo *);

static MagickBooleanType IsRIX(const unsigned char *magick, const size_t length)
{
  if (length < 3)
    return(MagickFalse);
  if (memcmp(magick, "RIX", 3) == 0)
    return(MagickTrue);
  return(MagickFalse);
}

static Image *ReadRIXImage(const ImageInfo *image_info, ExceptionInfo *exception)
{
  Image *image;
  MagickBooleanType status;

  /* Allocate an Image structure. */
  image=AcquireImage(image_info,exception);
  if (image == (Image *) NULL)
    return((Image *) NULL);

  /* Print a message to stdout for testing purposes. */
  printf("ReadRIXImage stub function called.\n");

  /* Dummy operation to avoid unused variable warnings for this stub. */
  status=SetImageExtent(image,0,0,exception);
  if (status == MagickFalse)
    {
      image=DestroyImage(image);
      return((Image *) NULL);
    }

  /* Return the empty image for now. */
  return(image);
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

