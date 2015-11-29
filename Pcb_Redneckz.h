// sensor and lcd
OneWire ds(12);
#if Dallas == true
  DallasTemperature sensors(&ds);
#endif

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

// push buttons
const char Button_up    = A3;
const char Button_dn    = A2;
const char Button_start = A1;
const char Button_enter = A0;

// outputs
const byte Pump = 8;
const byte Buzz = 13;

// thermal control
const byte THERMAL_CONTROL_DOWN = 9;
const byte THERMAL_CONTROL_UP   = 10;
const byte THERMAL_CONTROL_SCR  = 11;


