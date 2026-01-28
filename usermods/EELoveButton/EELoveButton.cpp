#include "wled.h"
#include <Adafruit_MCP23X08.h>
/*
 * EE-LoveButton usermod
 *
 * Based on the upstream v2 usermod example.
 * This is a ready-to-customize template specifically for the EE-LoveButton project.
 *
 * To customize behaviour, edit the methods of the EELoveButtonUsermod class below
 * (primarily setup(), loop(), handleButton(), and the JSON/config helpers).
 */

// class name. Use something descriptive and leave the ": public Usermod" part :)
class EELoveButtonUsermod : public Usermod {

  private:

    Adafruit_MCP23X08 mcp;
    bool mcpConnected = false;

    // Private class members. You can declare variables and functions only accessible to your usermod here

    bool buttonState = true;    
    int pressCount = 0;


    uint16_t pressesToLove = 10;
    uint16_t lovePreset = 10;


    // strings that are used multiple time (this will save some flash memory)
    static const char _name[];
    static const char _enabled[];


  public:

    // non WLED related methods, may be used for data exchange between usermods (non-inline methods should be defined out of class)

    // methods called by WLED (can be inlined as they are called only once but if you call them explicitly define them out of class)

    /*
     * setup() is called once at boot. WiFi is not yet connected at this point.
     * readFromConfig() is called prior to setup()
     * You can use it to initialize variables, sensors or similar.
     */
    void setup() override {
      mcpConnected = mcp.begin_I2C();
        if (mcpConnected) {
            Serial.println("Connected to MCP Switch Interface");
        } else {
            Serial.println("Not Connected to MCP Switch Interface");
        }
    }




    /*
     * loop() is called continuously. Here you can check for events, read sensors, etc.
     *
     * Tips:
     * 1. You can use "if (WLED_CONNECTED)" to check for a successful network connection.
     *    Additionally, "if (WLED_MQTT_CONNECTED)" is available to check for a connection to an MQTT broker.
     *
     * 2. Try to avoid using the delay() function. NEVER use delays longer than 10 milliseconds.
     *    Instead, use a timer check as shown here.
     */
    void loop() override {
      // if usermod is disabled or called during strip updating just exit
      // NOTE: on very long strips strip.isUpdating() may always return true so update accordingly
      if (strip.isUpdating()) return;

      if (!mcpConnected) return;


       if (buttonState != mcp.digitalRead(0)) {
          buttonState = mcp.digitalRead(0);
          if (buttonState == HIGH) {
              pressCount++;
              Serial.print("Button pressed ");
              Serial.print(pressCount);
              Serial.println(" times");
              if (pressCount >= pressesToLove) {
                  Serial.println("Entering love mode!");
                  applyPreset(lovePreset);
                  pressCount = 0;
              }
          }
       }
    }


    /*
     * addToConfig() can be used to add custom persistent settings to the cfg.json file in the "um" (usermod) object.
     * It will be called by WLED when settings are actually saved (for example, LED settings are saved)
     */
    void addToConfig(JsonObject& root) override
    {
      JsonObject top = root.createNestedObject(FPSTR(_name));

      // EE-LoveButton settings
      top[F("Number of button presses to trigger love")] = pressesToLove;
      top[F("Love preset number")] = lovePreset;
    }


    /*
     * readFromConfig() can be used to read back the custom settings you added with addToConfig().
     * This is called by WLED when settings are loaded (currently this only happens immediately after boot, or after saving on the Usermod Settings page)
     */
    bool readFromConfig(JsonObject& root) override
    {
      JsonObject top = root[FPSTR(_name)];

      bool configComplete = !top.isNull();

      // A 3-argument getJsonValue() assigns the 3rd argument as a default value if the Json value is missing
      configComplete &= getJsonValue(top[F("Number of button presses to trigger love")], pressesToLove, 10);
      configComplete &= getJsonValue(top[F("Love preset number")], lovePreset, 10);

      return configComplete;
    }


    /*
     * getId() allows you to optionally give your V2 usermod an unique ID (defined in const.h).
     * For this custom project usermod we use the generic "unspecified" ID.
     */
    uint16_t getId() override
    {
      return USERMOD_ID_UNSPECIFIED;
    }

};


// add more strings here to reduce flash memory usage
const char EELoveButtonUsermod::_name[]    PROGMEM = "EE-LoveButton";


static EELoveButtonUsermod ee_lovebutton_usermod;
REGISTER_USERMOD(ee_lovebutton_usermod);

