/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Blink
*/


const static PROGMEM uint8_t rom[] = {
#include "/Users/calle/Workspace/X65OS/rom.h"
};

const static char * opcodes[] = {
    "BRK", "ORA", "COP", "ORA", "TSB", "ORA", "ASL", "ORA",  "PHP", "ORA", "ASL", "PHD", "TSB", "ORA", "ASL", "ORA",
    "BPL", "ORA", "ORA", "ORA", "TRB", "ORA", "ASL", "ORA",  "CLC", "ORA", "INA", "TCS", "TRB", "ORA", "ASL", "ORA",
    "JSR", "AND", "JSR", "AND", "BIT", "AND", "ROL", "AND",  "PLP", "AND", "ROL", "PLD", "BIT", "AND", "ROL", "AND",
    "BMI", "AND", "AND", "AND", "BIT", "AND", "ROL", "AND",  "SEC", "AND", "DEA", "TSC", "BIT", "AND", "ROL", "AND",
    "RTI", "EOR", "WDM", "EOR", "MVP", "EOR", "LSR", "EOR",  "PHA", "EOR", "LSR", "PHK", "JMP", "EOR", "LSR", "EOR",
    "BVC", "EOR", "EOR", "EOR", "MVN", "EOR", "LSR", "EOR",  "CLI", "EOR", "PHY", "TCD", "JMP", "EOR", "LSR", "EOR",
    "RTS", "ADC", "PER", "ADC", "STZ", "ADC", "ROR", "ADC",  "PLA", "ADC", "ROR", "RTL", "JMP", "ADC", "ROR", "ADC",
    "BVS", "ADC", "ADC", "ADC", "STZ", "ADC", "ROR", "ADC",  "SEI", "ADC", "PLY", "TDC", "JMP", "ADC", "ROR", "ADC", 
    "BRA", "STA", "BRL", "STA", "STY", "STA", "STX", "STA",  "DEY", "BIT", "TXA", "PHB", "STY", "STA", "STX", "STA",
    "BCC", "STA", "STA", "STA", "STY", "STA", "STX", "STA",  "TYA", "STA", "TXS", "TXY", "STZ", "STA", "STZ", "STA",
    "LDY", "LDA", "LDX", "LDA", "LDY", "LDA", "LDX", "LDA",  "TAY", "LDA", "TAX", "PLB", "LDY", "LDA", "LDX", "LDA",
    "BCS", "LDA", "LDA", "LDA", "LDY", "LDA", "LDX", "LDA",  "CLV", "LDA", "TSX", "TYX", "LDY", "LDA", "LDX", "LDA",
    "CPY", "CMP", "REP", "CMP", "CPY", "CMP", "DEC", "CMP",  "INY", "CMP", "DEX", "WAI", "CPY", "CMP", "DEC", "CMP",
    "BNE", "CMP", "CMP", "CMP", "PEI", "CMP", "DEC", "CMP",  "CLD", "CMP", "PHX", "STP", "JMP", "CMP", "DEC", "CMP",
    "CPX", "SBC", "SEP", "SBC", "CPX", "SBC", "INC", "SBC",  "INX", "SBC", "NOP", "XBA", "CPX", "SBC", "INC", "SBC",
    "BEQ", "SBC", "SBC", "SBC", "PEA", "SBC", "INC", "SBC",  "SED", "SBC", "PLX", "XCE", "JSR", "SBC", "INC", "SBC"
};

static uint8_t ram[4096];

#define CLOCK_PIN LED_BUILTIN
#define A_PIN(x) (22+x)
#define D_PIN(x) (38+x)
#define RW_PIN 12
#define RES_PIN 11
#define VDA_PIN 10
#define ML_PIN 9
#define VPA_PIN 8

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(250000);
  
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(RW_PIN, INPUT);
  pinMode(RES_PIN, OUTPUT);
  pinMode(VPA_PIN, INPUT);
  pinMode(ML_PIN, INPUT);
  pinMode(VDA_PIN, INPUT);
  digitalWrite(RES_PIN, LOW);
  for (int pin = A_PIN(0); pin <= A_PIN(23); pin++) {
    pinMode(pin, INPUT);
  }
}

uint32_t readPins(int first, int last) {
  uint32_t res = 0, pin_value = 1;
  for (int pin = first; pin <= last; pin++, pin_value *= 2) {
    pinMode(pin, INPUT);
    if (digitalRead(pin)) {
      res |= pin_value;
    }
  }
  return res;
}

