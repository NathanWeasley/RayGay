

#ifndef IMAGE_JPEG_IO_H
#define IMAGE_JPEG_IO_H

#include "imageio.h"

/**
 * A loader and saver for JPEG image files.
 */
class JpegIO : public ImageIO {
    public:
	void save(const Image* const image, const std::string& filename) const;
	Image* load(const std::string& filename);
};

#endif
