//////////////////////////////////////////////////////////////////////////////////
//                                                                              //
// StyleAsCode exporter v1.2 - Style data exported as a values array            //
//                                                                              //
// USAGE: On init call: GuiLoadStyleBluish();                             //
//                                                                              //
// more info and bugs-report:  github.com/raysan5/raygui                        //
// feedback and support:       ray[at]raylibtech.com                            //
//                                                                              //
// Copyright (c) 2020-2023 raylib technologies (@raylibtech)                    //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////

#define BLUISH_STYLE_PROPS_COUNT  16

// Custom style name: bluish
static const GuiStyleProp bluishStyleProps[BLUISH_STYLE_PROPS_COUNT] = {
    { 0, 0, 0x5ca6a6ff },    // DEFAULT_BORDER_COLOR_NORMAL 
    { 0, 1, 0xb4e8f3ff },    // DEFAULT_BASE_COLOR_NORMAL 
    { 0, 2, 0x447e77ff },    // DEFAULT_TEXT_COLOR_NORMAL 
    { 0, 3, 0x5f8792ff },    // DEFAULT_BORDER_COLOR_FOCUSED 
    { 0, 4, 0xcdeff7ff },    // DEFAULT_BASE_COLOR_FOCUSED 
    { 0, 5, 0x4c6c74ff },    // DEFAULT_TEXT_COLOR_FOCUSED 
    { 0, 6, 0x3b5b5fff },    // DEFAULT_BORDER_COLOR_PRESSED 
    { 0, 7, 0xeaffffff },    // DEFAULT_BASE_COLOR_PRESSED 
    { 0, 8, 0x275057ff },    // DEFAULT_TEXT_COLOR_PRESSED 
    { 0, 9, 0x96aaacff },    // DEFAULT_BORDER_COLOR_DISABLED 
    { 0, 10, 0xc8d7d9ff },    // DEFAULT_BASE_COLOR_DISABLED 
    { 0, 11, 0x8c9c9eff },    // DEFAULT_TEXT_COLOR_DISABLED 
    { 0, 18, 0x84adb7ff },    // DEFAULT_LINE_COLOR 
    { 0, 19, 0xe8eef1ff },    // DEFAULT_BACKGROUND_COLOR 
		{ 0, 16, 0x00000010 },    // DEFAULT_TEXT_SIZE 
    { 0, 17, 0x00000000 },    // DEFAULT_TEXT_SPACING 
};

#define BLUISH_COMPRESSED_DATA_SIZE DARK_COMPRESSED_DATA_SIZE
#define bluishFontData darkFontData
#define bluishFontRecs darkFontRecs
#define bluishFontChars darkFontChars

// Style loading function: bluish
static void GuiLoadStyleBluish(void)
{
    // Load style properties provided
    // NOTE: Default properties are propagated
    for (int i = 0; i < BLUISH_STYLE_PROPS_COUNT; i++)
    {
        GuiSetStyle(bluishStyleProps[i].controlId, bluishStyleProps[i].propertyId, bluishStyleProps[i].propertyValue);
    }

    // Custom font loading
    // NOTE: Compressed font image data (DEFLATE), it requires DecompressData() function
    int bluishFontDataSize = 0;
    unsigned char *data = DecompressData(bluishFontData, BLUISH_COMPRESSED_DATA_SIZE, &bluishFontDataSize);
    Image imFont = { data, 256, 256, 1, 2 };

    Font font = { 0 };
    font.baseSize = 16;
    font.glyphCount = 95;

    // Load texture from image
    font.texture = LoadTextureFromImage(imFont);
    UnloadImage(imFont);  // Uncompressed data can be unloaded from memory

    // Copy char recs data from global fontRecs
    // NOTE: Required to avoid issues if trying to free font
    font.recs = (Rectangle *)malloc(font.glyphCount*sizeof(Rectangle));
    memcpy(font.recs, bluishFontRecs, font.glyphCount*sizeof(Rectangle));

    // Copy font char info data from global fontChars
    // NOTE: Required to avoid issues if trying to free font
    font.glyphs = (GlyphInfo *)malloc(font.glyphCount*sizeof(GlyphInfo));
    memcpy(font.glyphs, bluishFontChars, font.glyphCount*sizeof(GlyphInfo));

    GuiSetFont(font);

    // Setup a white rectangle on the font to be used on shapes drawing,
    // this way we make sure all gui can be drawn on a single pass because no texture change is required
    // NOTE: Setting up this rectangle is a manual process (for the moment)
    Rectangle whiteChar ={ 124, 6, 1, 1 };
    SetShapesTexture(font.texture, whiteChar);
	
    //-----------------------------------------------------------------

    // TODO: Custom user style setup: Set specific properties here (if required)
    // i.e. Controls specific BORDER_WIDTH, TEXT_PADDING, TEXT_ALIGNMENT
}