void writePins(int first, int last, uint32_t value) {
  for (int pin = first; pin <= last; pin++, value /= 2) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, value & 1);
  }
}

String zeropad(String s, int len) {
  String ret;
  int l = s.length();
  
  s.toUpperCase();
  
  ret.reserve(len);
  for (int n = 0; n < len-l; n++) {
    ret += "0";
  }
  return ret + s;
}

void printOpcode(uint8_t opc) {
  Serial.print(" ");
  Serial.print(opcodes[opc]);
}

void printRam(uint32_t addr, int len) {
  for (int n = 0; n < len; n++)
    Serial.print(String(" ") + zeropad(String(ram[addr + n], HEX), 2));
}

bool deadBeefCafe() {
  static uint8_t magic[] = { 0xde, 0xad, 0xbe, 0xef, 0xca, 0xfe };
  return memcmp(&ram[0xa], magic, 6) == 0;
}

void panic() {
  uint16_t splo = ram[0x10], sphi = ram[0x11];
  uint16_t sp = splo + (sphi << 8);
  
  Serial.println();
  Serial.println("********************************* PANIC *********************************");
  Serial.println();
  Serial.println("SP   DB DIREC Y-reg X-reg A-reg nvmxdizc PC    PB");
  Serial.print(zeropad(String(sp, HEX), 4));
  printRam(sp+1, 9);
  Serial.print(" ");
  Serial.print(zeropad(String(ram[sp+10], BIN), 8));
  printRam(sp+11, 3);
  Serial.println();
  Serial.println("-------------------------------------------------------------------------");
  Serial.println("ADDRESS  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F");
  Serial.println("-------------------------------------------------------------------------");
  for (int a = 0x0000; a < 0x0200; a += 0x10) {
    Serial.print(zeropad(String(a, HEX), 6));
    Serial.print(":");
    printRam(a, 16);
    Serial.println();
  }
}

static long reset = 1;
static long count = 0;

// the loop function runs over and over again forever
void loop() {
  uint32_t a;
  uint8_t d;
  bool rW, vpa, ml, vda;

  if (reset-- == 0) {
    Serial.println("-------------------------------------------------------------------------");
    Serial.println("********************************* RESET *********************************");
    digitalWrite(RES_PIN, HIGH);
  }
  
  // Phase 1
  digitalWrite(CLOCK_PIN, LOW);
  delay(1);
  a = readPins(A_PIN(0), A_PIN(23));
  rW = digitalRead(RW_PIN);
  vpa = digitalRead(VPA_PIN);
  ml = !digitalRead(ML_PIN);
  vda = digitalRead(VDA_PIN);
  
  delay(1);

  // Phase 2
  digitalWrite(CLOCK_PIN, HIGH);
  delay(1);
  if (rW) {
    if (a >= 0xc000 && a <= 0xffff) {
      d = pgm_read_byte_far(rom + (a & 0x1fff));
    } else {
      d = ram[a & 0x0fff];
    }
    writePins(D_PIN(0), D_PIN(7), d); 
  } else {
    d = readPins(D_PIN(0), D_PIN(7));
    ram[a & 0x0fff] = d;
  }
  delay(1);

  if ((count++ % 25) == 0) {
    Serial.println("-------------------------------------------------------------------------");
    Serial.println("ADDRES CTRL  DA OPC        0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F");
    Serial.println("-------------------------------------------------------------------------");
  }
  Serial.print(zeropad(String(a, HEX), 6));
  Serial.print(" ");
  Serial.print(rW ? "r" : "W");
  Serial.print(ml ? "*" : " ");
  Serial.print(vda ? "d" : " ");
  Serial.print(vpa ? "p" : " ");
  Serial.print(": ");
  if (vpa|vda) {
    Serial.print(zeropad(String(d, HEX), 2));
  }

  if (vda && vpa) {
    printOpcode(d);    
  } else {
    Serial.print("    ");
  }

  if (!rW) {
    Serial.print("  RAM:");
    printRam(a & 0xff0, 16);
  }
  
  Serial.println();
  delay(500);

  // STP instruction loaded
  if (vda && vpa && (d == 0xdb)) {
    if (deadBeefCafe()) panic();
    while (true) { delay(1000); }
  }
}
