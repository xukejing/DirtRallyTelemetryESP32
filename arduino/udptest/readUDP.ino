union charFloat {
  char str[264];
  float value[66];
};

void readMyUDP(void) {
  int packetSize = udp.parsePacket();
  if (packetSize == 264) {
    union charFloat bufFloat;
    udp.read(bufFloat.str, 264);
    // Serial.print("v=");
    // Serial.print(bufFloat.value[7] * 3.6);
    // Serial.print("  ");
    // Serial.print("gear=");
    // Serial.print(bufFloat.value[33]);
    // Serial.print("  ");
    // Serial.print("RPM=");
    // Serial.print(bufFloat.value[37] * 10.0);
    // Serial.println(";");
    float rpmMin=178;
    float rpm2 = (bufFloat.value[37] - rpmMin)/ (bufFloat.value[63]- rpmMin);
    myleds(rpm2);
    // Serial.print("RPM:");
    // Serial.print(bufFloat.value[37]);
    // Serial.print("  ");
    // Serial.print(bufFloat.value[63]);
    // Serial.println("");
  }
}