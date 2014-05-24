#include "color.h"

static uint8_t __hsl_to_rgb_helper(float i, float j, float h)
{
    if (h < 0.0f) h += 1.0f;
    if (h > 1.0f) h -= 1.0f;
    if (h < (1.0f / 6.0f))
        return (uint8_t)round((i + (j - i) * 6.0f * h) * 255.0f);
    else if (h < (1.0f / 2.0f))
        return (uint8_t)round(j * 255.0f);
    else if (h < (2.0f / 3.0f))
        return (uint8_t)round((i + (j - i) * 6.0f * ((2.0f / 3.0f) - h)) * 255.0f);
    else
        return (uint8_t)round(i * 255.0f);
}

xyz rgb_to_xyz(rgb32 *px)
{
    xyz res;
    float r = (px->r / 255.0f);
    float g = (px->g / 255.0f);
    float b = (px->b / 255.0f);

    r = (r > 0.04045f) ? pow(((r + 0.055f) / 1.055f), 2.4f) * 100.0f : r / 12.92f;
    g = (g > 0.04045f) ? pow(((g + 0.055f) / 1.055f), 2.4f) * 100.0f : g / 12.92f;
    b = (b > 0.04045f) ? pow(((b + 0.055f) / 1.055f), 2.4f) * 100.0f : b / 12.92f;
    res.x = r * 0.4124f + g * 0.3576f + b * 0.1805f;
    res.y = r * 0.2126f + g * 0.7152f + b * 0.0722f;
    res.z = r * 0.0193f + g * 0.1192f + b * 0.9505f;
    return res;
}

rgb32 xyz_to_rgb(xyz *px)
{
    rgb32 res;
    float x = px->x / 100.0f;
    float y = px->y / 100.0f;
    float z = px->z / 100.0f;

    float r = x * 3.2406f + y * -1.5372f + z * -0.4986f;
    float g = x * -0.9689f + y * 1.8758f + z * 0.0415f;
    float b = x * 0.0557f + y * -0.2040f + z * 1.0570f;

    r = (r > 0.0031308f) ? (1.055f * pow(r, (1.0f / 2.4f)) - 0.055f) : 12.92f * r;
    g = (g > 0.0031308f) ? (1.055f * pow(g, (1.0f / 2.4f)) - 0.055f) : 12.92f * g;
    b = (b > 0.0031308f) ? (1.055f * pow(b, (1.0f / 2.4f)) - 0.055f) : 12.92f * b;

    res.r = (uint8_t)round(r * 255.0f);
    res.g = (uint8_t)round(g * 255.0f);
    res.b = (uint8_t)round(b * 255.0f);
    return res;
}

hsl rgb_to_hsl(rgb32 *px)
{
    hsl res;
    float r, g, b;
    r = px->r / 255.0f;
    g = px->g / 255.0f;
    b = px->b / 255.0f;
    float Max = r > b ? r > g ? r : g : b > g ? b : g;
    float Min = r < b ? r < g ? r : g : b < g ? b : g;
    res.l = (Max + Min) / 2.0f;

    if(Max == Min)
        res.h = res.s = 0.0f;
    else
    {
        float Delta = Max - Min;
        res.s = res.l > 0.5f ? Delta / (2.0f - Max - Min) : Delta / (Max + Min);
        if (Max == r)
            res.h = (g - b) / Delta + (g < b ? 6.0f : 0.0f);
        else if(Max == g)
            res.h = (b - r) / Delta + 2.0f;
        else
            res.h = (r - g) / Delta + 4.0f;

        res.h /= 6.0f;
    }
    res.h *= 100.0f;
    res.s *= 100.0f;
    res.l *= 100.0f;
    return res;
}

rgb32 hsl_to_rgb(hsl *px)
{
    rgb32 res;
    float h = px->h / 100.0f;
    double s = px->s / 100.0f;
    float l = px->l / 100.0f;

    if (s == 0.0f)
    {
        res.r = res.g = res.b = (uint8_t)(l * 255.0f);
    }
    else
    {
        float j = (l < 0.5f) ? (l * (1.0f + l)) : ((l + s) - (s * l));
        float i = 2.0f * l - j;

        res.r = __hsl_to_rgb_helper(i, j, h + (1.0f / 3.0f));
        res.g = __hsl_to_rgb_helper(i, j, h);
        res.b = __hsl_to_rgb_helper(i, j, h - (1.0f / 3.0f));
    }
    return res;
}

lab xyz_to_lab(xyz *px)
{
    lab res;
    float x = px->x / 95.047f;
    float y = px->y / 100.000f;
    float z = px->z / 108.883f;

    x = (x > 0.008856f) ? pow(x, (1.0f / 3.0f)) : (x * 7.787f) + (16.0f / 116.0f);
    y = (y > 0.008856f) ? pow(y, (1.0f / 3.0f)) : (y * 7.787f) + (16.0f / 116.0f);
    z = (z > 0.008856f) ? pow(z, (1.0f / 3.0f)) : (z * 7.787f) + (16.0f / 116.0f);
    res.l = ((y * 116.0f) - 16.0f);
    res.a = ((x - y) * 500);
    res.b = ((y - z) * 200);
    return res;
}

xyz lab_to_xyz(lab *px)
{
    xyz res;
    float y = (px->l + 16.0f) / 116.0f;
    float x = ((px->a / 500.0f) + y);
    float z = (y - (px->b / 200.0f));

    y = (pow(y, 3) > 0.008856f) ? pow(y, 3) : ((y - (16.0f / 116.0f)) / 7.787f);
    x = (pow(x, 3) > 0.008856f) ? pow(x, 3) : ((x - (16.0f / 116.0f)) / 7.787f);
    z = (pow(z, 3) > 0.008856f) ? pow(z, 3) : ((z - (16.0f / 116.0f)) / 7.787f);
    res.x = (x * 95.047f);
    res.y = (y * 100.000f);
    res.z = (z * 108.883f);
    return res;
}
