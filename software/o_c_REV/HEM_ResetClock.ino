// Copyright (c) 2020, Peter Kyme
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#define RC_MIN_SPACING 6
#define RC_TICKS_PER_MS 17

class ResetClock : public HemisphereApplet {
public:

    const char* applet_name() {
        return "ResetClock";
    }

    void Start() {
        cursor = 0;
        spacing = 6;
        ticks_since_clock = 0;
        pending_clocks = 0;
        length = 8;
        position = 0;
        offset = 0;
    }

    void Controller() {
        if (Clock(1)) {
            pending_clocks += (length - position + offset) % length;
            ClockOut(1);
        }

        if (Clock(0)) pending_clocks++;

        if (pending_clocks && (ticks_since_clock > spacing * RC_TICKS_PER_MS)) {
            ClockOut(0);
            ticks_since_clock = 0;
            position = (position + 1) % length;
            pending_clocks--;
        } else {
            ticks_since_clock++;
        } 
    }

    void View() {
        gfxHeader(applet_name());
        DrawInterface();
        DrawIndicator();
    }

    void OnButtonPress() {
        if (++cursor > 3) cursor = 0;
        ResetCursor();
    }

    void OnEncoderMove(int direction) {
        if (cursor == 0) { // length
            length = constrain(length + direction, 1, 32);
            position %= length;
            offset %= length;
        } else if (cursor == 1) { // offset
            int prevOffset = offset;
            offset = constrain(offset + direction, 0, length-1);
            pending_clocks += offset - prevOffset;
            if (pending_clocks < 0) pending_clocks += length;
        } else if (cursor == 2) { // spacing
            spacing = constrain(spacing + direction, RC_MIN_SPACING, 100);
        } else if (cursor == 3) { // position
            position = ( position + direction + length ) % length;
        }
    }
        
    uint32_t OnDataRequest() {
        uint32_t data = 0;
        Pack(data, PackLocation {0,5}, length-1);
        Pack(data, PackLocation {5,5}, offset);
        Pack(data, PackLocation {10,7}, spacing);
        return data;
    }

    void OnDataReceive(uint32_t data) {
        length = Unpack(data, PackLocation {0,5}) + 1;
        offset = Unpack(data, PackLocation {5,5});
        spacing = Unpack(data, PackLocation {10,7});
    }

protected:
    void SetHelp() {
        //                               "------------------" <-- Size Guide
        help[HEMISPHERE_HELP_DIGITALS] = "Clock, Reset";
        help[HEMISPHERE_HELP_CVS]      = "";
        help[HEMISPHERE_HELP_OUTS]     = "Clock, Reset";
        help[HEMISPHERE_HELP_ENCODER]  = "Len/Offst/Spac/Pos";
        //                               "------------------" <-- Size Guide
    }
    
private:
    int cursor;
    int ticks_since_clock;
    int pending_clocks;
    
    // Settings
    int length;
    int position;
    int offset;
    int spacing;

    void DrawInterface() {
        // Length
        gfxIcon(1, 14, LOOP_ICON);
        gfxPrint(12 + pad(10, length), 15, length);

        // Offset
        gfxBitmap(32, 15, 8, ROTATE_R_ICON);
        gfxPrint(40 + pad(10, offset), 15, offset);
    
        // Spacing
        gfxBitmap(1, 25, 8, CLOCK_ICON);
        gfxPrint(12 + pad(10, spacing), 25, spacing);
        gfxPrint("ms");
    
        if (cursor == 0) gfxCursor(13, 23, 12); // length
        if (cursor == 1) gfxCursor(41, 23, 12); // offset
        if (cursor == 2) gfxCursor(13, 33, 12); // spacing
        if (cursor == 3) gfxCursor(1, 62, 62); // position
    }

    void DrawIndicator() {
        gfxLine(0, 45, 63, 45);
        gfxLine(0, 60, 63, 60);
        for(int i = 0; i < length; i++)
        {
            int x0 = (i*64)/length;
            int width = ((i+1)*64)/length - x0;
            if (position == i)
            {
                gfxRect(x0, 45, width, 15);
            }
            else
            {
                gfxLine(x0, 45, x0, 60);
            }  
        }
        gfxLine(63, 45, 63, 60);
    }

};


////////////////////////////////////////////////////////////////////////////////
//// Hemisphere Applet Functions
///
///  Once you run the find-and-replace to make these refer to ResetClock,
///  it's usually not necessary to do anything with these functions. You
///  should prefer to handle things in the HemisphereApplet child class
///  above.
////////////////////////////////////////////////////////////////////////////////
ResetClock ResetClock_instance[2];

void ResetClock_Start(bool hemisphere) {ResetClock_instance[hemisphere].BaseStart(hemisphere);}
void ResetClock_Controller(bool hemisphere, bool forwarding) {ResetClock_instance[hemisphere].BaseController(forwarding);}
void ResetClock_View(bool hemisphere) {ResetClock_instance[hemisphere].BaseView();}
void ResetClock_OnButtonPress(bool hemisphere) {ResetClock_instance[hemisphere].OnButtonPress();}
void ResetClock_OnEncoderMove(bool hemisphere, int direction) {ResetClock_instance[hemisphere].OnEncoderMove(direction);}
void ResetClock_ToggleHelpScreen(bool hemisphere) {ResetClock_instance[hemisphere].HelpScreen();}
uint32_t ResetClock_OnDataRequest(bool hemisphere) {return ResetClock_instance[hemisphere].OnDataRequest();}
void ResetClock_OnDataReceive(bool hemisphere, uint32_t data) {ResetClock_instance[hemisphere].OnDataReceive(data);}
