#ifndef __finder_h_
#define __finder_h_

#include <stdint.h>
#include "bitmap.h"
#include "color.h"

typedef struct CTSInfo_t
{
    int16_t CTSNum, tol;
    float hueMod, satMod;
    bitmap *targetImage;

    bool (*ctsFuncPtr)(void *this_ptr, rgb32 *first, rgb32 *second);

} CTSInfo;

typedef struct Point_t
{
    int32_t x;
    int32_t y;
} Point;

typedef struct PointArray_t
{
    Point* p;
    size_t size;
} PointArray;

typedef struct Finder_t
{
    CTSInfo info;
} Finder;


/** @brief Initialises a point array. All values are set to default. No memory is allocated by this function.
 *
 * @param pa PointArray* Pointer to the PointArray structure to be initialised.
 * @return void
 *
 */
extern void initPointArray(PointArray* pa);


/** @brief Frees a PointArray and nullifies all data-members.
 *
 * @param pa PointArray* Pointer to the structure to be freed. free() is called on the data-member p.
 * @return extern void
 *
 */
extern void freePointArray(PointArray* pa);


/** @brief Initialises all members of a CTSInfo structure to their default values.
 *
 * @param info CTSInfo* A pointer to the CTSInfo structure to be set to default.
 * @return void
 *
 */
extern void defaultCTS(CTSInfo *info);


/** @brief Sets the CTSInfo structure's comparison function pointer.
 *
 * @param info CTSInfo* A pointer to the CTSInfo structure whose pointer to set.
 * @param CTSNum int16_t A CTS value from -1 to 3 inclusive.
 * @return void
 *
 */
extern void setCTS(CTSInfo *info, int16_t CTSNum);


/** @brief Compares two pixels for similarity.
 *
 * @param info CTSInfo* A pointer to the CTSInfo structure whose target and comparator functions to use.
 * @param first rgb32* A pointer to an RGB structure representation of the first colour to be compared.
 * @param second rgb32* A pointer to an RGB structure representation of the second colour to be compared.
 * @param tolerance uint16_t Tolerance threshold defining how strict the comparison will be.
 * @return bool Returns true if the two colours are similar within the specified tolerance. False otherwise.
 *
 */
extern bool similarColours(CTSInfo *info, rgb32 *first, rgb32 *second, uint16_t tolerance);


/** @brief Counts the colours within a specified area without a tolerance threshold.
 *
 * @param info CTSInfo* A pointer to the CTSInfo structure whose target and comparator functions to use.
 * @param colour rgb32* A pointer to an RGB structure representing the colour to count.
 * @param x1 int32_t The x-coordinate of the upper-left corner of the area to search.
 * @param y1 int32_t The y-coordinate of the upper-left corner of the area to search.
 * @param x2 int32_t The x-coordinate of the lower-right corner of the area to search.
 * @param y2 int32_t The y-coordinate of the lower-right corner of the area to search.
 * @return uint32_t Returns the amount of colours found within the specified area. Zero otherwise.
 *
 */
extern uint32_t countColour(CTSInfo *info, rgb32 *colour, int32_t x1, int32_t y1, int32_t x2, int32_t y2);


/** @brief Counts the colour within a specified area with a tolerance threshold.
 *
 * @param info CTSInfo* A pointer to the CTSInfo structure whose target and comparator functions to use.
 * @param colour rgb32* A pointer to an RGB structure representing the colour to count.
 * @param x1 int32_t The x-coordinate of the upper-left corner of the area to search.
 * @param y1 int32_t The y-coordinate of the upper-left corner of the area to search.
 * @param x2 int32_t The x-coordinate of the lower-right corner of the area to search.
 * @param y2 int32_t The y-coordinate of the lower-right corner of the area to search.
 * @param tolerance uint16_t Tolerance threshold defining how strict the comparison will be.
 * @return uint32_t Returns the amount of colours found within the specified area. Zero otherwise.
 *
 */
extern uint32_t countColourTolerance(CTSInfo *info, rgb32 *colour, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t tolerance);


/** @brief Finds a colour within a specified area without a tolerance threshold.
 *
 * @param info CTSInfo* A pointer to the CTSInfo structure whose target and comparator functions to use.
 * @param x int32_t* A pointer to an integer that will contain the x-coordinate of the first colour found.
 * @param y int32_t* A pointer to an integer that will contain the y-coordinate of the first colour found.
 * @param colour rgb32* A pointer to an RGB structure representing the colour to find.
 * @param x1 int32_t The x-coordinate of the upper-left corner of the area to search.
 * @param y1 int32_t The y-coordinate of the upper-left corner of the area to search.
 * @param x2 int32_t The x-coordinate of the lower-right corner of the area to search.
 * @param y2 int32_t The y-coordinate of the lower-right corner of the area to search.
 * @return bool Returns true if the colour is found; false otherwise.
 *
 */
extern bool findColour(CTSInfo *info, int32_t *x, int32_t *y, rgb32* colour, int32_t x1, int32_t y1, int32_t x2, int32_t y2);


/** @brief Finds a colour within a specified area with a tolerance threshold.
 *
 * @param info CTSInfo* A pointer to the CTSInfo structure whose target and comparator functions to use.
 * @param x int32_t* A pointer to an integer that will contain the x-coordinate of the first colour found.
 * @param y int32_t* A pointer to an integer that will contain the y-coordinate of the first colour found.
 * @param colour rgb32* A pointer to an RGB structure representing the colour to find.
 * @param x1 int32_t The x-coordinate of the upper-left corner of the area to search.
 * @param y1 int32_t The y-coordinate of the upper-left corner of the area to search.
 * @param x2 int32_t The x-coordinate of the lower-right corner of the area to search.
 * @param y2 int32_t The y-coordinate of the lower-right corner of the area to search.
 * @param tolerance uint16_t Tolerance threshold defining how strict the comparison will be.
 * @return bool Returns true if the colour is found within the specified area and lies within the tolerance threshold; false otherwise.
 *
 */
