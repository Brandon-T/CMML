#include "bitmap.h"

static bool process_pixels(bitmap *bmp, uint32_t size, uint16_t bpp)
{
    uint32_t I, J;
    uint32_t height = bmp->height;
    uint32_t width = bmp->width;
    uint8_t *in = (uint8_t *)bmp->pixels;
    rgb32 *buffer = malloc(width * height * 4);

    if (!buffer)
    {
        free(bmp->pixels);
        bmp->width = 0;
        bmp->height = 0;
        return false;
    }

    for (I = 0; I < height; ++I)
    {
        for (J = 0; J < width; ++J)
        {
            buffer[(height - 1 - I) * width + J].b = *(in++);
            buffer[(height - 1 - I) * width + J].g = *(in++);
            buffer[(height - 1 - I) * width + J].r = *(in++);
            buffer[(height - 1 - I) * width + J].a = (bpp > 24 ? * (in++) : 0xFF);
        }

        if(bpp == 24)
            in += (-width * 3) & 3;
    }

    free(bmp->pixels);
    bmp->pixels = buffer;
    return true;
}

static void un_process_pixels(bitmap *bmp, uint8_t *outbuffer, uint32_t size, uint16_t bpp)
{
    uint32_t I, J;
    rgb32 *in = bmp->pixels;
    uint32_t height = bmp->height;
    uint32_t width = bmp->width;
    uint8_t *out = outbuffer;

    for (I = 0; I < height; ++I)
    {
        for (J = 0; J < width; ++J)
        {
            *(out++) = in[(height - 1 - I) * width + J].b;
            *(out++) = in[(height - 1 - I) * width + J].g;
            *(out++) = in[(height - 1 - I) * width + J].r;

            if (bpp > 24)
                *(out++) = in[(height - 1 - I) * width + J].a;
        }
        if(bpp == 24)
            out += (-width * 3) & 3;
    }
}

bool createbitmap(bitmap *bmp, uint32_t width, uint32_t height)
{
    if (bmp && !bmp->pixels)
    {
        size_t size = ((width * 32 + 31) / 32) * 4 * height;
        if ((bmp->pixels = malloc(size)))
        {
            bmp->width = width;
            bmp->height = height;
            memset(bmp->pixels, 0, size);
            return true;
        }
    }
    return false;
}

bool copybitmap(bitmap *in, bitmap *out)
{
    if (in && in->pixels && out && !out->pixels)
    {
        size_t size = ((in->width * 32 + 31) / 32) * 4 * in->height;
        if ((out->pixels = malloc(size)))
        {
            out->width = in->width;
            out->height = in->height;
            memcpy(out->pixels, in->pixels, size);
            return true;
        }
    }
    return false;
}

bool bitmap_from_file(bitmap *bmp, const char *filepath)
{
    if (!bmp || bmp->pixels)
        return false;

    bmp->width = 0;
    bmp->height = 0;
    bmp->pixels = NULL;

#if defined _WIN32 || defined _WIN64
    FILE *file = fopen(filepath, "rb");
#else
    int file = fopen(filepath, "rb");
#endif

    if (file)
    {
        uint8_t header[54];
        if (fread(header, 1, sizeof(header), file) != sizeof(header))
        {
            perror("Cannot read bitmap header.");
            goto error;
        }

        if (header[0] != 'B' && header[1] != 'M')
        {
            perror("Invalid bitmap. Only 24bit and 32bit bitmaps supported.");
            goto error;
        }

        uint16_t bpp = header[28];
        bmp->width = header[18] + (header[19] << 8);
        bmp->height = header[22] + (header[23] << 8);

        uint32_t pxoffset = header[10] + (header[11] << 8);
        uint32_t size = ((bmp->width * bpp + 31) / 32) * 4 * bmp->height;
        fseek(file, pxoffset, SEEK_SET);

        if (!(bmp->pixels = malloc(size)))
        {
            perror("Cannot allocate pixel memory.");
            goto error;
        }

        if (fread(bmp->pixels, 1, size, file) != size)
        {
            perror("Cannot read all pixels.");
            goto error;
        }

        fclose(file);
        return process_pixels(bmp, size, bpp);
    }
    perror("Cannot open file.");

error:
    fclose(file);
    return false;
}

