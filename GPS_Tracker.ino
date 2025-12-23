#include <Wire.h>
#include <U8g2lib.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <math.h>

/* ========= OLED ========= */
U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

/* ========= GPS ========= */
SoftwareSerial gpsSerial(3, 4); // RX, TX
TinyGPSPlus gps;

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);   // لو مش شغال جرّب 38400
  Wire.begin();

  u8g2.begin();
  u8g2.setFont(u8g2_font_6x10_tr);
}

void loop() {

  /* ===== Read GPS ===== */
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  u8g2.firstPage();
  do {
    u8g2.clearBuffer();

    /* ===== GPS VALID ===== */
    if (gps.location.isValid() && gps.course.isValid() && gps.speed.kmph() > 1.0) {

      float angle = gps.course.deg();      // اتجاه الحركة
      float rad   = angle * M_PI / 180.0;

      /* --- Draw Arrow --- */
      int cx = 64, cy = 38, len = 22;
      int x2 = cx + len * cos(rad);
      int y2 = cy - len * sin(rad);

      u8g2.drawLine(cx, cy, x2, y2);
      u8g2.drawCircle(cx, cy, 2);

      /* --- Coordinates --- */
      char lat[15], lon[15];
      dtostrf(gps.location.lat(), 8, 5, lat);
      dtostrf(gps.location.lng(), 8, 5, lon);

      u8g2.drawStr(0, 10, "LAT:");
      u8g2.drawStr(30, 10, lat);

      u8g2.drawStr(0, 22, "LON:");
      u8g2.drawStr(30, 22, lon);

    } else {
      u8g2.drawStr(10, 25, "MOVE TO UPDATE");
      u8g2.drawStr(18, 40, "WAIT GPS FIX");
    }

    /* ===== Satellites ===== */
    char sats[5];
    itoa(gps.satellites.value(), sats, 10);
    u8g2.drawStr(0, 60, "SAT:");
    u8g2.drawStr(30, 60, sats);

  } while (u8g2.nextPage());

  /* ===== Serial Debug ===== */
  Serial.print("LAT: ");
  Serial.print(gps.location.lat(), 6);
  Serial.print("  LON: ");
  Serial.print(gps.location.lng(), 6);
  Serial.print("  SPEED: ");
  Serial.print(gps.speed.kmph());
  Serial.print("  COURSE: ");
  Serial.print(gps.course.deg());
  Serial.print("  SAT: ");
  Serial.println(gps.satellites.value());
}
