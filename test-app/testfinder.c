#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"
#include "finder.h"

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

    if (findImageToleranceIn(info, bmp_to_find, &x, &y, 0, 0, info->targetImage->width, info->targetImage->height, 0))
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
