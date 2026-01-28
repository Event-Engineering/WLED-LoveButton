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
    bool enabled = false;
    bool initDone = false;
    unsigned long lastTime = 0;

    // EE-LoveButton configuration (shown on Usermod Settings page)
    // Number of button presses required to enter "love" mode
    uint16_t pressesToLove = 10;
    // How long love mode should last (milliseconds)
    uint16_t loveTimeoutMs = 2500;
    // WLED preset to activate while in love mode
    uint16_t lovePreset = 10;
    // WLED preset to restore when leaving love mode
    uint16_t normalPreset = 1;

    // strings that are used multiple time (this will save some flash memory)
    static const char _name[];
    static const char _enabled[];


  public:

    // non WLED related methods, may be used for data exchange between usermods (non-inline methods should be defined out of class)

    /**
     * Enable/Disable the usermod
     */
    inline void enable(bool enable) { enabled = enable; }

    /**
     * Get usermod enabled/disabled state
     */
    inline bool isEnabled() { return enabled; }


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
        // serializeConfig(); // slow but forces a sync with the settings system

      initDone = true;
    }


    /*
     * connected() is called every time the WiFi is (re)connected
     * Use it to initialize network interfaces
     */
    void connected() override {
      // Optional: react to WiFi connectivity here.
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
      if (!enabled || strip.isUpdating()) return;

      // Example heartbeat; replace with actual LoveButton behaviour
      if (millis() - lastTime > 1000) {
        lastTime = millis();
      }
    }


    /*
     * addToJsonInfo() can be used to add custom entries to the /json/info part of the JSON API.
     * Creating an "u" object allows you to add custom key/value pairs to the Info section of the WLED web UI.
     */
    void addToJsonInfo(JsonObject& root) override
    {
      // if "u" object does not exist yet we need to create it
      JsonObject user = root["u"];
      if (user.isNull()) user = root.createNestedObject("u");

      // Example: publish LoveButton related info here if desired
      // JsonArray arr = user.createNestedArray(FPSTR(_name));
      // arr.add(1);          // value
      // arr.add(F(" unit")); // unit
    }


    /*
     * addToJsonState() can be used to add custom entries to the /json/state part of the JSON API (state object).
     * Values in the state object may be modified by connected clients
     */
    void addToJsonState(JsonObject& root) override
    {
      if (!initDone || !enabled) return;  // prevent crash on boot applyPreset()

      JsonObject usermod = root[FPSTR(_name)];
      if (usermod.isNull()) usermod = root.createNestedObject(FPSTR(_name));

      //usermod["user0"] = userVar0;
    }


    /*
     * readFromJsonState() can be used to receive data clients send to the /json/state part of the JSON API (state object).
     * Values in the state object may be modified by connected clients
     */
    void readFromJsonState(JsonObject& root) override
    {
      if (!initDone) return;  // prevent crash on boot applyPreset()

      JsonObject usermod = root[FPSTR(_name)];
      if (!usermod.isNull()) {
        // Handle incoming /json/state updates for EE-LoveButton here if needed
      }
    }


    /*
     * addToConfig() can be used to add custom persistent settings to the cfg.json file in the "um" (usermod) object.
     * It will be called by WLED when settings are actually saved (for example, LED settings are saved)
     */
    void addToConfig(JsonObject& root) override
    {
      JsonObject top = root.createNestedObject(FPSTR(_name));
      top[FPSTR(_enabled)] = enabled;

      // EE-LoveButton settings
      top[F("Number of button presses to trigger love")] = pressesToLove;
      top[F("Love timeout")] = loveTimeoutMs;
      top[F("Love preset number")] = lovePreset;
      top[F("Normal preset number")] = normalPreset;
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
      configComplete &= getJsonValue(top[F("Love timeout")], loveTimeoutMs, 2500);
      configComplete &= getJsonValue(top[F("Love preset number")], lovePreset, 10);
      configComplete &= getJsonValue(top[F("Normal preset number")], normalPreset, 1);

      return configComplete;
    }


    /*
     * appendConfigData() is called when user enters usermod settings page
     * it may add additional metadata for certain entry fields (adding drop down is possible)
     * be careful not to add too much as oappend() buffer is limited to 3k
     */
    void appendConfigData() override
    {
      // Optional helper text for settings page
      oappend(F("addInfo('")); oappend(String(FPSTR(_name)).c_str()); oappend(F(":Love timeout")); oappend(F("',1,'milliseconds to stay in love before timing out');"));
    }


    /*
     * handleOverlayDraw() is called just before every show() (LED strip update frame) after effects have set the colors.
     * Use this to blank out some LEDs or set them to a different color regardless of the set effect mode.
     */
    void handleOverlayDraw() override
    {
      // Example: blank out the first pixel
      // strip.setPixelColor(0, RGBW32(0,0,0,0));
    }


    /**
     * handleButton() can be used to override default button behaviour. Returning true
     * will prevent button working in a default way.
     */
    bool handleButton(uint8_t b) override {
      yield();
      // ignore certain button types as they may have other consequences
      if (!enabled
       || buttons[b].type == BTN_TYPE_NONE
       || buttons[b].type == BTN_TYPE_RESERVED
       || buttons[b].type == BTN_TYPE_PIR_SENSOR
       || buttons[b].type == BTN_TYPE_ANALOG
       || buttons[b].type == BTN_TYPE_ANALOG_INVERTED) {
        return false;
      }

      bool handled = false;
      // TODO: implement EE-LoveButton specific button handling here
      return handled;
    }


    /**
     * onStateChange() is used to detect WLED state change
     * @mode parameter is CALL_MODE_... parameter used for notifications
     */
    void onStateChange(uint8_t mode) override {
      // do something if WLED state changed (color, brightness, effect, preset, etc)
    }


    /*
     * getId() allows you to optionally give your V2 usermod an unique ID (defined in const.h).
     * For this custom project usermod we use the generic "unspecified" ID.
     */
    uint16_t getId() override
    {
      return USERMOD_ID_UNSPECIFIED;
    }

   //More methods can be added in the future, this example will then be extended.
   //Your usermod will remain compatible as it does not need to implement all methods from the Usermod base class!
};


// add more strings here to reduce flash memory usage
const char EELoveButtonUsermod::_name[]    PROGMEM = "EE-LoveButton";
const char EELoveButtonUsermod::_enabled[] PROGMEM = "enabled";


static EELoveButtonUsermod ee_lovebutton_usermod;
REGISTER_USERMOD(ee_lovebutton_usermod);

