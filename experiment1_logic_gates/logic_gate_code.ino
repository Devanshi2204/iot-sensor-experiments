#include <Wire.h> 
#include <Adafruit_GFX.h> 
#include <Adafruit_SSD1306.h> 
 
// OLED Display Settings 
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
#define OLED_RESET -1 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); 
 
// Pin Definitions for ESP32 
const int INPUT_A = 12;      // GPIO12 - Output to logic gate input A 
const int INPUT_B = 14;      // GPIO14 - Output to logic gate input B 
const int GATE_OUTPUT = 27;  // GPIO27 - Input from logic gate output 
 
// Test Configuration 
// #define TEST_NOR_GATE    // Uncomment the gate type you're testing 
#define TEST_AND_GATE 
//#define TEST_NAND_GATE 
 
// Variables 
int inputAState = 0; 
int inputBState = 0; 
int gateOutputState = 0; 
int expectedOutput = 0; 
int testCase = 0; 
bool testPassed = false; 
 
void setup() { 
  Serial.begin(115200); 
  delay(100);  // Allow serial to initialize 
   
  Serial.println("\n\n================================"); 
  #ifdef TEST_NOR_GATE 
  Serial.println("   NOR GATE TESTER - ESP8266"); 
  #elif defined(TEST_AND_GATE) 
  Serial.println("   AND GATE TESTER - ESP8266"); 
  #elif defined(TEST_NAND_GATE) 
  Serial.println("  NAND GATE TESTER - ESP8266"); 
  #endif 
  Serial.println("================================"); 
   
  // Initialize pins 
  pinMode(INPUT_A, OUTPUT); 
  pinMode(INPUT_B, OUTPUT); 
  pinMode(GATE_OUTPUT, INPUT_PULLUP);  // Pull-up for reading gate output 
   
  // Set initial states 
  digitalWrite(INPUT_A, LOW); 
  digitalWrite(INPUT_B, LOW); 
   
  // Initialize OLED 
  Wire.begin(D2, D1);  // SDA=D2 (GPIO4), SCL=D1 (GPIO5) for ESP8266 
  // Default ESP32 I2C pins: GPIO21 (SDA), GPIO22 (SCL) 
  //Wire.begin(21, 22);  // No need to specify for most ESP32 boards 
   
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed")); 
    for(;;); 
  } 
   
  display.clearDisplay(); 
  display.setTextSize(1); 
  display.setTextColor(SSD1306_WHITE); 
   
  // Show startup screen 
  display.setCursor(0, 0); 
  #ifdef TEST_NOR_GATE 
  display.println(F("  NOR GATE TESTER")); 
  #elif defined(TEST_AND_GATE) 
  display.println(F("  AND GATE TESTER")); 
  #elif defined(TEST_NAND_GATE) 
  display.println(F(" NAND GATE TESTER")); 
  #endif 
  display.println(F("=================")); 
  display.println(F("ESP8266 Based")); 
  display.println(F("Initializing...")); 
  display.display(); 
  delay(2000); 
   
  Serial.println("Pins configured:"); 
  Serial.print("INPUT_A (A): GPIO"); 
  Serial.println(INPUT_A); 
  Serial.print("INPUT_B (B): GPIO"); 
  Serial.println(INPUT_B); 
  Serial.print("GATE_OUTPUT: GPIO"); 
  Serial.println(GATE_OUTPUT); 
  Serial.println("================================"); 
} 
 
