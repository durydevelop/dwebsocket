/*
#include <raylib.h>

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>
*/
#include <websocket>
#include <dlog.h>
#include "drawer.cpp"
#include "DTimer.h"

#define CONNECT_TIMEOUT 4000

dws::DWebSocket webSocket;
std::string url("ws://localhost:8088/");

// #define nowMillis() std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()

int main()
{
    int screenWidth = 800;
    int screenHeight = 450;
    float value = 0.5f;
    bool sliderEditMode = false;
    bool vSliderEditMode = false;
    bool vSliderBarEditMode = false;
    char strConn[20];
    dws::DWebSocketReadyState connStatus=dws::DWebSocketReadyState::STATE_CLOSED;
    std::vector<std::string> connStatusStr = {"STATE_CONNECTING", "STATE_OPEN", "STATE_CLOSING", "STATE_CLOSED", "STATE_TIMEOUT" };

    // Create window
    InitWindow(screenWidth, screenHeight, "Goldlake");
    SetTargetFPS(60);

    // Connect to server
	webSocket.setUrl(url);
    dLog(DLOG_DEBUG, "Start connecting to %s",url.c_str());
    connStatus=dws::DWebSocketReadyState::STATE_CONNECTING;
	if (!webSocket.connect()) {
        dLog(DLOG_ERROR, "Error on start connection");
    }

    strcpy(strConn,"Connecting");
    
    #ifdef EMSCRIPTEN
        DTimer::OneShot(CONNECT_TIMEOUT,[&connStatus] {
            if (connStatus != dws::DWebSocketReadyState::STATE_OPEN) {
                dLog(DLOG_ERROR,"...connection timeout");
                connStatus=dws::DWebSocketReadyState::STATE_TIMEOUT;
            }
        });
    #endif

	// Setup a callback to be fired when a message or an event (open, close, error) is received
	webSocket.setOnEventCallback([&connStatus](dws::DWebSocketEventType type, uint8_t *data, size_t len) {
        if (type == dws::DWebSocketEventType::EVENT_OPEN) {
            dLog(DLOG_DEBUG,"WebSocket Opened");
            connStatus=dws::DWebSocketReadyState::STATE_OPEN;
        }
        else if (type == dws::DWebSocketEventType::EVENT_CLOSE) {
            dLog(DLOG_DEBUG,"WebSocket Close");
            connStatus=dws::DWebSocketReadyState::STATE_CLOSED;
        }
        else if (type == dws::DWebSocketEventType::EVENT_ERROR) {
            dLog(DLOG_DEBUG,"WebSocket Error: %s",data);
            connStatus=dws::DWebSocketReadyState::STATE_CLOSED;
        }
        else if (type == dws::DWebSocketEventType::EVENT_DATA_STR)	{
            dLog(DLOG_DEBUG,"Data string: %s",data);
        }
        else if (type == dws::DWebSocketEventType::EVENT_DATA_BIN)	{
            dLog(DLOG_DEBUG,"Data binary:");
            for(int i = 0; i < len; ++i) {
                char* dd=(char*)data;
                char d=dd[i];
                dLog(DLOG_DEBUG,"Data binary:  %02X",d);
            }
            
        }
    });

    // Main loop (detect window close button or ESC key)
    while (!WindowShouldClose()) {
        //--------------------------- Update -----------------------------------------------
        float lastValue=value;
        //----------------------------------------------------------------------------------

        //--------------------------- Draw -------------------------------------------------
        BeginDrawing();
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

        if (vSliderEditMode || vSliderBarEditMode) GuiLock();
        else GuiUnlock();

        
        GuiGroupBox((Rectangle){ 66, 24, 276, 312 }, "STANDARD");
        char s[] = "Connecting";
        strcpy(s,connStatusStr[connStatus].c_str());
        /*
        if (connStatus == dws::DWebSocketReadyState::STATE_OPEN) {
            strcpy(s,"Connected");
        }
        else if (dws::DWebSocketReadyState::) {
            strcpy(s,"Timneout");
        }
        */
        GuiTextBox((Rectangle){ 66, 24, 276, 312 },s,strlen(s),false);
        /*
        value = GuiSlider((Rectangle){ 96, 48, 216, 16 }, TextFormat("%0.2f", value), NULL, value, 0.0f, 1.0f);
        value = GuiVerticalSlider((Rectangle){ 120, 120, 24, 192 }, TextFormat("%0.2f", value), NULL, value, 0.0f, 1.0f);
        value = GuiVerticalSliderBar((Rectangle){ 264, 120, 24, 192 }, TextFormat("%0.2f", value), NULL, value, 0.0f, 1.0f);
        */
        
        GuiGroupBox((Rectangle){ 378, 24, 276, 312 }, "OWNING");
        if (GuiSliderOwning((Rectangle){ 408, 48, 216, 16 }, NULL, TextFormat("%0.2f", value), &value, 0.0f, 1.0f, sliderEditMode)) sliderEditMode = !sliderEditMode;
        if (GuiVerticalSliderOwning((Rectangle){ 432, 120, 24, 192 }, NULL, TextFormat("%0.2f", value), &value, 0.0f, 1.0f, vSliderEditMode)) vSliderEditMode = !vSliderEditMode;
        if (GuiVerticalSliderBarOwning((Rectangle){ 576, 120, 24, 192 }, NULL, TextFormat("%0.2f", value), &value, 0.0f, 1.0f, vSliderBarEditMode)) vSliderBarEditMode = !vSliderBarEditMode;
        
        EndDrawing();
        //----------------------------------------------------------------------------------

        if (value != lastValue) {
            // Value changed
            dLog(DLOG_DEBUG, "%0.2f",value);
            std::string s=std::to_string(value);
            webSocket.send(s);
            if (!webSocket.send((uint8_t*)s.c_str(),s.size())) {
                dLog(DLOG_ERROR, "WebSocket send error");
            }
        }
    }

    // Close window and OpenGL context
    CloseWindow();

    return 0;
}
/*
float GuiVerticalSliderPro(Rectangle bounds, const char *textTop, const char *textBottom, float value, float minValue, float maxValue, int sliderHeight)
{
    GuiState state = (GuiState)GuiGetState();

    int sliderValue = (int)(((value - minValue)/(maxValue - minValue)) * (bounds.height - 2 * GuiGetStyle(SLIDER, BORDER_WIDTH)));

    Rectangle slider = {
        bounds.x + GuiGetStyle(SLIDER, BORDER_WIDTH) + GuiGetStyle(SLIDER, SLIDER_PADDING),
        bounds.y + bounds.height - sliderValue,
        bounds.width - 2*GuiGetStyle(SLIDER, BORDER_WIDTH) - 2*GuiGetStyle(SLIDER, SLIDER_PADDING),
        0.0f,
    };

    if (sliderHeight > 0)        // Slider
    {
        slider.y -= sliderHeight/2;
        slider.height = (float)sliderHeight;
    }
    else if (sliderHeight == 0)  // SliderBar
    {
        slider.y -= GuiGetStyle(SLIDER, BORDER_WIDTH);
        slider.height = (float)sliderValue;
    }
    // Update control
    //--------------------------------------------------------------------
    if ((state != STATE_DISABLED) && !guiLocked)
    {
        Vector2 mousePoint = GetMousePosition();

        if (CheckCollisionPointRec(mousePoint, bounds))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            {
                state = STATE_PRESSED;

                // Get equivalent value and slider position from mousePoint.x
                float normalizedValue = (bounds.y + bounds.height - mousePoint.y - (float)(sliderHeight / 2)) / (bounds.height - (float)sliderHeight);
                value = (maxValue - minValue) * normalizedValue + minValue;

                if (sliderHeight > 0) slider.y = mousePoint.y - slider.height / 2;  // Slider
                else if (sliderHeight == 0)                                          // SliderBar
                {
                    slider.y = mousePoint.y;
                    slider.height = bounds.y + bounds.height - slider.y - GuiGetStyle(SLIDER, BORDER_WIDTH);
                }
            }
            else state = STATE_FOCUSED;
        }

        if (value > maxValue) value = maxValue;
        else if (value < minValue) value = minValue;
    }


    // Bar limits check
    if (sliderHeight > 0)        // Slider
    {
        if (slider.y < (bounds.y + GuiGetStyle(SLIDER, BORDER_WIDTH))) slider.y = bounds.y + GuiGetStyle(SLIDER, BORDER_WIDTH);
        else if ((slider.y + slider.height) >= (bounds.y + bounds.height)) slider.y = bounds.y + bounds.height - slider.height - GuiGetStyle(SLIDER, BORDER_WIDTH);
    }
    else if (sliderHeight == 0)  // SliderBar
    {
        if (slider.y < (bounds.y + GuiGetStyle(SLIDER, BORDER_WIDTH)))
        {
            slider.y = bounds.y + GuiGetStyle(SLIDER, BORDER_WIDTH);
            slider.height = bounds.height - 2*GuiGetStyle(SLIDER, BORDER_WIDTH);
        }
    }

    //--------------------------------------------------------------------
    // Draw control
    //--------------------------------------------------------------------
    GuiDrawRectangle(bounds, GuiGetStyle(SLIDER, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(SLIDER, BORDER + (state*3))), guiAlpha), Fade(GetColor(GuiGetStyle(SLIDER, (state != STATE_DISABLED)?  BASE_COLOR_NORMAL : BASE_COLOR_DISABLED)), guiAlpha));

    // Draw slider internal bar (depends on state)
    if ((state == STATE_NORMAL) || (state == STATE_PRESSED)) GuiDrawRectangle(slider, 0, BLANK, Fade(GetColor(GuiGetStyle(SLIDER, BASE_COLOR_PRESSED)), guiAlpha));
    else if (state == STATE_FOCUSED) GuiDrawRectangle(slider, 0, BLANK, Fade(GetColor(GuiGetStyle(SLIDER, TEXT_COLOR_FOCUSED)), guiAlpha));

    // Draw top/bottom text if provided
    if (textTop != NULL)
    {
        Rectangle textBounds = { 0 };
        textBounds.width = (float)GetTextWidth(textTop);
        textBounds.height = (float)GuiGetStyle(DEFAULT, TEXT_SIZE);
        textBounds.x = bounds.x + bounds.width/2 - textBounds.width/2;
        textBounds.y = bounds.y - textBounds.height - GuiGetStyle(SLIDER, TEXT_PADDING);

        GuiDrawText(textTop, textBounds, TEXT_ALIGN_RIGHT, Fade(GetColor(GuiGetStyle(SLIDER, TEXT + (state*3))), guiAlpha));
    }

    if (textBottom != NULL)
    {
        Rectangle textBounds = { 0 };
        textBounds.width = (float)GetTextWidth(textBottom);
        textBounds.height = (float)GuiGetStyle(DEFAULT, TEXT_SIZE);
        textBounds.x = bounds.x + bounds.width/2 - textBounds.width/2;
        textBounds.y = bounds.y + bounds.height + GuiGetStyle(SLIDER, TEXT_PADDING);

        GuiDrawText(textBottom, textBounds, TEXT_ALIGN_LEFT, Fade(GetColor(GuiGetStyle(SLIDER, TEXT + (state*3))), guiAlpha));
    }
    //--------------------------------------------------------------------

    return value;
}

float GuiVerticalSlider(Rectangle bounds, const char *textTop, const char *textBottom, float value, float minValue, float maxValue)
{
    return GuiVerticalSliderPro(bounds, textTop, textBottom, value, minValue, maxValue, GuiGetStyle(SLIDER, SLIDER_WIDTH));
}

float GuiVerticalSliderBar(Rectangle bounds, const char *textTop, const char *textBottom, float value, float minValue, float maxValue)
{
    return GuiVerticalSliderPro(bounds, textTop, textBottom, value, minValue, maxValue, 0);
}

bool GuiSliderProOwning(Rectangle bounds, const char *textLeft, const char *textRight, float *value, float minValue, float maxValue, int sliderWidth, bool editMode)
{
    GuiState state = (GuiState)GuiGetState();

    float tempValue = *value;
    bool pressed = false;

    int sliderValue = (int)(((tempValue - minValue)/(maxValue - minValue))*(bounds.width - 2*GuiGetStyle(SLIDER, BORDER_WIDTH)));

    Rectangle slider = {
        bounds.x,
        bounds.y + GuiGetStyle(SLIDER, BORDER_WIDTH) + GuiGetStyle(SLIDER, SLIDER_PADDING),
        0,
        bounds.height - 2*GuiGetStyle(SLIDER, BORDER_WIDTH) - 2*GuiGetStyle(SLIDER, SLIDER_PADDING)
    };

    if (sliderWidth > 0)        // Slider
    {
        slider.x += (sliderValue - sliderWidth/2);
        slider.width = (float)sliderWidth;
    }
    else if (sliderWidth == 0)  // SliderBar
    {
        slider.x += GuiGetStyle(SLIDER, BORDER_WIDTH);
        slider.width = (float)sliderValue;
    }

    // Update control
    //--------------------------------------------------------------------
    if ((state != STATE_DISABLED) && (editMode || !guiLocked))
    {
        Vector2 mousePoint = GetMousePosition();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            if (CheckCollisionPointRec(mousePoint, bounds))
            {
                pressed = true;
            }
        }
        else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && editMode)
        {
            pressed = true;
        }
        if (editMode)
        {
            state = STATE_PRESSED;
            tempValue = ((maxValue - minValue)*(mousePoint.x - (float)(bounds.x + sliderWidth/2)))/(float)(bounds.width - sliderWidth) + minValue;

            if (sliderWidth > 0) slider.x = mousePoint.x - slider.width/2;  // Slider
            else if (sliderWidth == 0) slider.width = (float)sliderValue;          // SliderBar

        }
        else if (CheckCollisionPointRec(mousePoint, bounds))
        {
            state = STATE_FOCUSED;
        }

        if (tempValue > maxValue) tempValue = maxValue;
        else if (tempValue < minValue) tempValue = minValue;
    }


    // Bar limits check
    if (sliderWidth > 0)        // Slider
    {
        if (slider.x <= (bounds.x + GuiGetStyle(SLIDER, BORDER_WIDTH))) slider.x = bounds.x + GuiGetStyle(SLIDER, BORDER_WIDTH);
        else if ((slider.x + slider.width) >= (bounds.x + bounds.width)) slider.x = bounds.x + bounds.width - slider.width - GuiGetStyle(SLIDER, BORDER_WIDTH);
    }
    else if (sliderWidth == 0)  // SliderBar
    {
        if (slider.width > bounds.width) slider.width = bounds.width - 2*GuiGetStyle(SLIDER, BORDER_WIDTH);
    }

    //--------------------------------------------------------------------
    // Draw control
    //--------------------------------------------------------------------
    GuiDrawRectangle(bounds, GuiGetStyle(SLIDER, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(SLIDER, BORDER + (state*3))), guiAlpha), Fade(GetColor(GuiGetStyle(SLIDER, (state != STATE_DISABLED)?  BASE_COLOR_NORMAL : BASE_COLOR_DISABLED)), guiAlpha));

    // Draw slider internal bar (depends on state)
    if ((state == STATE_NORMAL) || (state == STATE_PRESSED))
        GuiDrawRectangle(slider, 0, BLANK, Fade(GetColor(GuiGetStyle(SLIDER, BASE_COLOR_PRESSED)), guiAlpha));
    else if (state == STATE_FOCUSED)
        GuiDrawRectangle(slider, 0, BLANK, Fade(GetColor(GuiGetStyle(SLIDER, TEXT_COLOR_FOCUSED)), guiAlpha));

    // Draw left/right text if provided
    if (textLeft != NULL)
    {
        Rectangle textBounds = { 0 };
        textBounds.width = (float)GetTextWidth(textLeft);
        textBounds.height = (float)GuiGetStyle(DEFAULT, TEXT_SIZE);
        textBounds.x = bounds.x - textBounds.width - GuiGetStyle(SLIDER, TEXT_PADDING);
        textBounds.y = bounds.y + bounds.height/2 - GuiGetStyle(DEFAULT, TEXT_SIZE)/2;

        GuiDrawText(textLeft, textBounds, TEXT_ALIGN_RIGHT, Fade(GetColor(GuiGetStyle(SLIDER, TEXT + (state*3))), guiAlpha));
    }

    if (textRight != NULL)
    {
        Rectangle textBounds = { 0 };
        textBounds.width = (float)GetTextWidth(textRight);
        textBounds.height = (float)GuiGetStyle(DEFAULT, TEXT_SIZE);
        textBounds.x = bounds.x + bounds.width + GuiGetStyle(SLIDER, TEXT_PADDING);
        textBounds.y = bounds.y + bounds.height/2 - GuiGetStyle(DEFAULT, TEXT_SIZE)/2;

        GuiDrawText(textRight, textBounds, TEXT_ALIGN_LEFT, Fade(GetColor(GuiGetStyle(SLIDER, TEXT + (state*3))), guiAlpha));
    }
    //--------------------------------------------------------------------

    *value = tempValue;
    return pressed;
}

bool GuiSliderOwning(Rectangle bounds, const char *textLeft, const char *textRight, float *value, float minValue, float maxValue, bool editMode)
{
    return GuiSliderProOwning(bounds, textLeft, textRight, value, minValue, maxValue, GuiGetStyle(SLIDER, SLIDER_WIDTH), editMode);
}

bool GuiSliderBarOwning(Rectangle bounds, const char *textLeft, const char *textRight, float *value, float minValue, float maxValue, bool editMode)
{
    return GuiSliderProOwning(bounds, textLeft, textRight, value, minValue, maxValue, 0, editMode);
}

bool GuiVerticalSliderProOwning(Rectangle bounds, const char *textTop, const char *textBottom, float *value, float minValue, float maxValue, int sliderHeight, bool editMode)
{
    GuiState state = (GuiState)GuiGetState();

    float tempValue = *value;
    bool pressed = false;

    int sliderValue = (int)(((tempValue - minValue)/(maxValue - minValue)) * (bounds.height - 2 * GuiGetStyle(SLIDER, BORDER_WIDTH)));

    Rectangle slider = {
        bounds.x + GuiGetStyle(SLIDER, BORDER_WIDTH) + GuiGetStyle(SLIDER, SLIDER_PADDING),
        bounds.y + bounds.height - sliderValue,
        bounds.width - 2*GuiGetStyle(SLIDER, BORDER_WIDTH) - 2*GuiGetStyle(SLIDER, SLIDER_PADDING),
        0.0f,
    };

    if (sliderHeight > 0)        // Slider
    {
        slider.y -= sliderHeight/2;
        slider.height = (float)sliderHeight;
    }
    else if (sliderHeight == 0)  // SliderBar
    {
        slider.y -= GuiGetStyle(SLIDER, BORDER_WIDTH);
        slider.height = (float)sliderValue;
    }
    // Update control
    //--------------------------------------------------------------------
    if ((state != STATE_DISABLED) && (editMode || !guiLocked))
    {
        Vector2 mousePoint = GetMousePosition();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            if (CheckCollisionPointRec(mousePoint, bounds))
            {
                pressed = true;
            }
        }
        else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && editMode)
        {
            pressed = true;
        }
        if (editMode)
        {
            state = STATE_PRESSED;

            float normalizedValue = (bounds.y + bounds.height - mousePoint.y - (float)(sliderHeight / 2)) / (bounds.height - (float)sliderHeight);
            tempValue = (maxValue - minValue) * normalizedValue + minValue;

            if (sliderHeight > 0) slider.y = mousePoint.y - slider.height / 2;  // Slider
            else if (sliderHeight == 0)                                          // SliderBar
            {
                slider.y = mousePoint.y;
                slider.height = bounds.y + bounds.height - slider.y - GuiGetStyle(SLIDER, BORDER_WIDTH);
            }
        }
        else if (CheckCollisionPointRec(mousePoint, bounds))
        {
            state = STATE_FOCUSED;
        }

        if (tempValue > maxValue) tempValue = maxValue;
        else if (tempValue < minValue) tempValue = minValue;
    }


    // Bar limits check
    if (sliderHeight > 0)        // Slider
    {
        if (slider.y < (bounds.y + GuiGetStyle(SLIDER, BORDER_WIDTH))) slider.y = bounds.y + GuiGetStyle(SLIDER, BORDER_WIDTH);
        else if ((slider.y + slider.height) >= (bounds.y + bounds.height)) slider.y = bounds.y + bounds.height - slider.height - GuiGetStyle(SLIDER, BORDER_WIDTH);
    }
    else if (sliderHeight == 0)  // SliderBar
    {
        if (slider.y < (bounds.y + GuiGetStyle(SLIDER, BORDER_WIDTH)))
        {
            slider.y = bounds.y + GuiGetStyle(SLIDER, BORDER_WIDTH);
            slider.height = bounds.height - 2*GuiGetStyle(SLIDER, BORDER_WIDTH);
        }
    }

    //--------------------------------------------------------------------
    // Draw control
    //--------------------------------------------------------------------
    GuiDrawRectangle(bounds, GuiGetStyle(SLIDER, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(SLIDER, BORDER + (state*3))), guiAlpha), Fade(GetColor(GuiGetStyle(SLIDER, (state != STATE_DISABLED)?  BASE_COLOR_NORMAL : BASE_COLOR_DISABLED)), guiAlpha));

    // Draw slider internal bar (depends on state)
    if ((state == STATE_NORMAL) || (state == STATE_PRESSED))
        GuiDrawRectangle(slider, 0, BLANK, Fade(GetColor(GuiGetStyle(SLIDER, BASE_COLOR_PRESSED)), guiAlpha));
    else if (state == STATE_FOCUSED)
        GuiDrawRectangle(slider, 0, BLANK, Fade(GetColor(GuiGetStyle(SLIDER, TEXT_COLOR_FOCUSED)), guiAlpha));

    // Draw top/bottom text if provided
    if (textTop != NULL)
    {
        Rectangle textBounds = { 0 };
        textBounds.width = (float)GetTextWidth(textTop);
        textBounds.height = (float)GuiGetStyle(DEFAULT, TEXT_SIZE);
        textBounds.x = bounds.x + bounds.width/2 - textBounds.width/2;
        textBounds.y = bounds.y - textBounds.height - GuiGetStyle(SLIDER, TEXT_PADDING);

        GuiDrawText(textTop, textBounds, TEXT_ALIGN_RIGHT, Fade(GetColor(GuiGetStyle(SLIDER, TEXT + (state*3))), guiAlpha));
    }

    if (textBottom != NULL)
    {
        Rectangle textBounds = { 0 };
        textBounds.width = (float)GetTextWidth(textBottom);
        textBounds.height = (float)GuiGetStyle(DEFAULT, TEXT_SIZE);
        textBounds.x = bounds.x + bounds.width/2 - textBounds.width/2;
        textBounds.y = bounds.y + bounds.height + GuiGetStyle(SLIDER, TEXT_PADDING);

        GuiDrawText(textBottom, textBounds, TEXT_ALIGN_LEFT, Fade(GetColor(GuiGetStyle(SLIDER, TEXT + (state*3))), guiAlpha));
    }
    //--------------------------------------------------------------------

    *value = tempValue;
    return pressed;
}

bool GuiVerticalSliderOwning(Rectangle bounds, const char *textTop, const char *textBottom, float *value, float minValue, float maxValue, bool editMode)
{
    return GuiVerticalSliderProOwning(bounds, textTop, textBottom, value, minValue, maxValue, GuiGetStyle(SLIDER, SLIDER_WIDTH), editMode);
}

bool GuiVerticalSliderBarOwning(Rectangle bounds, const char *textTop, const char *textBottom, float *value, float minValue, float maxValue, bool editMode)
{
    return GuiVerticalSliderProOwning(bounds, textTop, textBottom, value, minValue, maxValue, 0, editMode);
}
*/