#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#define RST_PIN         5          // Configurable, see typical pin layout above
#define SS_PIN          53          // Configurable, see typical pin layout above

Servo item1;
Servo item2;
Servo item3;
Servo item4;
LiquidCrystal_I2C lcd(0x27, 16, 2);
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance
int pos = 0;
char ch;
const int stepPin = 42; 
const int dirPin = 44; 
const int stepPin1 = 38; 
const int dirPin1 = 40; 
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {9,8,7,6}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {10,4,3,2}; //connect to the column pinouts of the keypad 
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 
 
void setup() {
  pinMode(stepPin,OUTPUT); 
  pinMode(dirPin,OUTPUT);
  pinMode(stepPin1,OUTPUT); 
  pinMode(dirPin1,OUTPUT);
 // pinMode(49,OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(35, OUTPUT);
  digitalWrite(35,HIGH);
  pinMode(33, OUTPUT);
  digitalWrite(33,HIGH);
  pinMode(31, OUTPUT);
  digitalWrite(31,HIGH);
  pinMode(45, OUTPUT);
  digitalWrite(45,LOW);
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(1,1);
  lcd.print("WELCOME");
  item1.attach(49);
  //item2.attach(49);
 // item3.attach(26);
  //item4.attach(28);
}
void loop() {
    char customKey = customKeypad.getKey();
    switch(customKey)
    {
      case '1': buy_item1();
                break;
      case '2': buy_item2();
                break;
      case '3': buy_item3();
                break;
      case '5': buy_item4();
                break;
      case '4': buy_coffee();
                break;
      case 'A': check_balance();
                break;       
    }
}

void buy_coffee(){
 Serial.println(F("Read personal data on a MIFARE PICC:"));    //shows in serial that it is ready to read
  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
  //some variables we need
  byte block;
  byte len;
  MFRC522::StatusCode status;

  //-------------------------------------------

  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.println(F("**Card Detected:**"));

  //-------------------------------------------

  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); //dump some details about the card

  //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));      //uncomment this to see all blocks in hex

  //-------------------------------------------

  //Serial.print(F("Name: "));
  byte buffer[38];
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  byte buffer1[18];

  block = 1;
  len = 18;

  //------------------------------------------- GET FIRST NAME
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Read(block, buffer1, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  //PRINT FIRST NAME
  String ch[20];
  Serial.println("Card Holder Name :");
  Serial.println(" ");
  lcd.clear();
  for (uint8_t i = 0,j=0; i < 16; i++,j++)
  {
    if (buffer1[i] != 32)
    {
      lcd.setCursor(j,0);
      Serial.println(i);
      lcd.write(buffer1[i]);
    }
  }
  Serial.println(" ");
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  byte buffer2[18];
  block = 2;

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 2, &key, &(mfrc522.uid)); //line 834
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Read(block, buffer2, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  Serial.println("Amount Present :");
  for (uint8_t i = 0; i < 1; i++) {
    Serial.println(buffer2[i]);
  }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 // Serial.setTimeout(10000L) ;

  if(buffer2[0]>=10)
  {  byte cost = 10;
    Serial.println(F("Amount Left : "));
    len = buffer2[0]-cost;
    Serial.println(len);
    lcd.setCursor(0,1);
    lcd.print("Cost:");
    lcd.print(cost);
    buffer[0]=len;
    for (byte i = len; i < 30; i++) buffer[i] = ' ';     // pad with spaces
    block = 2;
    //Serial.println(F("Authenticating using key A..."));
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("PCD_Authenticate() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }

    // Write block
    status = mfrc522.MIFARE_Write(block, buffer, 16);
    if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
    }
    else 
    Serial.println(F("MIFARE_Write() success: "));

  Serial.println(F("\n**End Reading**\n"));
  delay(1000); //change value if you want to read cards faster

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  coffee();
  }
  else
  {
     Serial.println(F("\n**End Reading**\n"));
  delay(1000); //change value if you want to read cards faster

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1(); 
  }
  Serial.println(F("\n**End Reading**\n"));
  delay(3000);
  lcd.clear();
  lcd.print("WELCOME");
  }

