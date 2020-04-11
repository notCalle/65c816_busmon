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

#define HZ 500

#define CLOCK_PIN LED_BUILTIN
#define A_PIN(x) (22+x)
#define D_PIN(x) (38+x)
#define RW_PIN 12
#define RES_PIN 11
#define VDA_PIN 10
#define ML_PIN 9
#define VPA_PIN 8
#define IRQ_PIN 7
#define NMI_PIN 6
#define T_PIN(x) (2+x)

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(250000);
  Serial.println();
  
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(RW_PIN, INPUT);
  pinMode(RES_PIN, OUTPUT);
  pinMode(VPA_PIN, INPUT);
  pinMode(ML_PIN, INPUT);
  pinMode(VDA_PIN, INPUT);
  pinMode(IRQ_PIN, INPUT);
  pinMode(NMI_PIN, OUTPUT);
  
  digitalWrite(IRQ_PIN, HIGH);
  digitalWrite(NMI_PIN, HIGH);
  digitalWrite(RES_PIN, LOW);

  for (int pin = A_PIN(0); pin <= A_PIN(23); pin++) {
    pinMode(pin, INPUT);
  }
  for (int pin = T_PIN(0); pin <= T_PIN(3); pin++) {
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

String pad(String s, int len, char pad) {
  String ret;
  int l = s.length();
  
  s.toUpperCase();
  
  ret.reserve(len);
  for (int n = 0; n < len-l; n++) {
    ret += pad;
  }
  return ret + s;
}

String zeropad(String s, int len) {
  return pad(s, len, '0');
}

String spacepad(String s, int len) {
  return pad(s, len, ' ');
}

void printOpcode(uint8_t opc) {
  Serial.print(" ");
  Serial.print(opcodes[opc]);
}

uint8_t readRam(uint32_t addr) {
  uint8_t d;
  pinMode(RW_PIN, OUTPUT);
  digitalWrite(CLOCK_PIN, LOW);
  writePins(A_PIN(0), A_PIN(15), addr);
  digitalWrite(RW_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(CLOCK_PIN, HIGH);
  delayMicroseconds(10);
  d = readPins(D_PIN(0), D_PIN(7));
  digitalWrite(CLOCK_PIN, LOW);
  pinMode(RW_PIN, INPUT);
  return d;
}

void printRam(uint32_t addr, int len, bool text) {
  String ascii = String();
  
  for (int n = 0; n < len; n++) {
    uint8_t d = readRam(addr + n);
    Serial.print(String(" ") + zeropad(String(d, HEX), 2));
    if (d >= 32 && d < 127) {
      ascii += (char)d;
    } else {
      ascii += "·";
    }
  }
  if (text)
    Serial.print(String(" | ") + ascii + " | ");
}

bool deadBeefCafe() {
  static uint8_t magic[] = { 0xde, 0xad, 0xbe, 0xef, 0xca, 0xfe };
  for (int n = 0; n < 6; n++) {
    if (readRam(0x7a + n) != magic[n]) return false;
  }
  return true;
}

void halt() {
  uint16_t sp, dp;
  
  digitalWrite(RES_PIN, LOW);

  sp = readRam(0x80) + (readRam(0x81) << 8);
  dp = readRam(sp-11) + (readRam(sp-10) << 8);
  
  Serial.println();
  if (deadBeefCafe()) {
    Serial.println("********************************** PANIC ***********************************");
    Serial.println();
    Serial.println("SP   DB DIREC Y-reg X-reg A-reg nvmxdizc PC    PB");
    Serial.print(zeropad(String(sp, HEX), 4));
    printRam(sp-12, 9, false);
    Serial.print(" ");
    Serial.print(zeropad(String(readRam(sp-3), BIN), 8));
    printRam(sp-2, 3, false);
    Serial.println();
    Serial.println("--------------------------------------------------------+------------------+");
    Serial.println("STACK    0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F | 0123456789ABCDEF |");
    Serial.println("--------------------------------------------------------+------------------+");
    for (uint16_t a = sp & 0xfff0; a < (sp & 0xff00) + 0x100; a += 0x10) {
      Serial.print(zeropad(String(a, HEX), 6));
      Serial.print(":");
      printRam(a, 16, true);
      Serial.println();
    }    
  } else {
    Serial.println("********************************** HALTED **********************************");
    Serial.println();
  }
  Serial.println("--------------------------------------------------------+------------------+");
  Serial.println("SYSZP    0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F | 0123456789ABCDEF |");
  Serial.println("--------------------------------------------------------+------------------+");
  for (uint16_t a = 0x0000; a < 0x0100; a += 0x10) {
    Serial.print(zeropad(String(a, HEX), 6));
    Serial.print(":");
    printRam(a, 16, true);
    Serial.println();
  }
  if (dp != 0x0000) {
    Serial.println("--------------------------------------------------------+------------------+");
    Serial.println("TASKZP   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F | 0123456789ABCDEF |");
    Serial.println("--------------------------------------------------------+------------------+");
    for (uint16_t a = dp; a < dp + 0x0100; a += 0x10) {
      Serial.print(zeropad(String(a, HEX), 6));
      Serial.print(":");
      printRam(a, 16, true);
      Serial.println();
    }
  }
  Serial.println("--------------------------------------------------------+------------------+");
}


static long reset = 1;
static long count = 0;

bool prompt = true;
bool pause = true;
bool pending = false;
bool single = false;
bool trace = true;
uint32_t breakpoint = 0xffffffff;
String command = String();

char bufUART[100];
unsigned int cntUART = 0;
bool doUART = false;

uint8_t writeUART(uint32_t a, uint8_t d) {
  switch (a & 7) {
    case 0:
      if (trace) {
        if (d == '\n') {
          doUART = true;
          cntUART = 0;
        } else if (cntUART < sizeof(bufUART)-1) {
          bufUART[cntUART++] = d;
          bufUART[cntUART] = 0;
        }
      } else {
        Serial.print((char)d);
      }
      break;
    default:
      break;
  }
  return d;
}

uint8_t readIO(uint32_t a) {
  uint8_t d;
  
  switch (a & 0xfc00) {
    case 0xdc00:
      if (a & 0x100) {
        if (trace) Serial.print("    *UARTA*");
      } else {
        if (trace) Serial.print("    *UARTB*");
      }
      d = 0xff; //readUART((a & 0x170) >> 4);
      break;
    default:
      if (trace) Serial.print("*UNDEFINED*");
      d = 0xff;
  }
  if (trace) {
    Serial.print(zeropad(String(a & 0x03ff, HEX), 3));
    Serial.print(" r     : ");
    Serial.println(zeropad(String(d, HEX), 2));
  }
  return d;
}

void writeIO(uint32_t a, uint8_t d) {
  switch (a & 0xfc00) {
    case 0xdc00:
      if (a & 0x100) {
        if (trace) Serial.print("    *UARTA*");
      } else {
        if (trace) Serial.print("    *UARTB*");
      }
      d = writeUART((a & 0x170) >> 4, d);
      break;
    default:
      if (trace) Serial.print("*UNDEFINED*");
      break;
  }  
  if (trace) {
    Serial.print(zeropad(String(a & 0x3ff, HEX), 3));
    Serial.print(" W     : ");
    Serial.println(zeropad(String(d, HEX), 2));
  }
}

bool simulatedIO(uint32_t a) {
  switch (a & 0xfc00) {
    case 0xdc00:
      return true;
    default:
      break;
  }
  return false;
}

bool checkIRQ() {
  return false;
}

uint32_t fromHex(String s) {
  uint32_t value = 0;
  int l = s.length();

  s.toUpperCase();
  s.trim();
  for (int i = 0; i < l; i++) {
    char c = s[i];

    value = value << 4;
    c -= '0';
    if (c > 9) c -= ('@' - '9');
    if (c > 15 || c < 0) return 0xffffffff;
    value += c;
  }
  return value;
}

// the loop function runs over and over again forever
void loop() {
  uint32_t a;
  uint8_t d = 0xff;
  bool rW, vpa, ml, vda;
  bool romRead = false, ioRegion = false, ioRead = false;
  bool nmi = false;
  bool irq = false; //checkIRQ();
  
  if (pending || pause) {
    if (prompt) Serial.print("-----------------------------> ");
    prompt = false;
  }

  if (pending) {
    Serial.println(command);
    if (command == "panic") {
      nmi = true;
      single = false;
      trace = false;
    } else if (command == "step") {
      single = true;
    } else if (command == "cont") {
      single = false;
    } else if (command.startsWith("break ")) {
      breakpoint = fromHex(command.substring(6));
      pause = true;
    } else if (command == "clear") {
      breakpoint = 0xffffffff;
      pause = true;
    } else if (command == "reset") {
      count = 0;
      reset = 10;
      digitalWrite(RES_PIN, LOW);
    } else if (command == "trace off") {
      pause = true;
      trace = false;
    } else if (command == "trace on") {
      pause = true;
      trace = true;
    }
    pending = false;
    command = String();
    prompt = true;
  }

  if (pause) return;

  if (reset-- == 0) {
    Serial.println("-------------------------------------------------------------------------");
    Serial.println("********************************* RESET *********************************");
    digitalWrite(RES_PIN, HIGH);
  }

  //digitalWrite(IRQ_PIN, irq ? LOW : HIGH);
  digitalWrite(NMI_PIN, nmi ? LOW : HIGH);
  
  // Phase 1
  digitalWrite(CLOCK_PIN, LOW);
  (void)readPins(D_PIN(0), D_PIN(7));
  delay(500/HZ);

  a = readPins(A_PIN(0), A_PIN(15));
  rW = digitalRead(RW_PIN);
  vpa = digitalRead(VPA_PIN);
  ml = !digitalRead(ML_PIN);
  vda = digitalRead(VDA_PIN);
  irq = !digitalRead(IRQ_PIN);
  // delay(250/HZ);

  // Phase 2
  digitalWrite(CLOCK_PIN, HIGH);
  // delay(250/HZ);

  romRead = rW && a >= 0xe000 && a <= 0xffff;
  if (romRead) {
    d = pgm_read_byte_far(rom + (a & 0x1fff));
    writePins(D_PIN(0), D_PIN(7), d); 
  }

  ioRegion = simulatedIO(a);
  ioRead = rW && ioRegion;
  if (ioRead) {
    d = readIO(a);
    writePins(D_PIN(0), D_PIN(7), d); 
  }

  delay(500/HZ);

  if (!romRead && !ioRead) {
    d = readPins(D_PIN(0), D_PIN(7));
  }

  // Only show bus active cycles
  if (vpa || vda || ml) {
    if (breakpoint == a) single = true;
    
    if (trace || single) {
      if ((count % 25) == 0) {
        Serial.println("------------------------------------");
        Serial.println(" CLOCK  ADDRES CTRL    DA OPC TEST");
        Serial.println("------------------------------------");
      }
      Serial.print(spacepad(String(count, DEC), 6));
      Serial.print("  ");
      Serial.print(zeropad(String(a, HEX), 6));
      Serial.print(" ");
      Serial.print(rW ? "r" : "W");
      Serial.print(ml ? "*" : " ");
      Serial.print(vda ? "d" : " ");
      Serial.print(vpa ? "p" : " ");
      Serial.print("  : ");
      if (vpa|vda) {
        Serial.print(zeropad(String(d, HEX), 2));
      } else {
        Serial.print("  ");
      }
    
      if (vda && vpa) {
        printOpcode(d);
      } else {
        Serial.print("    ");
      }

      uint32_t t_pins = readPins(T_PIN(0), T_PIN(3));
      String t_bits = zeropad(String(t_pins, BIN), 4);
      String t_hex = String(t_pins, HEX);
  
      Serial.print(" " + t_bits + " " + t_hex);
      
      Serial.println(nmi ? "        *NMI*" :
                     irq ? "        *IRQ*" : "");
    }

    if (doUART) {
      Serial.print("UART: ");
      Serial.println(bufUART);
      doUART = false;
    }
    
    if (single) pause = true;
  }

  if (!rW && ioRegion) {
    writeIO(a, d);
  }
  
  // STP instruction loaded
  if (vda && vpa && (d == 0xdb)) {
    halt();
    pause = true;
    prompt = true;
    trace = true;
    single = true;
  }
  
  count++;
}

void serialEvent() {
  pause = true;
  pending = false;
  while (Serial.available()) {
    char in = (char)Serial.read();
    switch (in) {
      case '\n':
        pause = false;
        pending = true;
        break;
      default:
        command += in;
    }  
  }
}
