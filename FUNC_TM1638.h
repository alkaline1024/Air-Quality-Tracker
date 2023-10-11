extern const int strobe;
extern const int data;
extern const int clock;

void sendCommand(uint8_t value)
{
  digitalWrite(strobe, LOW);
  shiftOut(data, clock, LSBFIRST, value);
  digitalWrite(strobe, HIGH);
}

void reset()
{
  sendCommand(0x40); // set auto increment mode
  digitalWrite(strobe, LOW);
  shiftOut(data, clock, LSBFIRST, 0xc0); // set starting address to 0
  for (uint8_t i = 0; i < 16; i++)
  {
    shiftOut(data, clock, LSBFIRST, 0x00);
  }
  digitalWrite(strobe, HIGH);
}

uint8_t readTM1638_Buttons(void)
{
  uint8_t buttons = 0;
  digitalWrite(strobe, LOW);
  shiftOut(data, clock, LSBFIRST, 0x42);

  pinMode(data, INPUT);

  for (uint8_t i = 0; i < 4; i++)
  {
    uint8_t v = shiftIn(data, clock, LSBFIRST) << i;
    buttons |= v;
  }

  pinMode(data, OUTPUT);
  digitalWrite(strobe, HIGH);
  return buttons;
}

void display7Segment(int position, int segmentData)
{
  digitalWrite(strobe, LOW);
  shiftOut(data, clock, LSBFIRST, position);
  shiftOut(data, clock, LSBFIRST, segmentData);
  digitalWrite(strobe, HIGH);
}

/*0*/ /*1*/ /*2*/ /*3*/ /*4*/ /*5*/ /*6*/ /*7*/ /*8*/ /*9*/
uint8_t digits[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};
void displayPMSMode(int mode)
{
  char mode_display1, mode_display2;
  switch (mode)
  {
  case 100:
    mode_display1 = digits[0] + 128;
    mode_display2 = digits[0];
    break;
  case 25:
    mode_display1 = digits[2] + 128;
    mode_display2 = digits[5];
    break;
  case 10:
    mode_display1 = digits[1] + 128;
    mode_display2 = digits[0];
    break;
  }
  sendCommand(0x44);                 // set single address
  display7Segment(0xc0, 0b01110011); // character to be displayed ='P'
  if (mode == 100)
  {
    display7Segment(0xc2, digits[1]);
  }
  else
  {
    display7Segment(0xc2, 0x00);
  }
  display7Segment(0xc4, mode_display1);
  display7Segment(0xc6, mode_display2);
}

void displayPMSValue(int value)
{
  display7Segment(0xce, digits[value % 10]);
  if (value > 999)
    display7Segment(0xc8, digits[value % 10000 / 1000]);
  else
    display7Segment(0xc8, 0x00);
  if (value > 99)
    display7Segment(0xca, digits[value % 1000 / 100]);
  else
    display7Segment(0xca, 0x00);
  if (value > 9)
    display7Segment(0xcc, digits[value % 100 / 10]);
  else
    display7Segment(0xcc, 0x00);
}