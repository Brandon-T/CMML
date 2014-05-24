#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"
#include "finder.h"



bool findImageToleranceIn2(CTSInfo *info, bitmap *imageToFind, int32_t *x, int32_t *y, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t tolerance)
{
    int I, J, XX, YY;
    info->tol = tolerance;
    int dX = (x2 - x1) - (imageToFind->width - 1);
    int dY = (y2 - y1) - (imageToFind->height - 1);

    for (I = 0; I < dY; ++I)
    {
        for (J = 0; J < dX; ++J)
        {
            for (YY = 0; YY < imageToFind->height; ++YY)
            {
                for (XX = 0; XX < imageToFind->width; ++XX)
                {
                    rgb32* pixel = &imageToFind->pixels[YY * imageToFind->width + XX];
                    rgb32* targetPixel = &info->targetImage->pixels[(YY + I) * info->targetImage->width + (XX + J)];

                    if (pixel->a != 0)
                    {
                        if (!(*info->ctsFuncPtr)(info, pixel, targetPixel))
                        {
                            goto Skip;
                        }
                    }
                }
            }

            *x = J + x1;
            *y = I + y1;
            return true;

            Skip:
                continue;
        }
    }

    *x = -1;
    *y = -1;
    return false;
}


bool findImageIn2(CTSInfo *info, bitmap *imageToFind, int32_t *x, int32_t *y, int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    uint16_t temp = info->CTSNum;
    setCTS(info, -1);
    bool Res = findImageToleranceIn2(info, imageToFind, x, y, x1, y1, x2, y2, 0);
    setCTS(info, temp);
    return Res;
}

bool findImage2(CTSInfo *info, bitmap *imageToFind, int32_t *x, int32_t *y)
{
    return findImageIn2(info, imageToFind, x, y, 0, 0, info->targetImage->width, info->targetImage->height);
}


void TestOne(CTSInfo* info)
{
    rgb32 colour = {144, 240, 255, 0};
    PointArray pts;
    initPointArray(&pts);

    if (findColours(info, &pts, &colour, 0, 0, info->targetImage->width, info->targetImage->height))
    {
        Point *p = pts.p;
        int I = 0;
        for (; I < pts.size; ++I, ++p)
            printf("Colour found at: (%d, %d)\n", p->x, p->y);
    }

    freePointArray(&pts);
}

void TestTwo(CTSInfo* info, bitmap* bmp_to_find)
{
    int x, y;

    if (findImageToleranceIn2(info, bmp_to_find, &x, &y, 0, 0, info->targetImage->width, info->targetImage->height, 0))
    {
        printf("Image found at: (%d, %d)\n", x, y);
    }
}

int main()
{
    CTSInfo info = {0};
    bitmap bmp_to_find = {0};
    bitmap bmp_target = {0};

    defaultCTS(&info);
    bitmap_from_file(&bmp_to_find, "C:/Users/Brandon/Desktop/find.bmp");
    bitmap_from_file(&bmp_target, "C:/Users/Brandon/Desktop/target.bmp");
    info.targetImage = &bmp_target;



    TestOne(&info);
    printf("\n");

    TestTwo(&info, &bmp_to_find);
    printf("\n");


    freebmp(&bmp_to_find);
    freebmp(&bmp_target);
    return 0;
}
