#define RAYGUI_IMPLEMENTATION

#include "config.h"
#if PBOX_DEBUG
//#define RAYGUI_DEBUG_TEXT_BOUNDS
#endif

#include "raygui.h"
#include "dark.h"
#include "bluish.h"

// Tab Bar control. Original source from RAYGUI.H
// Modified for my purposes
// NOTE: Using GuiToggle() for the TABS
int GuiTabBarEx(Rectangle bounds, int width, int closeable, const char **text, int count, int *active) {

    int closing = -1;
    Rectangle tabBounds = { bounds.x, bounds.y, width, bounds.height };

    if (*active < 0) *active = 0;
    else if (*active > count - 1) *active = count - 1;

    int offsetX = 0;    // Required in case tabs go out of bounds
    offsetX = (*active + 2)*width - bounds.width;
    if (offsetX < 0) offsetX = 0;

    // Draw control
    //--------------------------------------------------------------------
    for (int i = 0; i < count; i++)
    {
        tabBounds.x = bounds.x + (width + 4)*i - offsetX;

				// this is better than it was :/
        if (tabBounds.x < bounds.x + bounds.width && tabBounds.x > bounds.x - tabBounds.width)
        {
            // Draw tabs as toggle controls
            int textAlignment = GuiGetStyle(TOGGLE, TEXT_ALIGNMENT);
            int textPadding = GuiGetStyle(TOGGLE, TEXT_PADDING);
            GuiSetStyle(TOGGLE, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
            GuiSetStyle(TOGGLE, TEXT_PADDING, 8);
            if (i == *active) GuiToggle(tabBounds, text[i], true);
            else if (GuiToggle(tabBounds, text[i], false) == true) *active = i;
            GuiSetStyle(TOGGLE, TEXT_PADDING, textPadding);
            GuiSetStyle(TOGGLE, TEXT_ALIGNMENT, textAlignment);

            // Draw tab close button
            // NOTE: Only draw close button for curren tab: if (CheckCollisionPointRec(mousePoint, tabBounds))
						if (i == *active && closeable) {
            	int tempBorderWidth = GuiGetStyle(BUTTON, BORDER_WIDTH);
            	int tempTextAlignment = GuiGetStyle(BUTTON, TEXT_ALIGNMENT);
            	GuiSetStyle(BUTTON, BORDER_WIDTH, 1);
							GuiSetStyle(BUTTON, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
            	if (GuiButton((Rectangle){tabBounds.x + tabBounds.width - 14 - 5,
								tabBounds.y + 5, 14, 14 }, GuiIconText(ICON_CROSS_SMALL, NULL))) closing = i;
           		GuiSetStyle(BUTTON, BORDER_WIDTH, tempBorderWidth);
            	GuiSetStyle(BUTTON, TEXT_ALIGNMENT, tempTextAlignment);
						}
        }
    }

    // Draw tab-bar bottom line
    GuiDrawRectangle((Rectangle){ bounds.x, bounds.y + bounds.height - 1, bounds.width, 1 }, 0, BLANK, GetColor(GuiGetStyle(TOGGLE, BORDER_COLOR_NORMAL)));
    //--------------------------------------------------------------------

    return closing;     // Return closing tab requested
}

int current_theme = 0;

int GuiLoadTheme() {
	UnloadFont(GuiGetFont());
	GuiLoadStyleDefault();
	if (!current_theme) GuiLoadStyleDark();
	else {
		GuiLoadStyleBluish();
	}
}

int GuiToggleTheme() {
	current_theme = !current_theme;				
	GuiLoadTheme();
};
