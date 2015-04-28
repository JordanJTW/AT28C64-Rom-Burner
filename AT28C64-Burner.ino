const byte DEBUG = true;

const int ON = LOW;
const int OFF = HIGH;

const int chipEnable = 47;
const int outputEnable = 48;
const int writeEnable = 49;

const int dataBase = 22, 
    dataLength = 8;
const int addrBase = 30, 
    addrLength = 13;
    
const byte program[] = { 
      0xAD, 0x1F, 0x00, 0x8D, 0x03, 0x00,
      0xAD, 0x0B, 0x00, 0x8D, 0x02, 0x00,
      0xAD, 0x4A, 0x00, 0x20, 0x15, 0x00,
      0x4C, 0x00, 0x00, 0x48, 0xA9, 0x10,
      0x2C, 0x01, 0x00, 0xF0, 0xFB, 0x68,
      0x8D, 0x00, 0x00, 0x60 
    };
const int baseAddress = 0xF;

void setup() {
  pinMode(chipEnable, OUTPUT);
  pinMode(outputEnable, OUTPUT);
  pinMode(writeEnable, OUTPUT); 
 
  digitalWrite(chipEnable, OFF);
  digitalWrite(outputEnable, OFF);
  digitalWrite(writeEnable, OFF);

  Serial.begin(115200);
  
//  writeProgram();
  
  digitalWrite(writeEnable, OFF);
  delay(20);
  
  Serial.println(verifyProgram() ? "Written!" : "Error...");
}

void loop() {}

byte verifyProgram() {
  for (int i=0; i<sizeof(program); ++i) {
    int address = baseAddress + i;
    byte value = readByte(address);
    
    if (value != program[i] && i != 0) {
      Serial.println(value, 16);
      Serial.println(program[i], 16);
      return false;
    }
  }
  return true;
}

void writeProgram() {
  for (int i=0; i<sizeof(program); ++i) {
    int address = baseAddress + i;
    writeByte(address, program[i]);
  }
}

void test() {
  Serial.println("****************** BEGIN ******************");
 
  for (int i=0; i<=0xFF; ++i) 
    writeByte(0xFFF + i, 0xBA);
    
  digitalWrite(writeEnable, OFF);
  delay(20);

  for (int i=0; i<=0xFF; ++i) {
    byte value = readByte(0xFFF + i);
    Serial.println(value, 16);
  }
  
  Serial.println("****************** END ********************");
}

byte readByte(int address) {
  writeToPins(addrBase, address, addrLength);
  
  digitalWrite(chipEnable, ON);
  digitalWrite(outputEnable, ON);
  
  byte data =  getDataOnPins();
  
  digitalWrite(outputEnable, OFF);
  digitalWrite(chipEnable, OFF);
  
  return data;
}

byte getDataOnPins() {
  byte value = 0;
  
  for (int i=0; i<dataLength; ++i) {
    int pin = dataBase + i;
    pinMode(pin, INPUT);
 
    int b = digitalRead(pin);
    if (b == HIGH) {
      bitSet(value, i);
    }
  }
  
  return value; 
}
    
void writeByte(int addr, byte data) {
  if (DEBUG) {
    Serial.print("Writing ");
    printHex(data);
    Serial.print(" to ");
    Serial.println(addr);
  }
  
  digitalWrite(chipEnable, OFF);   
  digitalWrite(outputEnable, ON);
  digitalWrite(writeEnable, OFF);
  
  writeToPins(dataBase, data, dataLength);
  delay(1);

  digitalWrite(chipEnable, ON);   
  digitalWrite(outputEnable, OFF);

  writeToPins(addrBase, addr, addrLength);
  delay(1);

  digitalWrite(writeEnable, ON);  
  digitalWrite(chipEnable, OFF); 

  delay(5);  
}

void writeToPins(int base, int data, int length) {  
  for (int i=0; i<length; ++i) {
    int b = bitRead(data, i);
    int pin = base + i;
    
    pinMode(pin, OUTPUT);
    digitalWrite(pin, b == 1 ? HIGH : LOW);
  }
}
 
void printHex(char value) {  
  Serial.print("0x");
  Serial.print(value & 0xFF, 16);
}
  


