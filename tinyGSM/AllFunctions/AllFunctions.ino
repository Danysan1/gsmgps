#define TINY_GSM_MODEM_SIM808
#define SerialMon Serial
#define SerialAT Serial3
#define TINY_GSM_DEBUG SerialMon

#define GSM_AUTOBAUD_MIN 9600
#define GSM_AUTOBAUD_MAX 38400

/*
 * Test enabled
 */
#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_CALL true
#define TINY_GSM_USE_SMS true
#define TINY_GSM_USE_USSD true
// powerdown modem after tests
#define TINY_GSM_POWERDOWN false

#define DUMP_AT_COMMANDS

// set GSM PIN, if any
#define GSM_PIN "7975"

// Set phone numbers, if you want to test SMS and Calls
#define SMS_TARGET  "+393471840366"
#define CALL_TARGET "+393471840366"
//#define CALL_TARGET "+390542616529"

// Your GPRS credentials
// Leave empty, if missing user or pass
const char apn[]  = "wap.postemobile.it";
const char user[] = "";
const char pass[] = "";

#include <TinyGsmClient.h>

#ifdef DUMP_AT_COMMANDS
  #include <StreamDebugger.h>
  StreamDebugger debugger(SerialAT, SerialMon);
  TinyGsm modem(debugger);
#else
  TinyGsm modem(SerialAT);
#endif

void setup() {
  // Set console baud rate
  SerialMon.begin(115200);
  delay(10);

  // Set your reset, enable, power pins here

  delay(3000);

  // Set GSM module baud rate
  TinyGsmAutoBaud(SerialAT,GSM_AUTOBAUD_MIN,GSM_AUTOBAUD_MAX);
}

void loop() {

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  DBG("Initializing modem...");
  if (!modem.restart()) {
    DBG("Failed to restart modem, delaying 10s and retrying");
    delay(3000);
    // restart autobaud in case GSM just rebooted
    TinyGsmAutoBaud(SerialAT,GSM_AUTOBAUD_MIN,GSM_AUTOBAUD_MAX);
    delay(10000);
    return;
  }

  String modemInfo = modem.getModemInfo();
  DBG("Modem:", modemInfo);

  // Unlock your SIM card with a PIN if needed
  if ( GSM_PIN && modem.getSimStatus() != 3 ) {
    modem.simUnlock(GSM_PIN);
  }

  DBG("Waiting for network...");
  if (!modem.waitForNetwork()) {
    delay(10000);
    return;
  }

  if (modem.isNetworkConnected()) {
    DBG("Network connected");
  }

#if TINY_GSM_USE_GPRS
  DBG("Connecting to", apn);
  if (!modem.gprsConnect(apn, user, pass)) {
    delay(10000);
    return;
  }

  bool res = modem.isGprsConnected();
  DBG("GPRS status:", res ? "connected" : "not connected");

  String ccid = modem.getSimCCID();
  DBG("CCID:", ccid);

  String imei = modem.getIMEI();
  DBG("IMEI:", imei);

  String cop = modem.getOperator();
  DBG("Operator:", cop);

  IPAddress local = modem.localIP();
  DBG("Local IP:", local);

  int csq = modem.getSignalQuality();
  DBG("Signal quality:", csq);

  // This is NOT supported on M590
  int battLevel = modem.getBattPercent();
  DBG("Battery lavel:", battLevel);

  // This is only supported on SIMxxx series
  float battVoltage = modem.getBattVoltage() / 1000.0F;
  DBG("Battery voltage:", battVoltage);

  // This is only supported on SIMxxx series
  String gsmLoc = modem.getGsmLocation();
  DBG("GSM location:", gsmLoc);

  // This is only supported on SIMxxx series
  // String gsmTime = modem.getGSMDateTime(DATE_TIME);
  // DBG("GSM Time:", gsmTime);
  // String gsmDate = modem.getGSMDateTime(DATE_DATE);
  // DBG("GSM Date:", gsmDate);

  String ussd_balance = modem.sendUSSD("*111#");
  DBG("Balance (USSD):", ussd_balance);

  String ussd_phone_num = modem.sendUSSD("*161#");
  DBG("Phone number (USSD):", ussd_phone_num);
#endif

#if defined(TINY_GSM_MODEM_HAS_GPS)
  modem.enableGPS();
  String gps_raw = modem.getGPSraw();
  modem.disableGPS();
  DBG("GPS raw data:", gps_raw);
#endif

#if TINY_GSM_USE_SMS && defined(SMS_TARGET)
  res = modem.sendSMS(SMS_TARGET, String("Hello from ") + imei);
  DBG("SMS:", res ? "OK" : "fail");

  // This is only supported on SIMxxx series
  res = modem.sendSMS_UTF16(SMS_TARGET, u"Привіііт!", 9);
  DBG("UTF16 SMS:", res ? "OK" : "fail");
#endif

#if TINY_GSM_USE_CALL && defined(CALL_TARGET)
  DBG("Calling:", CALL_TARGET);

  // This is NOT supported on M590
  res = modem.callNumber(CALL_TARGET);
  DBG("Call:", res ? "OK" : "fail");

  if (res) {
    delay(1000L);

    // Play DTMF A, duration 1000ms
    modem.dtmfSend('A', 1000);

    // Play DTMF 0..4, default duration (100ms)
    for (char tone='0'; tone<='4'; tone++) {
      modem.dtmfSend(tone);
    }

    delay(5000);

    res = modem.callHangup();
    DBG("Hang up:", res ? "OK" : "fail");
  }
#endif

#if TINY_GSM_USE_GPRS
  modem.gprsDisconnect();
  if (!modem.isGprsConnected()) {
    DBG("GPRS disconnected");
  } else {
    DBG("GPRS disconnect: Failed.");
  }
#endif

#if TINY_GSM_POWERDOWN
  // Try to power-off (modem may decide to restart automatically)
  // To turn off modem completely, please use Reset/Enable pins
  modem.poweroff();
  DBG("Poweroff.");
#endif

  // Do nothing forevermore
  while (true) {
    modem.maintain();
  }
}