void buy_item1()
{
    Serial.println(F("Read personal data on a MIFARE PICC:"));    //shows in serial that it is ready to read
  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
  //some variables we need
  byte block;
  byte len;
  MFRC522::StatusCode status;

  //-------------------------------------------

  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.println(F("**Card Detected:**"));

  //-------------------------------------------

  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); //dump some details about the card

  //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));      //uncomment this to see all blocks in hex

  //-------------------------------------------

  //Serial.print(F("Name: "));
  byte buffer[38];
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  byte buffer1[18];

  block = 1;
  len = 18;

  //------------------------------------------- GET FIRST NAME
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Read(block, buffer1, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  //PRINT FIRST NAME
  String ch[20];
  Serial.println("Card Holder Name :");
  Serial.println(" ");
  lcd.clear();
  for (uint8_t i = 0,j=0; i < 16; i++,j++)
  {
    if (buffer1[i] != 32)
    {
      lcd.setCursor(j,0);
      Serial.println(i);
      lcd.write(buffer1[i]);
    }
  }
  Serial.println(" ");
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  byte buffer2[18];
  block = 2;

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 2, &key, &(mfrc522.uid)); //line 834
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Read(block, buffer2, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  Serial.println("Amount Present :");
  for (uint8_t i = 0; i < 1; i++) {
    Serial.println(buffer2[i]);
  }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 // Serial.setTimeout(10000L) ;

  if(buffer2[0]>=10)
  {  byte cost = 10;
    Serial.println(F("Amount Left : "));
    len = buffer2[0]-cost;
    Serial.println(len);
    lcd.setCursor(0,1);
    lcd.print("Cost:");
    lcd.print(cost);
    buffer[0]=len;
    for (byte i = len; i < 30; i++) buffer[i] = ' ';     // pad with spaces
    block = 2;
    //Serial.println(F("Authenticating using key A..."));
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("PCD_Authenticate() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }

    // Write block
    status = mfrc522.MIFARE_Write(block, buffer, 16);
    if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
    }
    else 
    Serial.println(F("MIFARE_Write() success: "));

  Serial.println(F("\n**End Reading**\n"));
  delay(1000); //change value if you want to read cards faster

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  motor1();
  }
  else
  {
     Serial.println(F("\n**End Reading**\n"));
  delay(1000); //change value if you want to read cards faster

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1(); 
  }
  Serial.println(F("\n**End Reading**\n"));
  delay(3000);
  lcd.clear();
  lcd.print("WELCOME");
  }
//*****************************************************************************************/

void buy_item2()
{
    Serial.println(F("Read personal data on a MIFARE PICC:"));    //shows in serial that it is ready to read
  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
  //some variables we need
  byte block;
  byte len;
  MFRC522::StatusCode status;

  //-------------------------------------------

  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.println(F("**Card Detected:**"));

  //-------------------------------------------

  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); //dump some details about the card

  //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));      //uncomment this to see all blocks in hex

  //-------------------------------------------

  //Serial.print(F("Name: "));
  byte buffer[38];
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  byte buffer1[18];

  block = 1;
  len = 18;

  //------------------------------------------- GET FIRST NAME
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Read(block, buffer1, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  //PRINT FIRST NAME
  String ch[20];
  Serial.println("Card Holder Name :");
  Serial.println(" ");
  lcd.clear();
  for (uint8_t i = 0,j=0; i < 16; i++,j++)
  {
    if (buffer1[i] != 32)
    {
      lcd.setCursor(j,0);
      Serial.println(i);
      lcd.write(buffer1[i]);
    }
  }
  Serial.println(" ");
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  byte buffer2[18];
  block = 2;

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 2, &key, &(mfrc522.uid)); //line 834
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Read(block, buffer2, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  Serial.println("Amount Present :");
  for (uint8_t i = 0; i < 1; i++) {
    Serial.println(buffer2[i]);
  }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 // Serial.setTimeout(10000L) ;

  if(buffer2[0]>=10)
  {  byte cost = 20;
    Serial.println(F("Amount Left : "));
    len = buffer2[0]-cost;
    Serial.println(len);
    lcd.setCursor(0,1);
    lcd.print("Cost:");
    lcd.print(cost);
    buffer[0]=len;
    for (byte i = len; i < 30; i++) buffer[i] = ' ';     // pad with spaces
    block = 2;
    //Serial.println(F("Authenticating using key A..."));
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("PCD_Authenticate() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }

    // Write block
    status = mfrc522.MIFARE_Write(block, buffer, 16);
    if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
    }
    else 
    Serial.println(F("MIFARE_Write() success: "));

  Serial.println(F("\n**End Reading**\n"));
  delay(1000); //change value if you want to read cards faster

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  motor2();
  item_dropper2();}
  else
  {
     Serial.println(F("\n**End Reading**\n"));
  delay(1000); //change value if you want to read cards faster

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1(); 
  }
  Serial.println(F("\n**End Reading**\n"));
  delay(3000);
  lcd.clear();
  lcd.print("WELCOME");
  }
