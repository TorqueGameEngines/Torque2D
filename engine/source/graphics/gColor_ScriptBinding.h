//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

/*! @defgroup ColorFunctions Color
	@ingroup TorqueScriptFunctions
	@{
*/

/*! Gets a count of available stock colors.
    @return A count of available stock colors.
*/
ConsoleFunctionWithDocs( getStockColorCount, ConsoleInt, 1, 1, ())
{
    return StockColor::getCount();
}

//-----------------------------------------------------------------------------

/*! Gets the stock color name at the specified index.
    @param stockColorIndex The zero-based index of the stock color name to retrieve.
    @return The stock color name at the specified index or nothing if the string is invalid.
*/
ConsoleFunctionWithDocs( getStockColorName, ConsoleString, 2, 2, (stockColorIndex))
{
    // Fetch stock color index.
    const S32 stockColorIndex = dAtoi(argv[1]);

    // Fetch the color item.
    const StockColorItem* pColorItem = StockColor::getColorItem( stockColorIndex );

    return pColorItem == NULL ? NULL : pColorItem->getColorName();
}

//-----------------------------------------------------------------------------

/*! Gets whether the specified name is a stock color or not.
    @param stockColorName - The stock color name to test for.
    @return Whether the specified name is a stock color or not.
*/
ConsoleFunctionWithDocs( isStockColor, ConsoleBool, 2, 2, (stockColorName))
{
    // Fetch stock color name.
    const char* pStockColorName = argv[1];

    // Return whether this is a stock color name or not.
    return StockColor::isColor( pStockColorName );
}

//-----------------------------------------------------------------------------

/*! Gets a floating-point-based stock color by name.
    @param stockColorName - The stock color name to retrieve.
    @return The stock color that matches the specified color name.  Returns nothing if the color name is not found.
*/
ConsoleFunctionWithDocs( getStockColorF, ConsoleString, 2, 2, (stockColorName))
{
    // Fetch stock color name.
    const char* pStockColorName = argv[1];

    // Return nothing if stock color name is invalid.
    if ( !StockColor::isColor( pStockColorName ) )
        return StringTable->EmptyString;

    // Fetch stock color.
    const ColorF& color = StockColor::colorF( pStockColorName );

    // Format stock color.
    char* returnBuffer = Con::getReturnBuffer(256);
    dSprintf(returnBuffer, 256, "%g %g %g %g", color.red, color.green, color.blue, color.alpha);
    return(returnBuffer);
}

//-----------------------------------------------------------------------------

/*! Gets a byte-based stock color by name.
    @param stockColorName - The stock color name to retrieve.
    @return The stock color that matches the specified color name.  Returns nothing if the color name is not found.
*/
ConsoleFunctionWithDocs( getStockColorI, ConsoleString, 2, 2, (stockColorName))
{
    // Fetch stock color name.
    const char* pStockColorName = argv[1];

    // Return nothing if stock color name is invalid.
    if ( !StockColor::isColor( pStockColorName ) )
        return StringTable->EmptyString;

    // Fetch stock color.
    const ColorI& color = StockColor::colorI( pStockColorName );

    // Format stock color.
    char* returnBuffer = Con::getReturnBuffer(256);
    dSprintf(returnBuffer, 256, "%d %d %d %d", color.red, color.green, color.blue, color.alpha);
    return(returnBuffer);
}

/*! @} */ // group ColorFunctions