void loop() { 
  // Test all 4 combinations: 00, 01, 10, 11 
  int testCases[4][2] = {{0,0}, {0,1}, {1,0}, {1,1}}; 
   
  for(testCase = 0; testCase < 4; testCase++) { 
    inputAState = testCases[testCase][0]; 
    inputBState = testCases[testCase][1]; 
     
    // Set inputs to logic gate (from ESP8266) 
    digitalWrite(INPUT_A, inputAState); 
    digitalWrite(INPUT_B, inputBState); 
     
    delay(50); // Allow signals to stabilize (propagation delay) 
     
    // Read logic gate output (into ESP8266) 
    gateOutputState = digitalRead(GATE_OUTPUT); 
     
    // Calculate expected output based on gate type 
    #ifdef TEST_NOR_GATE 
    expectedOutput = !(inputAState || inputBState);  // NOR gate 
    #elif defined(TEST_AND_GATE) 
    expectedOutput = (inputAState && inputBState);   // AND gate 
    #elif defined(TEST_NAND_GATE) 
    expectedOutput = !(inputAState && inputBState);  // NAND gate 
    #endif 
     
    // Check if test passed 
    testPassed = (gateOutputState == expectedOutput); 
     
    // Update OLED Display 
    updateDisplay(); 
     
    // Serial Monitor output 
    Serial.println("-------------------------------"); 
    #ifdef TEST_NOR_GATE 
    Serial.print("NOR Gate Test "); 
    #elif defined(TEST_AND_GATE) 
    Serial.print("AND Gate Test "); 
    #elif defined(TEST_NAND_GATE) 
    Serial.print("NAND Gate Test "); 
    #endif 
    Serial.println(testCase + 1); 
    Serial.print("Input A (GPIO"); 
    Serial.print(INPUT_A); 
    Serial.print("): "); 
    Serial.println(inputAState ? "HIGH (1)" : "LOW (0)"); 
     
    Serial.print("Input B (GPIO"); 
    Serial.print(INPUT_B); 
    Serial.print("): "); 
    Serial.println(inputBState ? "HIGH (1)" : "LOW (0)"); 
     
    Serial.print("Gate Output (GPIO"); 
    Serial.print(GATE_OUTPUT); 
    Serial.print("): "); 
    Serial.println(gateOutputState ? "HIGH (1)" : "LOW (0)"); 
     
    Serial.print("Expected Output: "); 
    Serial.println(expectedOutput ? "HIGH (1)" : "LOW (0)"); 
     
    Serial.print("Result: "); 
    if(testPassed) { 
      Serial.println("PASS ✓"); 
    } else { 
      Serial.println("FAIL ✗"); 
    } 
     
    delay(3000); // Wait 3 seconds between tests 
  } 
   
  // Summary after all tests 
  displaySummary(); 
  delay(5000); // Wait 5 seconds before restarting tests 
} 
 
void updateDisplay() { 
  display.clearDisplay(); 
   
  // Title 
  display.setTextSize(1); 
  display.setCursor(0, 0); 
  #ifdef TEST_NOR_GATE 
  display.println(F("  NOR")); 
  #elif defined(TEST_AND_GATE) 
  display.println(F("  AND")); 
  #elif defined(TEST_NAND_GATE) 
  display.println(F(" NAND")); 
  #endif 
  display.setTextSize(1); 
  display.println(F("=============")); 
   
  // Test case number 
  display.print(F("Test: ")); 
  display.print(testCase + 1); 
  display.println(F("/4")); 
   
  // Input States 
  display.print(F("A=")); 
  display.print(inputAState); 
  display.print(F("  B=")); 
  display.println(inputBState); 
   
  display.println(F("------------")); 
   
  // Output States 
  display.print(F("Output: ")); 
  display.print(gateOutputState); 
  display.print(F(" (")); 
  display.print(gateOutputState ? "HIGH" : "LOW"); 
  display.println(F(")")); 
   
  // Expected output 
  display.print(F("Expect: ")); 
  display.print(expectedOutput); 
  display.print(F(" (")); 
  display.print(expectedOutput ? "HIGH" : "LOW"); 
  display.println(F(")")); 
  display.println(F("------------")); 
  display.display(); 
  } 
 
void displaySummary() { 
  display.clearDisplay(); 
   
  display.setTextSize(1); 
  display.setCursor(0, 0); 
  #ifdef TEST_NOR_GATE 
  display.println(F(" NOR")); 
  display.println(F(" TEST")); 
  #elif defined(TEST_AND_GATE) 
  display.println(F(" AND")); 
  display.println(F(" TEST")); 
  #elif defined(TEST_NAND_GATE) 
  display.println(F("NAND")); 
  display.println(F(" TEST")); 
  #endif 
   
  display.setTextSize(1); 
  display.println(F("=============")); 
  display.println(F("Complete!")); 
  display.println(F("")); 
  display.println(F("Press RESET to")); 
  display.println(F("run tests again")); 
   
  display.display(); 
   
  Serial.println("\n================================"); 
  Serial.println("Test Sequence Complete!"); 
  Serial.println("All 4 test cases executed."); 
  Serial.println("Press ESP8266 RESET button"); 
  Serial.println("to run tests again."); 
  Serial.println("================================"); 
} 