//*****************************************************************************************/

void buy_item3()
{
    Serial.println(F("Read personal data on a MIFARE PICC:"));    //shows in serial that it is ready to read
  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
  //some variables we need
  byte block;
  byte len;
  MFRC522::StatusCode status;

  //-------------------------------------------

  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.println(F("**Card Detected:**"));

  //-------------------------------------------

  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); //dump some details about the card

  //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));      //uncomment this to see all blocks in hex

  //-------------------------------------------

  //Serial.print(F("Name: "));
  byte buffer[38];
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  byte buffer1[18];

  block = 1;
  len = 18;

  //------------------------------------------- GET FIRST NAME
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Read(block, buffer1, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  //PRINT FIRST NAME
  String ch[20];
  Serial.println("Card Holder Name :");
  Serial.println(" ");
  lcd.clear();
  for (uint8_t i = 0,j=0; i < 16; i++,j++)
  {
    if (buffer1[i] != 32)
    {
      lcd.setCursor(j,0);
      Serial.println(i);
      lcd.write(buffer1[i]);
    }
  }
  Serial.println(" ");
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  byte buffer2[18];
  block = 2;

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 2, &key, &(mfrc522.uid)); //line 834
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Read(block, buffer2, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  Serial.println("Amount Present :");
  for (uint8_t i = 0; i < 1; i++) {
    Serial.println(buffer2[i]);
  }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 // Serial.setTimeout(10000L) ;

  if(buffer2[0]>=10)
  {  byte cost = 30;
    Serial.println(F("Amount Left : "));
    len = buffer2[0]-cost;
    Serial.println(len);
    lcd.setCursor(0,1);
    lcd.print("Cost:");
    lcd.print(cost);
    buffer[0]=len;
    for (byte i = len; i < 30; i++) buffer[i] = ' ';     // pad with spaces
    block = 2;
    //Serial.println(F("Authenticating using key A..."));
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("PCD_Authenticate() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }

    // Write block
    status = mfrc522.MIFARE_Write(block, buffer, 16);
    if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
    }
    else 
    Serial.println(F("MIFARE_Write() success: "));

  Serial.println(F("\n**End Reading**\n"));
  delay(1000); //change value if you want to read cards faster

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  motor3();
  item_dropper3();}
  else
  {
     Serial.println(F("\n**End Reading**\n"));
  delay(1000); //change value if you want to read cards faster

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1(); 
  }
  Serial.println(F("\n**End Reading**\n"));
  delay(3000);
  lcd.clear();
  lcd.print("WELCOME");
  }
//*****************************************************************************************/

