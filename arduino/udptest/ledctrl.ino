void myleds(float RPM) {
  int midled = int(NUM_LEDS / 2);
  int bright = int(midled * 128 * RPM);
  int i = 0;
  for (i = 0; i < midled; i++) {
    if (bright > (i * 128))
      hsvV[i] = 128;
    else if ((bright > ((i - 1) * 128)))
      hsvV[i] = bright - ((i - 1) * 128);
    else
      hsvV[i] = 0;
  }
  for (i = midled; i < NUM_LEDS; i++) {
    if (bright > ((NUM_LEDS - i - 1) * 128))
      hsvV[i] = 128;
    else if ((bright > ((NUM_LEDS - i - 2) * 128)))
      hsvV[i] = bright - ((NUM_LEDS - i - 2) * 128);
    else
      hsvV[i] = 0;
  }
  if (RPM > 0.95) {
    blink1 = !blink1;
  } else
    blink1 = false;
  if (blink1) {
    for (i = 0; i < NUM_LEDS; i++) {
      leds[i].setHSV(255, 255, hsvV[i] * 1.5);
    }
  } else {
    for (i = 0; i < NUM_LEDS; i++) {
      leds[i].setHSV(hsvH[i], 255, hsvV[i]);
    }
  }

  FastLED.show();
}