extern bool findColourTolerance(CTSInfo *info, int32_t *x, int32_t *y, rgb32* colour, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t tolerance);


/** @brief Finds a colour within a specified area without a tolerance threshold.
 *
 * @param info CTSInfo* A pointer to the CTSInfo structure whose target and comparator functions to use.
 * @param points PointArray* A pointer to a PointArray structure that will contain an array of points specifying the location of each colour found.
 *                           This structure's members must be set to default using initPointArray().
 *                           This structure must be freed using freePointArray().
 * @param colour rgb32* A pointer to an RGB structure representing the colour to find.
 * @param x1 int32_t The x-coordinate of the upper-left corner of the area to search.
 * @param y1 int32_t The y-coordinate of the upper-left corner of the area to search.
 * @param x2 int32_t The x-coordinate of the lower-right corner of the area to search.
 * @param y2 int32_t The y-coordinate of the lower-right corner of the area to search.
 * @return bool Returns true if the colour is found within the specified area; false otherwise.
 *
 */
extern bool findColours(CTSInfo *info, PointArray *points, rgb32* colour, int32_t x1, int32_t y1, int32_t x2, int32_t y2);


/** @brief Finds a colour within a specified area with a tolerance threshold.
 *
 * @param info CTSInfo* A pointer to the CTSInfo structure whose target and comparator functions to use.
 * @param points PointArray* A pointer to a PointArray structure that will contain an array of points specifying the location of each colour found.
 *                           This structure's members must be set to default using initPointArray().
 *                           This structure must be freed using freePointArray().
 * @param colour rgb32* A pointer to an RGB structure representing the colour to find.
 * @param x1 int32_t The x-coordinate of the upper-left corner of the area to search.
 * @param y1 int32_t The y-coordinate of the upper-left corner of the area to search.
 * @param x2 int32_t The x-coordinate of the lower-right corner of the area to search.
 * @param y2 int32_t The y-coordinate of the lower-right corner of the area to search.
 * @param tolerance uint16_t Tolerance threshold defining how strict the comparison will be.
 * @return bool Returns true if the colour is found within the specified area and lies within the tolerance threshold; false otherwise.
 *
 */
extern bool findColoursTolerance(CTSInfo *info, PointArray *points, rgb32* colour, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t tolerance);


/** @brief Finds an image within the target area without a tolerance threshold.
 *
 * @param info CTSInfo* A pointer to the CTSInfo structure whose target and comparator functions to use.
 * @param imageToFind bitmap* A pointer to a bitmap structure representing the image to search the target area for.
 * @param x int32_t* A pointer to an integer that will contain the x-coordinate of the upper-left coordinate of the image found.
 * @param y int32_t* A pointer to an integer that will contain the y-coordinate of the upper-left coordinate of the image found.
 * @return bool Returns true if the image is found within the target area; false otherwise.
 *
 */
extern bool findImage(CTSInfo *info, bitmap* imageToFind, int32_t *x, int32_t *y);


/** @brief Finds an image within a specified area without a tolerance threshold.
 *
 * @param info CTSInfo* A pointer to the CTSInfo structure whose target and comparator functions to use.
 * @param imageToFind bitmap* A pointer to a bitmap structure representing the image to search the target area for.
 * @param x int32_t* A pointer to an integer that will contain the x-coordinate of the upper-left coordinate of the image found.
 * @param y int32_t* A pointer to an integer that will contain the y-coordinate of the upper-left coordinate of the image found.
 * @param x1 int32_t The x-coordinate of the upper-left corner of the area to search.
 * @param y1 int32_t The y-coordinate of the upper-left corner of the area to search.
 * @param x2 int32_t The x-coordinate of the lower-right corner of the area to search.
 * @param y2 int32_t The y-coordinate of the lower-right corner of the area to search.
 * @return bool Returns true if the image is found within the specified area; false otherwise.
 *
 */
extern bool findImageIn(CTSInfo *info, bitmap* imageToFind, int32_t *x, int32_t *y, int32_t x1, int32_t y1, int32_t x2, int32_t y2);


/** @brief Finds an image within a specified area with a tolerance threshold.
 *
 * @param info CTSInfo* A pointer to the CTSInfo structure whose target and comparator functions to use.
 * @param imageToFind bitmap* A pointer to a bitmap structure representing the image to search the target area for.
 * @param x int32_t* A pointer to an integer that will contain the x-coordinate of the upper-left coordinate of the image found.
 * @param y int32_t* A pointer to an integer that will contain the y-coordinate of the upper-left coordinate of the image found.
 * @param x1 int32_t The x-coordinate of the upper-left corner of the area to search.
 * @param y1 int32_t The y-coordinate of the upper-left corner of the area to search.
 * @param x2 int32_t The x-coordinate of the lower-right corner of the area to search.
 * @param y2 int32_t The y-coordinate of the lower-right corner of the area to search.
 * @param tolerance uint16_t Tolerance threshold defining how strict the comparison will be.
 * @return bool Returns true if the image is found within the specified area and lies within the specified threshold; false otherwise.
 *
 */
extern bool findImageToleranceIn(CTSInfo *info, bitmap* imageToFind, int32_t *x, int32_t *y, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t tolerance);

#endif // __finder_h_