bool bitmap_from_24bit_string(bitmap *bmp, const char *str)
{
    if (!bmp || bmp->pixels)
        return false;

    int I, J;
    char *buffer;
    uint32_t buff_len;

    if ((str[0] == 'm') && base64decode((const uint8_t *)&str[1], strlen(&str[1]), &buffer, &buff_len))
    {
        uint32_t size = ((bmp->width * 24 + 31) / 32) * 4 * bmp->height;
        rgb24 *pixels = malloc(size);

        if (pixels)
        {
            if (uncompress((Bytef *)pixels, (uLongf *)&size, (Bytef *)buffer, buff_len) == Z_OK)
            {
                bmp->pixels = malloc(((bmp->width * 32 + 31) / 32) * 4 * bmp->height);
                if (bmp->pixels)
                {
                    for (I = 0; I < bmp->height; ++I)
                    {
                        for (J = 0; J < bmp->width; ++J)
                        {
                            bmp->pixels[I * bmp->width + J].r = pixels[I * bmp->width + J].b;
                            bmp->pixels[I * bmp->width + J].g = pixels[I * bmp->width + J].g;
                            bmp->pixels[I * bmp->width + J].b = pixels[I * bmp->width + J].r;
                            bmp->pixels[I * bmp->width + J].a = 0xFF;
                        }
                    }

                    free(pixels);
                    free(buffer);
                    return true;
                }
            }
            free(pixels);
        }
        free(buffer);
    }

    bmp->width = 0;
    bmp->height = 0;
    bmp->pixels = NULL;
    return false;
}

bool bitmap_from_32bit_string(bitmap *bmp, const char *str)
{
    if (!bmp || bmp->pixels)
        return false;

    char *buffer;
    uint32_t buff_len;

    if ((str[0] == 'm') && base64decode((const uint8_t *)&str[1], strlen(&str[1]), &buffer, &buff_len))
    {
        uint32_t size = ((bmp->width * 32 + 31) / 32) * 4 * bmp->height;
        bmp->pixels = malloc(size);

        if (bmp->pixels)
        {
            if (uncompress((Bytef *)bmp->pixels, (uLongf *)&size, (Bytef *)buffer, buff_len) == Z_OK)
            {
                free(buffer);
                return true;
            }

            free(bmp->pixels);
        }

        free(buffer);
    }

    bmp->width = 0;
    bmp->height = 0;
    bmp->pixels = NULL;
    return false;
}

bool bitmap_to_24bit_string(bitmap *bmp, char **str, uint32_t *len)
{
    if (!bmp || !bmp->pixels)
        return false;

    int I, J;
    uint32_t size = ((bmp->width * 24 + 31) / 32) * 4 * bmp->height;
    rgb24 *pixels = malloc(size);

    if (pixels)
    {
        for (I = 0; I < bmp->height; ++I)
        {
            for (J = 0; J < bmp->width; ++J)
            {
                pixels[I * bmp->width + J].b = bmp->pixels[I * bmp->width + J].r;
                pixels[I * bmp->width + J].g = bmp->pixels[I * bmp->width + J].g;
                pixels[I * bmp->width + J].r = bmp->pixels[I * bmp->width + J].b;
            }
        }

        uint32_t destlen = compressBound(size);
        *str = malloc(destlen);

        if (*str)
        {
            if (compress((Bytef *)*str, (uLongf *)&destlen, (Bytef *)pixels, size) == Z_OK)
            {
                free(pixels);
                pixels = NULL;

                char *b64str;
                uint32_t b64_len;

                if (base64encode((const uint8_t *)*str, destlen, &b64str, &b64_len))
                {
                    free(*str);
                    *str = b64str;
                    *len = b64_len + 2;

                    b64str = malloc(*len);
                    if (b64str)
                    {
                        b64str[0] = 'm';
                        strncpy(&b64str[1], *str, b64_len);
                        free(*str);
                        *str = b64str;
                        (*str)[b64_len + 1] = '\0';
                        return true;
                    }
                }
            }

            free(*str);
            *len = 0;
            *str = NULL;
        }
        free(pixels);
    }
    return false;
}

