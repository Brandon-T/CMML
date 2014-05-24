#include "finder.h"

void initPointArray(PointArray* pa)
{
    pa->p = NULL;
    pa->size = 0;
}

void freePointArray(PointArray* pa)
{
    free(pa->p);
    pa->p = NULL;
    pa->size = 0;
}

static bool __CTSN(void *this_ptr, rgb32 *first, rgb32 *second)
{
    return (first->r == second->r) && (first->g == second->g) && (first->b == second->b); //first->color == second->color;
}

static bool __CTS0(void *this_ptr, rgb32 *first, rgb32 *second)
{
    uint16_t tol = ((CTSInfo *) this_ptr)->tol;
    return abs(first->r - second->r) <= tol && abs(first->g - second->g) <= tol && abs(first->b - second->b) <= tol;
}

static bool __CTS1(void *this_ptr, rgb32 *first, rgb32 *second)
{
    uint16_t tol = ((CTSInfo *) this_ptr)->tol;
    return ((first->r - second->r) * (first->r - second->r) + (first->g - second->g) * (first->g - second->g) + (first->b - second->b) * (first->b - second->b)) <= (tol * tol);
}

static bool __CTS2(void *this_ptr, rgb32 *first, rgb32 *second)
{
    CTSInfo *info = this_ptr;
    static hsl hfirst = {0}, hsecond = {0};

    if (first->r != 0 && first->g != 0 && first->b != 0)
    {
        hfirst = rgb_to_hsl(first);
    }

    if (second->r != 0 && second->g != 0 && second->b != 0)
    {
        hsecond = rgb_to_hsl(second);
    }

    return (fabs(hsecond.h - hfirst.h) <= (info->tol * info->hueMod)) && (fabs(hsecond.s - hfirst.s) <= (info->tol * info->satMod));
}

static bool __CTS3(void *this_ptr, rgb32 *first, rgb32 *second)
{
    CTSInfo *info = this_ptr;
    static lab lfirst = {0}, lsecond = {0};

    if (first->r != 0 && first->g != 0 && first->b != 0)
    {
        xyz temp = rgb_to_xyz(first);
        lfirst = xyz_to_lab(&temp);
    }

    if (second->r != 0 && second->g != 0 && second->b != 0)
    {
        xyz temp = rgb_to_xyz(second);
        lsecond = xyz_to_lab(&temp);
    }

    double L = (lsecond.l - lfirst.l);
    double A = (lsecond.a - lfirst.a);
    double B = (lsecond.b - lfirst.b);

    return (L * L) + (A * A) + (B * B) <= ceil(sqrt(info->tol * info->tol));
}

void setCTS(CTSInfo *info, int16_t CTSNum)
{
    if (!info)
        return;

    switch(CTSNum)
    {
        case -1:
            info->CTSNum = -1;
            info->ctsFuncPtr = &__CTSN;
            break;

        case 0:
            info->CTSNum = 0;
            info->ctsFuncPtr = &__CTS0;
            break;

        case 1:
            info->CTSNum = 1;
            info->ctsFuncPtr = &__CTS1;
            break;

        case 2:
            info->CTSNum = 2;
            info->ctsFuncPtr = &__CTS2;
            break;

        case 3:
            info->CTSNum = 3;
            info->ctsFuncPtr = &__CTS3;
            break;

        default:
            info->CTSNum = 1;
            info->ctsFuncPtr = &__CTS1;
            break;
    }
}

void defaultCTS(CTSInfo *info)
{
    if (info)
    {
        info->CTSNum = 1;
        info->tol = 0.0f;
        info->hueMod = 0.2f;
        info->satMod = 0.2f;
        info->targetImage = NULL;
        info->ctsFuncPtr = &__CTS1;
    }
}

bool similarColours(CTSInfo *info, rgb32 *first, rgb32 *second, uint16_t tolerance)
{
    info->tol = tolerance;
    return (*info->ctsFuncPtr)(info, first, second); //return ((first->r - second->r) * (first->r - second->r) + (first->g - second->g) * (first->g - second->g) + (first->b - second->b) * (first->b - second->b)) <= (tolerance * tolerance);
}

uint32_t countColour(CTSInfo *info, rgb32 *colour, int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    int16_t temp = info->CTSNum;
    setCTS(info, -1);
    uint32_t Res = countColourTolerance(info, colour, x1, y1, x2, y2, 0);
    setCTS(info, temp);
    return Res;
}

uint32_t countColourTolerance(CTSInfo *info, rgb32 *colour, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t tolerance)
{
    int I, J;;
    uint32_t Result = 0;
    info->tol = tolerance;

    for (I = y1; I < y2; ++I)
    {
        for (J = x1; J < x2; ++J)
        {
            if ((*info->ctsFuncPtr)(info, colour, &info->targetImage->pixels[I * info->targetImage->width + J]))
            {
                ++Result;
            }
        }
    }
    return Result;
}

bool findColour(CTSInfo *info, int32_t *x, int32_t *y, rgb32 *colour, int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    int16_t temp = info->CTSNum;
    setCTS(info, -1);
    bool Res = findColourTolerance(info, x, y, colour, x1, y1, x2, y2, 0);
    setCTS(info, temp);
    return Res;
}

bool findColourTolerance(CTSInfo *info, int32_t *x, int32_t *y, rgb32 *colour, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t tolerance)
{
    int I, J;
    info->tol = tolerance;

    for (I = y1; I < y2; ++I)
    {
        for (J = x1; J < x2; ++J)
        {
            if ((*info->ctsFuncPtr)(info, colour, &info->targetImage->pixels[I * info->targetImage->width + J]))
            {
                *x = J;
                *y = I;
                return true;
            }
        }
    }

    *x = -1;
    *y = -1;
    return false;
}

bool findColours(CTSInfo *info, PointArray *points, rgb32 *colour, int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    int16_t temp = info->CTSNum;
    setCTS(info, -1);
    bool Result = findColoursTolerance(info, points, colour, x1, y1, x2, y2, 0);
    setCTS(info, temp);
    return Result;
}

bool findColoursTolerance(CTSInfo *info, PointArray *points, rgb32 *colour, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t tolerance)
{
    if (!points || points->p)
        return false;

    int I, J;
    points->p = NULL;
    points->size = 0;
    info->tol = tolerance;

    for (I = y1; I < y2; ++I)
    {
        for (J = x1; J < x2; ++J)
        {
            if ((*info->ctsFuncPtr)(info, colour, &info->targetImage->pixels[I * info->targetImage->width + J]))
            {
                Point *loc = realloc(points->p, sizeof(Point) * (points->size + 1));
                if (loc)
                {
                    loc[points->size].x = I;
                    loc[points->size].y = J;

                    points->p = loc;
                    ++points->size;
                    continue;
                }

                free(points->p);
                points->p = NULL;
                points->size = 0;
            }
        }
    }
    return points->p;
}

bool findImage(CTSInfo *info, bitmap *imageToFind, int32_t *x, int32_t *y)
{
    return findImageIn(info, imageToFind, x, y, 0, 0, info->targetImage->width, info->targetImage->height);
}

bool findImageIn(CTSInfo *info, bitmap *imageToFind, int32_t *x, int32_t *y, int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    uint16_t temp = info->CTSNum;
    setCTS(info, -1);
    bool Res = findImageToleranceIn(info, imageToFind, x, y, x1, y1, x2, y2, 0);
    setCTS(info, temp);
    return Res;
}

bool findImageToleranceIn(CTSInfo *info, bitmap *imageToFind, int32_t *x, int32_t *y, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t tolerance)
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