void buy_item4()
{
    Serial.println(F("Read personal data on a MIFARE PICC:"));    //shows in serial that it is ready to read
  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
  //some variables we need
  byte block;
  byte len;
  MFRC522::StatusCode status;

  //-------------------------------------------

  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.println(F("**Card Detected:**"));

  //-------------------------------------------

  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); //dump some details about the card

  //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));      //uncomment this to see all blocks in hex

  //-------------------------------------------

  //Serial.print(F("Name: "));
  byte buffer[38];
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  byte buffer1[18];

  block = 1;
  len = 18;

  //------------------------------------------- GET FIRST NAME
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Read(block, buffer1, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  //PRINT FIRST NAME
  String ch[20];
  Serial.println("Card Holder Name :");
  Serial.println(" ");
  lcd.clear();
  for (uint8_t i = 0,j=0; i < 16; i++,j++)
  {
    if (buffer1[i] != 32)
    {
      lcd.setCursor(j,0);
      Serial.println(i);
      lcd.write(buffer1[i]);
    }
  }
  Serial.println(" ");
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  byte buffer2[18];
  block = 2;

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 2, &key, &(mfrc522.uid)); //line 834
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Read(block, buffer2, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  Serial.println("Amount Present :");
  for (uint8_t i = 0; i < 1; i++) {
    Serial.println(buffer2[i]);
  }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 // Serial.setTimeout(10000L) ;

  if(buffer2[0]>=10)
  {  byte cost = 40;
    Serial.println(F("Amount Left : "));
    len = buffer2[0]-cost;
    Serial.println(len);
    lcd.setCursor(0,1);
    lcd.print("Cost:");
    lcd.print(cost);
    buffer[0]=len;
    for (byte i = len; i < 30; i++) buffer[i] = ' ';     // pad with spaces
    block = 2;
    //Serial.println(F("Authenticating using key A..."));
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("PCD_Authenticate() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }

    // Write block
    status = mfrc522.MIFARE_Write(block, buffer, 16);
    if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
    }
    else 
    Serial.println(F("MIFARE_Write() success: "));

  Serial.println(F("\n**End Reading**\n"));
  delay(1000); //change value if you want to read cards faster

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  motor4();
  item_dropper4();}
  else
  {
     Serial.println(F("\n**End Reading**\n"));
  delay(1000); //change value if you want to read cards faster

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1(); 
  }
  Serial.println(F("\n**End Reading**\n"));
  delay(3000);
  lcd.clear();
  lcd.print("WELCOME");
  }
//*****************************************************************************************/

void check_balance(){
      Serial.println(F("Read personal data on a MIFARE PICC:"));    //shows in serial that it is ready to read
  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  //some variables we need
  byte block;
  byte len;
  MFRC522::StatusCode status;

  //-------------------------------------------

  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.println(F("**Card Detected:**"));

  //-------------------------------------------

  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); //dump some details about the card

  //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));      //uncomment this to see all blocks in hex

  //-------------------------------------------

  //Serial.print(F("Name: "));
  byte buffer[38];
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  byte buffer1[18];

  block = 1;
  len = 18;

  //------------------------------------------- GET FIRST NAME
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Read(block, buffer1, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  //PRINT FIRST NAME
  Serial.println("Card Holder Name :");
  Serial.println(" ");
  lcd.clear();
  for (uint8_t i = 0,j=0; i < 16; i++,j++)
  {
    if (buffer1[i] != 32)
    {
        lcd.setCursor(j,0);
        Serial.println(i);
        lcd.write(buffer1[i]);
    }
  }
  Serial.println(" ");
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  byte buffer2[18];
  block = 2;

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 2, &key, &(mfrc522.uid)); //line 834
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Read(block, buffer2, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  Serial.println("Amount Present :");
  for (uint8_t i = 0; i < 1; i++) {
    Serial.println(buffer2[i]);
  }
  lcd.setCursor(0,1);
  lcd.print("Balance:");
  lcd.print(buffer2[0]);
  Serial.println(F("\n**End Reading**\n"));
  delay(1000); //change value if you want to read cards faster

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  delay(3000);
  lcd.clear();
  lcd.print("WELCOME");
}

void motor1(){
 digitalWrite(dirPin,LOW); 
  for(int x = 0; x < 1200 ;x++) {
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(500);
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(500);
   }
   delay(2000);
         digitalWrite(dirPin1,LOW);
    for(int x = 0; x < 10000 ;x++) {
    digitalWrite(stepPin1,HIGH); 
    delayMicroseconds(500);
    digitalWrite(stepPin1,LOW); 
    delayMicroseconds(500);
   }
   item_dropper1();
   delay(1000);
            digitalWrite(dirPin1,HIGH);
    for(int x = 0; x < 10000 ;x++) {
    digitalWrite(stepPin1,HIGH); 
    delayMicroseconds(500);
    digitalWrite(stepPin1,LOW); 
    delayMicroseconds(500);
   }
   delay(1000);
    digitalWrite(dirPin,HIGH); 
  for(int x = 0; x < 2000 ;x++) {
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(500);
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(500);
   }

}

