// image_load.c

//
// Handles loading PNG images and reformatting them to a usable state
//

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "common.h"
#include "logging.h"
#include "image_info.h"
#include "options.h"

#include <defines.h>
#include <lodepng.h>


static bool image_validate(image_data *);
static bool image_load_png(image_data *, const char *);


// Validates incoming image properties
// Return: true if success
static bool image_validate(image_data * p_decoded_image) {

    if (p_decoded_image->width != VALIDATE_WIDTH) {
        log_error("Error: Image width is %d, must be %d\n", p_decoded_image->width, VALIDATE_WIDTH);
        return false;
    }

    // TODO: There is a soft upper bound of 25 since:
    //       Width is currently fixed at 160 pixels (20 tiles)
    //       20 tiles wide x 25 tiles high = 500 tiles
    //       Which is the highest possible within the 512 available on the CGB
    //       without reloading the tiles and maps using an offset
    if ((p_decoded_image->height == 0) || (p_decoded_image->height > VALIDATE_HEIGHT)) {
        log_error("Error: Image height is %d, must be between 0 and %d\n", p_decoded_image->height, VALIDATE_HEIGHT);
        return false;
    }

    if ((p_decoded_image->height % 8) != 0u) {
        log_verbose("Error: Image height must be a multiple of 8, %d is not\n", p_decoded_image->height);
        return false;
    }

    if ((p_decoded_image->width % 8) != 0u) {
        log_verbose("Error: Image width must be a multiple of 8, %d is not\n", p_decoded_image->width);
        return false;
    }

    return true;
}


// Loads a PNG image image into RGB888(24) format
// Return: true if success
static bool image_load_png(image_data * p_decoded_image, const char * filename) {

    bool status = true;
    LodePNGState png_state;
    lodepng_state_init(&png_state);

    // Decode with auto conversion to RGB888(24)
    unsigned error = lodepng_decode24_file(&p_decoded_image->p_img_data, &p_decoded_image->width, &p_decoded_image->height, filename);

    if (error) {
        status = false;
        log_error("Error: PNG load: %u: %s\n", error, lodepng_error_text(error));
    } else {
        p_decoded_image->bytes_per_pixel = RGB_24SZ;
        p_decoded_image->size = p_decoded_image->width * p_decoded_image->height * p_decoded_image->bytes_per_pixel;
    }

    // Free resources
    lodepng_state_cleanup(&png_state);

    return status;
}


// Loads an image image
// Return: true if success
bool image_load(image_data * p_decoded_image, const char * filename, const uint8_t image_type) {

    bool status = true;

    log_verbose("Loading image from file: %s, type: %d\n", filename, image_type);

    switch (image_type) {
        case IMG_TYPE_PNG:
            status = image_load_png(p_decoded_image, filename);
            break;

        default:
            status = false;
            log_error("Invalid image format. No image will be loaded\n");
            break;
    }

    if (status)
        status = image_validate(p_decoded_image);

    if (status) {
        log_verbose("Decoded image.width: %d\n", p_decoded_image->width);
        log_verbose("Decoded image.height: %d\n", p_decoded_image->height);
        log_verbose("Decoded image.size: %d\n", p_decoded_image->size);
        log_verbose("Decoded image.bytes_per_pixel: %d\n", p_decoded_image->bytes_per_pixel);
    }

    return status;
}