bool bitmap_to_32bit_string(bitmap *bmp, char **str, uint32_t *len)
{
    if (!bmp || !bmp->pixels)
        return false;

    uint32_t size = ((bmp->width * 32 + 31) / 32) * 4 * bmp->height;
    uint32_t destlen = compressBound(size);
    *str = malloc(destlen);

    if (*str)
    {
        if (compress((Bytef *)*str, (uLongf *)&destlen, (Bytef *)bmp->pixels, size) == Z_OK)
        {
            char *b64str;
            uint32_t b64_len;

            if (base64encode((const uint8_t *)*str, destlen, &b64str, &b64_len))
            {
                free(*str);
                *str = b64str;
                *len = b64_len + 2;

                b64str = malloc(*len);
                if (b64str)
                {
                    b64str[0] = 'm';
                    strncpy(&b64str[1], *str, b64_len);
                    free(*str);
                    *str = b64str;
                    (*str)[b64_len + 1] = '\0';
                    return true;
                }
            }
        }

        free(*str);
        *len = 0;
        *str = NULL;
    }
    return false;
}

bool savebmp(bitmap *bmp, const char *filepath, uint16_t bpp)
{
    if (!bmp || !bmp->pixels)
        return false;

#if defined _WIN32 || defined _WIN64
    FILE *file = fopen(filepath, "wb");
#else
    int file = fopen(filepath, "wb");
#endif

    if (file)
    {
        uint32_t trash = 0;
        uint16_t planes = 1;
        uint32_t biSize = 40;
        uint16_t type = 0x4D42;
        uint32_t compression = 0;
        uint32_t pxoffsetbits = 54;
        uint32_t size = ((bmp->width * bpp + 31) / 32) * 4 * bmp->height;
        uint32_t bfSize = 54 + size;

        uint8_t *pixels = malloc(size);
        un_process_pixels(bmp, pixels, size, bpp);

        fwrite(&type, sizeof(type), 1, file);
        fwrite(&bfSize, sizeof(bfSize), 1, file);
        fwrite(&trash, sizeof(trash), 1, file);
        fwrite(&pxoffsetbits, sizeof(pxoffsetbits), 1, file);
        fwrite(&biSize, sizeof(biSize), 1, file);
        fwrite(&bmp->width, sizeof(bmp->width), 1, file);
        fwrite(&bmp->height, sizeof(bmp->height), 1, file);
        fwrite(&planes, sizeof(planes), 1, file);
        fwrite(&bpp, sizeof(bpp), 1, file);
        fwrite(&compression, sizeof(compression), 1, file);
        fwrite(&size, sizeof(size), 1, file);
        fwrite(&trash, sizeof(trash), 1, file);
        fwrite(&trash, sizeof(trash), 1, file);
        fwrite(&trash, sizeof(trash), 1, file);
        fwrite(&trash, sizeof(trash), 1, file);
        fwrite(pixels, sizeof(uint8_t), size, file);

        fclose(file);
        free(pixels);
        return true;
    }

    fclose(file);
    perror("Cannot open file.");
    return false;
}

void freebmp(bitmap *bmp)
{
    if (bmp && bmp->pixels)
    {
        free(bmp->pixels);
        bmp->width = 0;
        bmp->height = 0;
        bmp->pixels = NULL;
    }
}