void motor2(){
 digitalWrite(dirPin,LOW); 
  for(int x = 0; x < 1200 ;x++) {
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(500);
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(500);
   }
   delay(1000);
            digitalWrite(dirPin1,HIGH);
    for(int x = 0; x < 3000 ;x++) {
    digitalWrite(stepPin1,HIGH); 
    delayMicroseconds(500);
    digitalWrite(stepPin1,LOW); 
    delayMicroseconds(500);
   }
   delay(1000);
            digitalWrite(dirPin1,LOW);
    for(int x = 0; x < 3000 ;x++) {
    digitalWrite(stepPin1,HIGH); 
    delayMicroseconds(500);
    digitalWrite(stepPin1,LOW); 
    delayMicroseconds(500);
   }
   delay(1000);
   digitalWrite(dirPin,HIGH);
    for(int x = 0; x < 2000 ;x++) {
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(500);
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(500);
   }
}

void motor3(){
 digitalWrite(dirPin,LOW); 
  for(int x = 0; x < 600 ;x++) {
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(500);
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(500);
   }
   delay(2000);
         digitalWrite(dirPin1,LOW);
    for(int x = 0; x < 10000 ;x++) {
    digitalWrite(stepPin1,HIGH); 
    delayMicroseconds(500);
    digitalWrite(stepPin1,LOW); 
    delayMicroseconds(500);
   }
   delay(1000);
            digitalWrite(dirPin1,HIGH);
    for(int x = 0; x < 10000 ;x++) {
    digitalWrite(stepPin1,HIGH); 
    delayMicroseconds(500);
    digitalWrite(stepPin1,LOW); 
    delayMicroseconds(500);
   }
   delay(1000);
    digitalWrite(dirPin,HIGH); 
  for(int x = 0; x < 1000 ;x++) {
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(500);
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(500);
   }
}

void motor4(){
 digitalWrite(dirPin,LOW); 
  for(int x = 0; x < 600 ;x++) {
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(500);
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(500);
   }
   delay(2000);
         digitalWrite(dirPin1,HIGH);
    for(int x = 0; x < 10000 ;x++) {
    digitalWrite(stepPin1,HIGH); 
    delayMicroseconds(500);
    digitalWrite(stepPin1,LOW); 
    delayMicroseconds(500);
   }
   delay(1000);
            digitalWrite(dirPin1,LOW);
    for(int x = 0; x < 10000 ;x++) {
    digitalWrite(stepPin1,HIGH); 
    delayMicroseconds(500);
    digitalWrite(stepPin1,LOW); 
    delayMicroseconds(500);
   }
   delay(1000);
    digitalWrite(dirPin,HIGH); 
  for(int x = 0; x < 1000 ;x++) {
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(500);
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(500);
   }
}
void item_dropper1(){
item1.attach(49);
item1.write(0);
delay(8000);
item1.detach();
}

void item_dropper2(){
item2.attach(49);
item2.write(0);
delay(8000);
item2.detach();
}

void item_dropper3(){
    for(pos = 0; pos < 360; pos += 1)  // goes from 0 degrees to 180 degrees
  {                                  // in steps of 1 degree
    item3.write(pos);              // tell servo to go to position in variable 'pos'
                        // waits 15ms for the servo to reach the position
  }
}

void item_dropper4(){
    for(pos = 0; pos < 360; pos += 1)  // goes from 0 degrees to 180 degrees
  {                                  // in steps of 1 degree
    item4.write(pos);              // tell servo to go to position in variable 'pos'
                        // waits 15ms for the servo to reach the position
  }
}

void coffee(){
  digitalWrite(12, 0);  
  digitalWrite(13, 0);
  delay(1000);
    digitalWrite(12, 1);  
  digitalWrite(13, 1);
}
