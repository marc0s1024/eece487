#define TEST_TX_PIN 17  // GPIO17 (TX2)
#define TEST_RX_PIN 16  // GPIO16 (RX2)

HardwareSerial TestSerial(2);  // UART2

void setup() {
    Serial.begin(115200);
    TestSerial.begin(9600, SERIAL_8N1, TEST_RX_PIN, TEST_TX_PIN);
}

void loop() {
    Serial.println("\nReading...");
    delay(500);
  /*
    TestSerial.println("Hello There");  // send over TX2 (GPIO17)
    delay(500);  // transmission delay
  */
    // check if data is available on RX2 (GPIO16)
    if (TestSerial.available()) {
        uint8_t receivedData[13];
        TestSerial.readBytes(receivedData, 13);
        //String receivedData = TestSerial.readStringUntil('\n');
        Serial.print("Received on RX2: ");
        for (int i = 0; i < 13; i++) {
          Serial.printf("0x%02X ", receivedData[i]);
        }
    }

    delay(1000);
}
