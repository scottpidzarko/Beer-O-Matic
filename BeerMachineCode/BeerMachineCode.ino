//#include <pcmRF.h>
//#include <TMRpcm.h>

#include <SD.h>

//TMRpcm tmrpcm;



const int chipSelect = 53;
const int name_offset = 11;
const int beer_offset = 22;
const int lifetime_offset = 26;
const int password_offset = 4;
const int sd_power = 48;

//********Keypad Setup********
#include <Keypad.h>
const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {
  3, 8, 7, 5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {
  4, 2, 6,9}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
char key;
//********Finished Keypad Setup********


//global variable declarations//
int tempNum = 0;
int counter = 0;
File accountData;
File backupData;
File leaderBoard;
long filePosition = 0;
long accountPosition = 0;
int tempBeerNum = 0;
int beerNum = 0;
int lifetime = 0;
char accountChar[4];
char passChar[7];
char beersChar[4];
char accountName[11] = "          ";
char emptyAccount[11] = "NO ACCOUNT";
String leaderNames[] = {"          ","          ","          ","          ","          "};
int leaderBeers[5];
char tempChar;
char* errorMsgs[] = {"SD Card Not Initialized","Keypad Not Initialized","Unknown Error Occurred","Could Not Open Account File"};
char* adminMenu[] = {"Press 1 To Add","Beer To Account","Press 2 To Set","Beer On Account","Press 3 To Edit","Account Info","Press 4 To Reset","SPD Leaderboard","Press 5 To Set","Beer God PW","Press 6 For Beer","God Star Mode","Press 7 To Back","Up Data File","Press 8 To Set","New Custom Msg","Press 9 To Reset","An Account"};
char* optionsMenu[] = {"Press 1 For","Account Info","Press 2 For","User Directory","Press 3 For","Leaderboard","Press 4 For","Star Mode","Press 5 To","Change Password","Press 6 To","Transfer Beers","Press 9 For","Machine Admin"};
char* editScreen[] = {"Enter Name & PW ","Use * For Back  ","Use 0 For Next  ","Press # To Set  "};
char customMessage[] ={"                                "};
char keyLetters [][8] = {
                        {'a','b','c','1','A','B','C','!'},
                        {'d','e','f','2','D','E','F','@'},
                        {'g','h','i','3','G','H','I','#'},
                        {'j','k','l','4','J','K','L','$'},
                        {'m','n','o','5','M','N','O','%'},
                        {'p','q','r','6','P','Q','R','^'},
                        {'s','t','u','7','S','T','U','&'},
                        {'v','w','x','8','V','W','X','*'},
                        {'y','z','9','Y','Z','-','?','+'}
                      };      
unsigned long timer=0;
int accountNum = 0;
long accountPass = 0;
long tempPass = 0;
int errorNum = 4;
int startup = 0;
int match = 0;
int beersToAdd = 0;
int lcdPosition = 128;



void setup()

{
  //tmrpcm.speakerPin = 11;
  pinMode(42,OUTPUT);
  digitalWrite(42,HIGH);
  pinMode(40,OUTPUT);
  digitalWrite(40,HIGH);
  pinMode(sd_power,OUTPUT);
  digitalWrite(sd_power,HIGH);
  Serial.begin(115200);
  Serial.println(F("Created Serial Connection!"));
  Serial3.begin(9600);
  clearScreen();
  turnOffCursors();
  Serial3.write("Set Up LCD Link!");
  //Set backlight to highest
  Serial3.write(0x7C);
  Serial3.write(157);
  errorNum = initAndCheckDevices();
  if (errorNum==4)
  {
    setToSecondLine();
    Serial3.print(F("Beer-O-Matic v13"));
    delay(1000);
  }
  else
  {
    displayErrorMsg();
  }
}

void loop()
{
  clearScreen();
  if(!accountData) 
  {
    accountData = SD.open("BeerData.dat", FILE_WRITE);
    accountData.seek(0);
  }
  if(!leaderBoard)
  {
    leaderBoard = SD.open("BeerLead.dat", FILE_WRITE);
    leaderBoard.seek(0);
  }
  if (startup == 0)
  {
    Serial.println(F("Ready For Use!"));
    clearScreen();
    Serial3.print(F("Ready For Use!"));
    startup = 1;
  }
  displayHomeMenu();
  if (key=='*')
  {
    displayOptionsMenu();
  }
  else if(key != '#')
  {
    if(validateAccount())
    {
      if(easterEggs()){}
      else
      {
        dispenseBeer();
        updateStatistics();
      }
    }
  }
  accountData.close();
  leaderBoard.close();
}

int initAndCheckDevices()
{  
  //Turn on and check SD Card//
  Serial.println(F("Initializing SD Card..."));
  if (!SD.begin(chipSelect))
  {
    Serial.println(F("SD Card Initialization Failed!"));
    return 0;
  }
  Serial.println(F("SD Card Initialization Complete!"));
  //Finished starting up SD Card//
  
  //Opening account data file//
  Serial.println(F("Opening Data Files..."));
  accountData = SD.open("BeerData.dat", FILE_WRITE);
  leaderBoard = SD.open("BeerLead.dat", FILE_WRITE);
  if (!accountData)
  {
    return 3;
  }
  else 
  {
    Serial.println(F("Account File Opened Successfully!"));
    accountData.seek(0);
    while(accountData.available())
    {
      Serial.write(accountData.read());
    }
    Serial.println();
    Serial.println(F("Finished reading file. Resetting to position 0 within file"));
    accountData.seek(0);
    
    if(leaderBoard)
    {
      Serial.println();
      Serial.println(F("Displaying Leaderboard!"));
      leaderBoard.seek(0);
      while(leaderBoard.available())
      {
        Serial.write(leaderBoard.read());
      }
    }
  }
  return 4;
}

void displayErrorMsg()
{
  Serial.println(errorMsgs[errorNum]);
  while(1){}
}

void displayHomeMenu()
{
  getCustomMessage();
  clearScreen();
  key = keypad.getKey();
  while(!key)
  {
    timer = millis();
    clearScreen();
    displayCustomMessage();
    while(millis() < timer + 3000 && !key)
    {
      key=keypad.getKey();
    }
    if (!key)
    {
      for(int i = 0; i<3; i++)
      {
        if(!key)
        {
          timer = millis();
          clearScreen();
          Serial3.print(F("Enter Account #"));
          setToSecondLine();
          Serial3.print(F("And PW For Beer"));
          Serial.println(F("Enter Account #"));
          Serial.println(F("And PW For Beer"));
          while(millis() < timer + 1500 && !key)
          {
            key=keypad.getKey();
          }
        }
        if(!key)
        {
          clearScreen();
          Serial3.print(F("Or Press * Key"));
          setToSecondLine();
          Serial3.print(F("For More Options"));
          Serial.println(F("Or Press * Key"));
          Serial.println(F("For More Options"));
          timer = millis();
          while(millis() < timer + 1500 && !key)
          {
            key=keypad.getKey();
          }
        }
      }
    }
  }
}

void displayOptionsMenu()
{
  key = keypad.getKey();
  while(!key)
  {
    for (int i = 0; i<14; i++)
    {
      timer = millis();
      clearScreen();
      Serial3.print(optionsMenu[i]);
      Serial.println(optionsMenu[i]);
      i++;
      setToSecondLine();
      Serial3.print(optionsMenu[i]);
      Serial.println(optionsMenu[i]);
      while (millis() < timer + 1500 && !key)
      {
        key=keypad.getKey();
      }
      if((int)key != 0) break;
    }
  }
  if(key == '1')
  {
    key=keypad.getKey();
    getAccount();
    if(findAccount()==1)
    {
      displayInfoPage();
    }
    else 
    {
      noAccountFound();
    }
  }
  else if(key == '2')
  {
    key=keypad.getKey();
    displayDirectory();
  }
  else if(key == '3')
  {
    key=keypad.getKey();
    getBeerLeaders();
  }
  else if(key == '4')
  {
    key=keypad.getKey();
    enterStarMode();
  }
  else if(key == '5')
  {
    key = keypad.getKey();
    if(validateAccount()) changePassword();
  }
  else if(key == '6')
  {
    key=keypad.getKey();
    transferBalance();
  }
  else if(key == '9')
  {
    key=keypad.getKey();
    if (checkGodPW())
    {
      displayAdminMenu();
    }
  }
}

int validateAccount()
{
  getAccount();
  match = findAccount();
  if(match == 1)
  {
    getPrivatePassword();
    if(checkPassword())
    {
      return 1;
    }
    else return 0;
  }
  else if(match == 2)
  {
    noAccountFound();
  }
  else 
  {
    noAccountFound();
  }
  return 0;
}

void dispenseBeer()
{
  getBeerNum();
  if(beerNum > 0)
  {
    subtractBeer();
    increaseLifetime();
    releaseBeer();
    clearScreen();
    Serial3.print(F("Beers Left: "));
    Serial3.print(beerNum);
    setToSecondLine();
    Serial.print(F("Beers Left: "));
    Serial.println(beerNum);
    Serial3.print(F("Lifetime #: "));
    Serial3.print(lifetime);
    Serial.print(F("Lifetime #: "));
    Serial.println(lifetime);
    accountData.seek(0);
    delay(1500);
  }
  else
  {
    clearScreen();
    Serial3.print(F("Out Of Beer Bro!"));
    setToSecondLine();
    Serial3.print(F("No Beer For You!"));
    Serial.println(F("Out Of Beer Bro!"));
    delay(1500);
    accountData.seek(0);
  }
}

void displayInfoPage()
{
  accountData.seek(accountPosition+name_offset);
  setToSecondLine();
  Serial3.print(F("User: "));
  Serial.print(F("User: "));
  for (int i = 0; i<10; i++)
  {
    tempChar = accountData.read();
    Serial3.write(tempChar);
    Serial.write(tempChar);
  }
  delay(1500);
  clearScreen();
  getBeerNum();
  Serial.println();
  Serial3.print(F("Beers Left: "));
  Serial.print(F("Beers Left: "));
  Serial3.print(beerNum);
  Serial.print(beerNum);
  setToSecondLine();
  getLifetime();
  Serial.println();
  Serial3.print(F("Lifetime #: "));
  Serial.print(F("Lifetime #: "));
  Serial3.print(lifetime);
  Serial.print(lifetime);
  delay(1500);
  accountData.seek(0);
  Serial.println();
}

int checkGodPW()
{
  clearScreen();
  Serial3.print(F("U BEER GOD BRO??"));
  getPrivatePassword();
  accountData.seek(0);
  while(accountData.read()!=58){}
  accountData.read();
  match = 0;
  for (int i = 0; i<6; i++)
  {
    if (accountData.read() == passChar[i])
    {
      match++;
    }
  }
  if (match == 6)
  {
    Serial.println(F("Correct PW!"));
    return 1;
  }
  else 
  {
    Serial.println(F("Incorrect PW!"));
    return 0;
  }
}

void displayAdminMenu()  
{
  Serial.println(F("Admin Menu:"));
  key=keypad.getKey();
  while(!key)
  {
    for (int i = 0; i<18; i++)
    {
      clearScreen();
      timer = millis();
      Serial3.print(adminMenu[i]);
      Serial.println(adminMenu[i]);
      i++;
      setToSecondLine();
      Serial3.print(adminMenu[i]);
      Serial.println(adminMenu[i]);
      while (millis() < timer + 1500 && !key)
      {
        key=keypad.getKey();
      }
      if((int)key != 0)
      {
        break;
      }
    }
  }
  if(key == '1')
  {
    key=keypad.getKey();
    addBeerToAccount();
  }
  else if(key == '2')
  {
    key=keypad.getKey();
    setBeersOnAccount();
  }
  else if(key == '3')
  {
    key=keypad.getKey();
    editAccountInfo();
  }
  else if(key == '4')
  {
    key=keypad.getKey();
    resetStatistics();
  }
  else if(key == '5')
  {
    key=keypad.getKey();
    setBeerGodPW();
  }
  else if(key == '6')
  {
    key=keypad.getKey();
    enterGodStarMode();
  }
  else if(key == '7')
  {
    key=keypad.getKey();
    backupDataFile();
  }
  else if(key == '8')
  {
    key=keypad.getKey();
    setCustomMessage();
  }
  else if(key == '9')
  {
    key=keypad.getKey();
    resetAccount();
  }
}

void setBeerGodPW()
{
  clearScreen();
  Serial3.print(F("SET NEW GOD PW!?"));
  setToSecondLine();
  accountData.seek(0);
  while((int)accountData.read() != 58) {}
  accountData.read();
  filePosition = accountData.position();
  
  Serial.println(F("Enter New PW:"));
  lcdPosition += Serial3.print(F("Password: "));
  for(int i = 0; i<6; i++)
  {
    key = keypad.getKey();
    while(!key) key=keypad.getKey();
    if(key == '*') 
    {
      if(i > 0) 
      {
        backSpace();
        accountData.seek(accountData.position()-1);
        i-=2;
      }
      else i--;
    }
    else
    {
      accountData.write(key);
      lcdPosition += Serial3.print(F("*"));
      Serial.print(F("*"));    
    }
  }
  Serial.println();
  accountData.seek(0);
  clearScreen();
  Serial3.print(F("  HALLELUJAH!!  "));
  setToSecondLine();
  Serial3.print(F("GOD PW CHANGED!!"));
  Serial.println(F("God PW Changed!"));  
  delay(1500);
}

void addBeerToAccount()
{
  getAccount();
  match = findAccount();
  if(match == 1)
  {
    Serial.println();
    accountData.seek(accountPosition+name_offset);
    clearScreen();
    Serial3.print(F("User: "));
    Serial.print(F("User: "));
    for(int i = 0; i<10; i++)
    {
      tempChar = accountData.read();
      Serial.write(tempChar);
      Serial3.write(tempChar);
    }
    setToSecondLine();
    getBeerNum();
    Serial.println();
    Serial3.print(F("Beer On Tab: "));
    Serial3.print(beerNum);
    Serial.print(F("Beers: "));
    Serial.println(beerNum);
    delay(1500);
    clearScreen();
    Serial3.print(F("Beer On Tab: "));
    Serial3.print(beerNum);
    setToSecondLine();
    lcdPosition += Serial3.print(F("Beer To Add: "));
    Serial.print(F("Beers to add: "));
    tempBeerNum = 0;
    for (int i = 0; i<3; i++)
    {
      key = keypad.getKey();
      while(!key)
      {
        key=keypad.getKey();
      }
      if(key)
      {
        if(key == '*')
        {
          if(i != 0) backSpace();
          i-=2;
          if (i<-1) i=-1;
          tempBeerNum -= tempBeerNum%10;
          tempBeerNum /= 10;
          Serial.write('/');
        }
        else 
        {
          Serial3.write(key);
          lcdPosition++;
          Serial.write(key);
          tempBeerNum*=10;
          tempBeerNum+=(int)key-48;
        }
      }
    }
    Serial.println();
    if(tempBeerNum > 0)
    {
      beerNum += tempBeerNum;
      clearScreen();
      if (beerNum > 999) 
      {
        Serial3.print(F("Too Many Beers!"));
        Serial.println(F("Too Many Beers!"));
      }
      else {
        Serial3.print(F("New Balance: "));
        Serial3.print(beerNum); 
        Serial.print(F("New Balance: "));
        Serial.println(beerNum);
        setBeerNum();
      }
      accountData.seek(0);
      delay(1500);
    }
  }
  else
  {
    noAccountFound();
  }
}

int findAccount()
{
  match = 0;
  accountData.seek(0);
  while(accountData.available())
  {
    if((int)accountData.read() == 10)
    {
      match = 0;
      for(int i = 0; i<3; i++)
      {
        if(accountData.read() == accountChar[i])
        {
          match++;
        }
      }
      if (match == 3)
      {
        accountPosition = accountData.position() - 3;
        match = 0;
        accountData.seek(accountPosition+name_offset);
        for (int i = 0; i<10; i++)
        {
          if(accountData.read() == emptyAccount[i])
          {
            match++;
          }
        }
        accountData.seek(accountPosition);
        if (match == 10)
        {
          Serial.println(F("Empty Account!"));
          return 2;
        }
        else return 1;
      }
    }
  }
  Serial.println(F("No Account Found"));
  return 0;
}

int checkPassword()
{
  accountData.seek(accountPosition + password_offset);
  match = 0;
  for (int i = 0; i<6; i++)
  {
    if(accountData.read() == passChar[i])
    {
      match++;
    }
  }
  if (match == 6)
  {
    accountData.seek(accountPosition);
    return 1;
  }
  else 
  {
    clearScreen();
    Serial.println(F("Wrong Password"));
    Serial3.print(F("WRONG PASSWORD"));
    setToSecondLine();
    Serial3.print(F("TRY HARDER BRO"));
    delay(1500);
    return 0;
  }
}

void getPrivatePassword()
{
  Serial.print(F("Password: "));
  setToSecondLine();
  lcdPosition+=Serial3.print(F("Password: "));
  turnOnCursor();
  accountPass = 0;
  if(!key) key = keypad.getKey();
  for (int i = 0; i<6; i++)
  {
    key=keypad.getKey();
    while(!key)
    {
      key = keypad.getKey();
    }
    if(key)
    {
      if(key == '*')
      {
        i-=2;
        if(i >= -1) backSpace();
        if (i<-1) i=-1;
        accountPass -= accountPass%10;
        accountPass /= 10;
        Serial.write('/');
      }
      else 
      {
        passChar[i] = key;
        lcdPosition += Serial3.write('*');
        Serial.write(key);
        accountPass *= 10;
        accountPass += (int)key - 48; 
      }
    }
  }
  turnOffCursors();
  Serial.println();
}


void getAccount()
{
  Serial.print(F("Account #: "));
  clearScreen();
  lcdPosition += (int)Serial3.print(F("Account #: "));
  accountNum = 0;
  turnOnCursor();
  for (int i = 0; i<3; i++)
  {
    while(!key)
    {
      key = keypad.getKey();
    }
    if(key)
    {
      if(key == '*')
      {
        i-=2;
        if(i >= -1) backSpace();
        if (i<-1) i=-1;
        accountNum -= accountNum%10;
        accountNum /= 10;
        Serial.print('/');
      }
      else
      {
        accountChar[i] = key;
        lcdPosition += Serial3.write(key);
        Serial.write(key);
        accountNum *= 10;
        accountNum += (int)key - 48; 
      }
    }
    key = keypad.getKey();
  }
  turnOffCursors();
  Serial.println();
}

void editAccountInfo()
{
  int j = 0;
  tempNum = 0;
  getAccount();
  if (findAccount() == 0)
  {
    setToSecondLine();
    Serial3.print(F("NO ACCOUNT BRO!"));
    delay(1500);
    Serial.println(F("Invalid Account"));
    return;
  }
  else
  {
    accountData.seek(accountPosition + 11);
    setToSecondLine();
    Serial3.print(F("User: "));
    Serial.print(F("User: "));
    for(int i=0; i<10; i++)
    {
      tempChar = accountData.read();
      Serial3.write(tempChar);
      Serial.write(tempChar);
    }
    Serial.println();
    accountData.seek(accountPosition + 11);
    delay(1500);
    Serial.println(F("Enter Name & PW"));
    Serial.println(F("Use * For Back"));
    Serial.println(F("Use 0 For Next"));    
    Serial.println(F("Press # To Set"));
    timer = millis();
    counter = 0;
    clearScreen();
    Serial3.print(editScreen[j]);
    setToSecondLine();
    lcdPosition+=Serial3.print(F("User: "));
    tempChar = ' ';
    match = 1;
    turnOnCursor();
    while(key != '#')
    {
      if(millis() > timer + 1500)
      {
        setToFirstLine();
        Serial3.print(editScreen[match]);
        timer = millis();
        match++;
        if(match == 4) 
        {
          match = 0; 
        }
        setToSecondLine();
        lcdPosition+=Serial3.print(F("User: "))+j;
        lcdGoTo();
      }
      if(key)
      {
        if (key == '*')
        {
          lcdPosition++;
          backSpace();
          tempChar = ' ';
          accountName[j] = tempChar;
          if(j!= 0 && counter == 0)
          { 
            backSpace();
            j--; 
            accountName[j] = tempChar;
          }
          counter = 0;
          if(j <0) j=0;
          Serial.println('/');
        }
        else if (key == '0')
        {
          accountName[j] = tempChar;
          tempChar = ' ';
          j++;
          counter = 0;
          if(j<10) 
          {
            lcdPosition++;
            lcdGoTo();
          }
          if(j == 10) j=9;
          Serial.println();
        }
        else if (key == '#'){}
        else
        {
          if(tempNum != (int)key-49) counter=0;
          tempNum = (int)key-49;
          tempChar = keyLetters[tempNum][counter];
          Serial3.write(tempChar);
          lcdGoTo();
          Serial.write(tempChar);
          counter++;
          if(counter >= 8)
          {
            counter = 0;
          }
        }
      }
      key = keypad.getKey();
    }
    turnOffCursors();
    if(j!=0)
    {
      if (tempChar != ' ')
      {
        accountName[j] = tempChar;
        j++;
      }
      else if(accountName[j] != ' ') j++;
      while(j < 10)
      {
        accountName[j] = ' ';
        j++;
      }
      Serial.println();
      while(!setNewPassword()) {}
      clearScreen();
      Serial3.print(F("Account Info Set"));
      setToSecondLine();
      Serial3.print(F("Successfully!"));
      delay(1500);
    }
  }
  accountData.seek(0);
  accountPosition = 0; 
}

void resetStatistics()
{
  if(leaderBoard)
  {
    leaderBoard.close();
  }
 SD.remove("BeerLead.dat");
 leaderBoard = SD.open("BeerLead.dat", FILE_WRITE);
 for(int i = 0; i<5; i++)
 {
   leaderNames[i] = "          ";
   leaderBeers[i] = 0;
 }
 Serial.println(F("Stats Reset!"));
 clearScreen();
 Serial3.print(F("Stats Reset!"));
 delay(1500);
 leaderBoard.seek(0);
}

void updateStatistics()
{
  if(!leaderBoard)
  {
    leaderBoard = SD.open("BeerLead.dat", FILE_WRITE);
  }
  tempNum = 0;
  beerNum = 0;
  accountData.seek(accountPosition+name_offset);
  leaderBoard.seek(0);
  while(leaderBoard.available()&&tempNum==0)
  {
    while((char)leaderBoard.read()!= '*' && leaderBoard.available()) {}
    accountData.seek(accountPosition+name_offset);
    match = 0;
    for (int i = 0; i<10; i++)
    {
      if (leaderBoard.read() == accountData.read())
      {
        match++;
      }
    }
    if(match == 10)
    {
      leaderBoard.read();
      beerNum = 0;
      for (int i = 0; i<3; i++)
      {
        beerNum*=10;
        beerNum+=(int)leaderBoard.read()-48;
      }
      beerNum++;
      leaderBoard.seek(leaderBoard.position()-3);
      for (int i = 100; beerNum/i == 0; i/=10)
      {
        leaderBoard.print(F("0"));
      }
      leaderBoard.print(beerNum);
      tempNum++;
    }
  }
  if (tempNum == 0)
  {
    leaderBoard.println();
    leaderBoard.print("*");
    accountData.seek(accountPosition+name_offset);
    for(int i = 0; i<10; i++)
    {
      leaderBoard.write(accountData.read());
    }
    leaderBoard.print(":001");
  }
  leaderBoard.close();
}

void getBeerLeaders()
{
  Serial.println(F("Current Leaders:"));
  clearScreen();
  Serial3.print(F("Current Leaders:"));
  setToSecondLine();
  leaderBoard.seek(0);
  for (int i = 0; i<5; i++)
  {
    leaderBeers[i]=0;
  }
  while(leaderBoard.available())
  {
    counter = 0;
    beerNum = 0;
    while(leaderBoard.available() && (char)leaderBoard.read()!='*') {}
    for(int i = 0; i<10; i++)
    {
      accountName[i] = leaderBoard.read();
    }
    leaderBoard.read();
    for (int i = 0; i<3; i++)
    {
      beerNum*=10;
      beerNum+=(int)leaderBoard.read()-48;
    }
    for(int i=0; i<5; i++)
    {
      if(beerNum >= leaderBeers[i])
      {
        counter++;
      }
    }
    if (counter == 1)
    {
      leaderNames[4] = accountName;
      leaderBeers[4] = beerNum;
    }  
    else if(counter > 1)
    {
      shiftList();
    }
  }
  counter = 0;
  for(int i = 0; i<5; i++)
  {
    if(leaderBeers[i] > 0)
    {
      if(counter == 1) 
      {
        clearScreen();
        Serial3.print(leaderNames[i-1]); Serial3.print(F(": ")); Serial3.print(leaderBeers[i-1]);
        setToSecondLine();
      }
      else counter = 1;
      Serial3.print(leaderNames[i]); Serial3.print(F(": ")); Serial3.print(leaderBeers[i]);
      Serial.print(leaderNames[i]); Serial.print(": "); Serial.println(leaderBeers[i]);
      delay(1000);
    }
  }   
  if(leaderBeers[0] == 0)
  {
    clearScreen();
    Serial3.print(F("NO BEER LEADERS!"));
    setToSecondLine();
    Serial3.print(F("DRINK MORE BRO!!"));
    delay(1500);
  }
  else delay(1000);  
  getAllTimeLeaders();
}

void getAllTimeLeaders(){
  clearScreen();
  Serial.println(F("All-Time Leaders"));
  clearScreen();
  Serial3.print(F("All-Time Leaders"));
  setToSecondLine();
  beerNum = 0;
}
  


void shiftList()
{
  for(int i=4; i>5-counter; i--)
  {
    leaderNames[i] = leaderNames[i-1];
    leaderBeers[i] = leaderBeers[i-1];
  }
  leaderNames[5-counter]=accountName;
  leaderBeers[5-counter]=beerNum;
}

void displayDirectory()
{
  Serial.println();
  clearScreen();
  Serial3.print(F("User Directory: "));
  setToSecondLine();
  Serial.println(F("User Directory: "));
  accountData.seek(0);
  counter = 1;
  key = keypad.getKey();
  while(accountData.available() && ( key != '*' && key != '#'))
  {
    match = 0;
    while(accountData.available() && (int)accountData.read()!=10){}
    accountPosition=accountData.position();
    for (int i =0; i<3; i++)
    {
      tempNum = (int)accountData.read();
      if(tempNum < 48 || tempNum > 57) match = 10;
    }
    accountData.seek(accountPosition+name_offset);
    for(int i = 0; i<10; i++)
    {
      accountName[i] = accountData.read();
      if (accountName[i] == emptyAccount[i]) match++;
      if ((int)accountName[i] < 32) match = 10;
    }
    if(match >= 10)
    {
      accountData.seek(accountPosition);
    }
    else
    {
      accountData.seek(accountPosition);
      for (int i = 0; i<3; i++)
      {
        tempChar = accountData.read();
        Serial3.write(tempChar);
        Serial.write(tempChar);
      }
      Serial3.print(": ");
      Serial.print(": ");
      accountData.seek(accountPosition+name_offset);
      for (int i = 0; i<10; i++)
      {
        tempChar = accountData.read();
        Serial3.write(tempChar);
        Serial.write(tempChar);
      }
      Serial.println();
      if(counter == 1) 
      {
        counter = 0;
        timer = millis();
        while(millis() < timer + 1000)
        {
          key = keypad.getKey();
          if (key == '*' || key == '#') timer = 0;
        }
        clearScreen();
        accountData.seek(accountPosition - 1);
      }
      else
      {
        counter = 1;
        setToSecondLine();
      }        
    }
    if (!key) key = keypad.getKey();
  }
}

void enterStarMode()
{
  if(validateAccount())
  {
    key=keypad.getKey();
    getBeerNum();
    tempBeerNum = 0;
    counter = 0;
    clearScreen();
    Serial3.print(F("Beer Left: "));
    Serial.print(F("Beer Left: "));
    Serial3.print(beerNum);
    setToSecondLine();
    Serial.println(beerNum);
    if(beerNum>0)
    {
      Serial.println(F("Set Beer Cap"));
      delay(1500);
      clearScreen();
      Serial3.print(F("Enter 0 For Max "));
      setToSecondLine();
      lcdPosition+=Serial3.print(F("Beer Cap: "));
      Serial.println(F("Enter 0 For Max")); 
      Serial.println(F("Hit # To Enter"));
      timer = millis();
      while(key != '#')
      {
        if(millis() > timer + 3000)
        {
          setToFirstLine();
          Serial3.print(F("Enter 0 For Max"));
          setToSecondLine(); 
          lcdPosition+=Serial3.print(F("Beer Cap: "));
          lcdPosition+=counter;
          if(counter == 3) lcdPosition--;
          lcdGoTo();
          timer = millis();
        }
        else if(millis() > timer + 1500)
        {
          setToFirstLine();
          Serial3.print(F("Hit # To Select"));
          setToSecondLine(); 
          lcdPosition+=Serial3.print(F("Beer Cap: "));
          lcdPosition+=counter;
          if(counter == 3) lcdPosition--;
          lcdGoTo();
        }
        if (key)
        {
          if(key == '*')
          {
            if(counter > 0 && counter < 3) 
            {
              backSpace();
              counter--;
            }
            else if(counter == 3)
            {
              lcdPosition++;
              backSpace();
              counter--;
            }     
            tempBeerNum -= tempBeerNum%10;
            tempBeerNum /= 10;       
          }
          else if(key == '#'){}
          else
          {
            if(counter < 3)
            {
              tempBeerNum *= 10;
              tempBeerNum += (int)key - 48;
              counter++;
              lcdPosition += Serial3.write(key);
            }
            else if(counter == 3)
            {
              tempBeerNum -= tempBeerNum%10;
              tempBeerNum += (int)key - 48;
              Serial3.write(key);
              lcdGoTo();
            }
          }
        }
        key = keypad.getKey();          
      }
      key = keypad.getKey();
      if(tempBeerNum > beerNum)
      {
        clearScreen();
        Serial3.print(F("CAP TOO HIGH BRO"));
        setToSecondLine();
        Serial.println(F("Cap Set Too High"));
        delay(1500);
      }
      else if(tempBeerNum == 0)
      {
        counter = 0;
        timer = millis();
        clearScreen();
        Serial3.print(beerNum);
        Serial3.print(F(" Beers Left!"));
        setToSecondLine();
        Serial3.print(F("Press * To Serve"));
        
        Serial.print(beerNum);
        Serial.println(F(" Beer Left!"));
        Serial.println(F("Press * To Serve"));
        Serial.println(F("Press # To End"));
        while(beerNum > 0 & key != '#')
        {
          while(key != '#')
          {
            if(millis() > timer + 1500 && counter == 0)
            {
              setToSecondLine();
              Serial3.print(F("Press # To Quit "));
              counter=1;
            }
            else if(millis() > timer + 3000 && counter == 1)
            {
              setToSecondLine();
              Serial3.print(F("Press * To Serve"));
              counter=0;
              timer = millis();
            }
            if(key == '*')
            {
              dispenseBeer();
              clearScreen();
              Serial3.print(beerNum);
              Serial3.print(F(" Beer Left!"));
              setToSecondLine();
              if(counter == 0) Serial3.print(F("Press * To Serve"));
              else Serial3.print(F("Press # To Quit"));
            
              Serial.println(F("Press * To Serve"));
              Serial.println(F("Press # To End"));
            }
            key = keypad.getKey();
          }
        }
        key = keypad.getKey();
      }
      else
      {
        counter = 0;
        timer = millis();
        clearScreen();
        Serial3.print(tempBeerNum);
        Serial3.print(F(" Beer Left!"));
        setToSecondLine();
        Serial3.print(F("Press * To Serve"));
        
        Serial.print(tempBeerNum);
        Serial.println(F(" Beer Left!"));
        Serial.println(F("Press * To Serve"));
        Serial.println(F("Press # To End"));
        while(key != '#' && tempBeerNum > 0)
        {
          if(millis() > timer + 1500 && counter == 0)
          {
            setToSecondLine();
            Serial3.print(F("Press # To Quit "));
            counter=1;
          }
          else if(millis() > timer + 3000 && counter == 1)
          {
            setToSecondLine();
            Serial3.print(F("Press * To Serve"));
            counter=0;
            timer = millis();
          }
          if(key)
          {
            if(key == '*')
            {
              tempBeerNum--;
              releaseBeer();
              subtractBeer();
              increaseLifetime();
              updateStatistics();
              clearScreen();
              Serial3.print(tempBeerNum);
              Serial3.print(F(" Beer Left!"));
              setToSecondLine();
              if(counter == 0) Serial3.print(F("Press * To Serve"));
              else Serial3.print(F("Press # To Quit"));
              Serial.print(tempBeerNum);
              Serial.println(F(" Beers Left!"));
              Serial.println(F("Press * To Serve"));
              Serial.println(F("Press # To End"));
            }
          }
          key = keypad.getKey();
        }
      }
    }
  }
}
        
              
          

void enterGodStarMode()
{
  clearScreen();
  Serial3.print(F(" Welcome To God "));
  setToSecondLine();
  Serial3.print(F("   Star Mode!   "));
  Serial.println(F("Welcome to God"));
  Serial.println(F("Star Mode!"));
  delay(1500);
  clearScreen();
  Serial3.print(F("Press * To Serve"));
  setToSecondLine();
  Serial3.print(F("Press # To End"));
  Serial.println(F("Press * To Serve"));
  Serial.println(F("Press # To End"));
  timer = millis();
  counter = 0;
  while(key != '#')
  {
    if(millis() > timer + 3000 && counter == 1)
    {
      clearScreen();
      Serial3.print(F("Press * To Serve"));
      setToSecondLine();
      Serial3.print(F("Press # To End"));
      timer = millis();
      counter = 0;
    }
    if(millis() > timer + 1500 && counter == 0)
    {
      clearScreen();
      Serial3.print(F(" Welcome To God "));
      setToSecondLine();
      Serial3.print(F("   Star Mode!   "));
      counter = 1;
    }      
    if(key)
    {
      if(key == '#') {}
      else if(key == '*')
      {
        releaseBeer();
      }
    }
    key = keypad.getKey();
  }    
}

void setBeersOnAccount()
{
  getAccount();
  match = findAccount();
  beerNum = 0;
  if(match == 1)
  {
    clearScreen();
    Serial.println();
    accountData.seek(accountPosition+name_offset);
    Serial3.print(F("User: "));
    Serial.print(F("User: "));
    for(int i = 0; i<10; i++)
    {
      tempChar = accountData.read();
      Serial3.write(tempChar);
      Serial.write(tempChar);
    }
    accountData.read();
    for (int i = 0; i<3; i++)
    {
      beerNum*=10;
      beerNum+=(int)accountData.read()-48;
    }
    Serial.println();
    setToSecondLine();
    Serial3.print(F("Beer On Tab: "));
    Serial3.print(beerNum);
    Serial.print(F("Beers: "));
    Serial.println(beerNum);
    delay(1500);
    clearScreen();
    Serial3.print(F("Beer On Tab: "));
    Serial3.print(beerNum);
    setToSecondLine();
    lcdPosition += Serial3.print(F("Set New Tab: "));
    Serial.print(F("Beers to set: "));
    beerNum = 0;
    for (int i = 0; i<3; i++)
    {
      key = keypad.getKey();
      while(!key)
      {
        key = keypad.getKey();
      }
      if(key)
      {
        if(key == '*')
        {
          if(i != 0) backSpace();
          i-=2;
          if (i<-1) i=-1;
          beerNum -= beerNum%10;
          beerNum /= 10;
          Serial.write('/');
        }
        else if(key == '#')
        {
          i--;
          key=keypad.getKey();
        }
        else 
        {
          lcdPosition += Serial3.write(key);
          Serial.write(key);
          beerNum*=10;
          beerNum+=(int)key-48;
        }
      }
    }
    clearScreen();
    Serial3.print(F("New Balance: "));
    Serial3.print(beerNum);
    Serial.println();
    Serial.print(F("New Balance: "));
    Serial.println(beerNum);
    setBeerNum();
  }
  else
  {
    noAccountFound();
    accountData.seek(0);
  }
}

void releaseBeer()
{
  Serial.println(F("Dispensing Beer!"));
  timer = millis();
  //relay is active low for can machine
  //First relay bridges pins 1 and 3 on the Jones Plug
  digitalWrite(40,LOW);
  while(millis() < timer+500){}
  digitalWrite(40,HIGH);
  while(millis() < timer+1000){}
  //Second relay bridges pins 1 and 7 on the Jones Plug
  digitalWrite(42,LOW);
  while(millis() < timer+1500){}
  digitalWrite(42,HIGH);
}

void clearScreen()
{
  Serial3.write(254);
  Serial3.write(1);
  setToFirstLine();
}

void setToFirstLine()
{
  Serial3.write(254);
  Serial3.write(128);
  lcdPosition = 128;
}

void setToSecondLine()
{
  Serial3.write(254);
  Serial3.write(192);
  lcdPosition = 192;
}

void lcdGoTo()
{
  Serial3.write(254);
  Serial3.write(lcdPosition);
}

void backSpace()
{
  if(lcdPosition == 128 || lcdPosition == 192){}
  else
  {
    lcdPosition--;
    Serial3.write(254);
    Serial3.write(lcdPosition);
    Serial3.write(' ');
    Serial3.write(254);
    Serial3.write(lcdPosition);
  }
}

void turnOffCursors()
{
  Serial3.write(254);
  Serial3.write(12);
}

void turnOnCursor()
{
  Serial3.write(254);
  Serial3.write(14);
}

void backupDataFile()
{
  backupData = SD.open("Backup.dat", FILE_WRITE);
  if(!backupDataFile)
  {
    clearScreen();
    Serial3.print(F("Couldn't Open"));
    setToSecondLine();
    Serial3.print(F("Backup File :("));
    delay(1500);
  }
  else
  {
    clearScreen();
    Serial3.print(F("Backing Up Data"));
    setToSecondLine();
    accountData.seek(0);
    backupData.seek(0);
    timer = millis();
    counter = 0;
    tempNum = accountData.available();
    while(accountData.available()>0 && counter < tempNum)
    {
      backupData.write(accountData.read());
      counter++;
      if(millis() > timer + 1000)
      {
        setToSecondLine();
        Serial3.print(counter); 
        Serial3.write('/');
        Serial3.print(tempNum);
        timer = millis();
      }   
    }
    backupData.close();
    accountData.seek(0);
    clearScreen();
    Serial3.print(F("Backup Complete!"));
    setToSecondLine();
    Serial3.print(F("Cool Story Bro!"));
    delay(1500);
  }
}

void transferBalance()
{
  if(validateAccount())
  {
    filePosition = accountPosition;
    clearScreen();
    getBeerNum();
    tempBeerNum = 0;
    clearScreen();
    Serial3.print(F("Beer Left: "));
    Serial.print(F("Beer Left: "));
    Serial3.print(beerNum);
    Serial.println(beerNum);
    setToSecondLine();
    lcdPosition += Serial3.print(F("# To Send: "));
    Serial.print(F("# Of Beer To Send: "));
    for (int i = 0; i<3; i++)
    {
      key = keypad.getKey();
      while(!key)
      {
        key=keypad.getKey();
      }
      if(key)
      {
        if(key == '*')
        {
          if(i != 0) backSpace();
          i-=2;
          if (i<-1) i=-1;
          tempBeerNum -= tempBeerNum%10;
          tempBeerNum /= 10;
          Serial.write('/');
        }
        else 
        {
          Serial3.write(key);
          lcdPosition++;
          Serial.write(key);
          tempBeerNum*=10;
          tempBeerNum+=(int)key-48;
        }
      }
    }
    key=keypad.getKey();
    if(tempBeerNum < beerNum)
    {
      clearScreen();
      Serial3.print(F("Send Beers To"));
      setToSecondLine();
      Serial3.print(F("Which Account?"));
      delay(1500);
      getAccount();
      if(findAccount()==1)
      {
        getBeerNum();
        beerNum += tempBeerNum;
        if(beerNum > 999)
        {
          tempBeerNum -= beerNum - 999;
          beerNum = 999;
        }
        setBeerNum();
        accountData.seek(accountPosition + name_offset);
        clearScreen();
        Serial3.print(F("Sent ")); Serial3.print(tempBeerNum); Serial3.print(F(" Beers"));
        setToSecondLine(); Serial3.print(F("To "));
        for(int i = 0; i<10; i++) Serial3.write(accountData.read());
        accountPosition = filePosition;
        getBeerNum();
        beerNum -= tempBeerNum;
        setBeerNum();
        accountData.seek(0);
        delay(1500);
        clearScreen();
      }
      else
      {
        delay(1500);
        clearScreen();
      }
    }
    else
    {
      clearScreen();
      Serial3.print(F("Not Enough Beer"));
      setToSecondLine();
      Serial3.print(F("To Send Broseph!"));
      delay(1500);
    }
  }
}
  
void subtractBeer()
{
  getBeerNum();
  beerNum--;
  setBeerNum();
}

void increaseLifetime()
{
  getLifetime();
  lifetime++;
  setLifetime();
}

void getBeerNum()
{
  beerNum = 0;
  accountData.seek(accountPosition+beer_offset);
  for (int i = 0; i<3; i++)
  {
    beerNum*=10;
    beerNum+=(int)accountData.read()-48;
  }
}

void setBeerNum()
{
  accountData.seek(accountPosition+beer_offset);
  if(beerNum != 0)
  {
    for (int i = 100; beerNum/i == 0; i/=10)
    {
      accountData.print(F("0"));
    }
    accountData.print(beerNum);
  }
  else
  {
    accountData.print(F("000"));
  }
}

void getLifetime()
{
  lifetime = 0;
  accountData.seek(accountPosition+lifetime_offset);
  for (int i = 0; i<4; i++)
  {
    lifetime *= 10;
    lifetime += (int)accountData.read()-48;
  }
}

int setLifetime()
{
  accountData.seek(accountPosition+lifetime_offset);
  for (int i = 1000; lifetime/i == 0; i/=10)
  {
    accountData.print(F("0"));
  }
  accountData.print(lifetime);
}

int setNewPassword()
{
  match = 1;
  accountData.seek(accountPosition+name_offset);
  clearScreen();
  Serial3.print(F("User: "));
  Serial.print(F("User: "));
  for(int i = 0; i<10; i++)
  {
    accountData.write(accountName[i]);
    Serial3.write(accountName[i]);
    Serial.write(accountName[i]);
  }      
  getPrivatePassword();
  tempPass = accountPass;
  clearScreen();
  Serial3.print(F("Re-enter your PW"));
  getPrivatePassword();
  match = (int)(tempPass-accountPass);
  if(match != 0)
  {
    clearScreen();
    Serial3.print(F("PASSWORDS DON'T"));
    setToSecondLine(); 
    Serial3.print(F("MATCH JACKASS!"));
    delay(1500);
    return 0;
  }
  else
  {
    accountData.seek(accountPosition+password_offset);
    for(int i = 0; i<6; i++)
    {
      accountData.write(passChar[i]);
    }
    return 1;
  }
}


void changePassword()
{
  match = 1;
  clearScreen();
  Serial3.print(F("Set New Password"));
  getPrivatePassword();
  tempPass = accountPass;
  clearScreen();
  Serial3.print(F("Re-Type Password"));
  getPrivatePassword();
  match = (int)(tempPass-accountPass);
  if(match != 0)
  {
    clearScreen();
    Serial3.print(F("PASSWORDS DON'T"));
    setToSecondLine(); 
    Serial3.print(F("MATCH JACKASS!"));
    delay(1500);
  }
  else
  {
    accountData.seek(accountPosition+password_offset);
    for(int i = 0; i<6; i++)
    {
      accountData.write(passChar[i]);
    }
    clearScreen();
    Serial3.print(F("COOL NEW"));
    setToSecondLine();
    Serial3.print(F("PASSWORD BRO!"));
    delay(1500);
  }
}

void setCustomMessage()
{
  int j = 0;
  clearScreen();
  Serial3.print(F("Current Custom"));
  setToSecondLine();
  Serial3.print(F("Message Text Is: "));
  delay(1500);
  clearScreen();
  getCustomMessage();
  displayCustomMessage();
  delay(1500);
  clearScreen();
  Serial3.print(F("Type New Message"));
  setToSecondLine();
  Serial3.print(F("* = Back # = Set"));
  delay(3000);
  clearScreen();
  turnOnCursor();
  counter = 0;
  key=keypad.getKey();
  while(key != '#')
  {
    if(key)
    {
      if (key == '*')
      {
        lcdPosition++;
        backSpace();
        tempChar = ' ';
        customMessage[j] = tempChar;
        if(j!= 0 && counter == 0)
        { 
          if(j != 16)
          {
            backSpace();
          }
          else
          {
            lcdPosition = 144;
            backSpace();
          }
          j--; 
          customMessage[j] = tempChar;
        }
        counter = 0;
        if(j <0) j=0;
        Serial.println('/');
      }
      else if (key == '0')
      {
        customMessage[j] = tempChar;
        tempChar = ' ';
        j++;
        counter = 0;
        if(j<16) 
        {
          lcdPosition++;
          lcdGoTo();
        }
        else if(j == 16) setToSecondLine();
        else if (j>16 && j<32)
        {
          lcdPosition++;
          lcdGoTo();
        }        
        else if (j == 32) j=31;    
        Serial.println();
      }
      else if (key == '#'){}
      else
      {
        if(tempNum != (int)key-49) counter=0;
        tempNum = (int)key-49;
        tempChar = keyLetters[tempNum][counter];
        Serial3.write(tempChar);
        lcdGoTo();
        Serial.write(tempChar);
        counter++;
        if(counter >= 8)
        {
          counter = 0;
        }
      }
    }
    key = keypad.getKey();
  }
  turnOffCursors();
  if(j!=0)
  {
    if (tempChar != ' ')
    {
      customMessage[j] = tempChar;
      j++;
    }
    else if(customMessage[j] != ' ') j++;
    while(j < 32)
    {
      customMessage[j] = ' ';
      j++;
    }
    accountData.seek(filePosition);
    for(int i = 0; i<16; i++)
    {
      accountData.write(customMessage[i]);
    }
    counter = 0;
    while(counter < 4)
    {
      if(accountData.read() == '/') counter++;
    }
    for(int i = 16; i<32; i++)
    {
      accountData.write(customMessage[i]);
    }
    clearScreen();
    Serial3.print(F("YOU SET A FANCY"));
    setToSecondLine();
    Serial3.print(F("NEW MESSAGE BRO!"));
    delay(1500);
  }
  accountData.seek(0);
  accountPosition = 0; 
}

void getCustomMessage()
{
  accountData.seek(0);
  while(accountData.available() && accountData.peek() != '/') accountData.read();
  if(accountData.read() == '/') accountData.read();
  filePosition = accountData.position();
  clearScreen();
  for(int i = 0; i<16; i++)
  {
    customMessage[i]=(accountData.read());
  }
  counter = 0;
  while(counter < 4) 
  {
    if (accountData.read() == '/') counter++;
  }
  setToSecondLine();
  for(int i = 16; i<32; i++)
  {
    customMessage[i]=(accountData.read());
  }
}

void displayCustomMessage()
{
  clearScreen();
  for(int i = 0; i<16; i++)
  {
    Serial3.print(customMessage[i]);
  }
  setToSecondLine();
  for(int i = 16; i<32; i++)
  {
    Serial3.print(customMessage[i]);
  }
}

void resetAccount()
{
  getAccount();
  if (findAccount() == 1)
  {
    clearScreen();
    accountData.seek(accountPosition + name_offset);
    Serial3.print(F("Wipe "));
    for(int i = 0; i<10; i++)
    {
      Serial3.write(accountData.read());
    }
    Serial3.write('?');
    setToSecondLine();
    timer = millis();
    counter = 0;
    Serial3.print(F("Press # To Wipe "));
    key = keypad.getKey();
    while(key != '*' && key != '#')
    {
      if(millis() > timer + 1500)
      {
        timer = millis();
        setToSecondLine();
        if(counter == 0)
        {
          Serial3.print(F("Or * To Cancel "));
          counter++;
        }
        else
        {
          Serial3.print(F("Press # To Wipe "));
          counter--;
        }
      }
      key = keypad.getKey();
    }
    if(key == '#')
    {
      accountData.seek(accountPosition + name_offset);
      accountData.print(F("NO ACCOUNT,000,0000"));
    }
    else{}
  }
  else
  {
    noAccountFound();
  }
}

void noAccountFound()
{
  setToSecondLine();
  Serial3.print(F("NO ACCOUNT BRO!"));
  delay(1500);
}

int easterEggs()
{
  randomSeed(millis()/2);
  counter = random(1,1000);
  Serial.println();
  Serial.print(counter);
  if(counter == 666)
  {
    clearScreen();
    //tmrpcm.play("Electric.wav");
    for(int i = 0; i < 32; i++)
    {
      if(i == 16) setToSecondLine();
      timer = millis();
      Serial3.write(random(125,253));
      while(millis() < timer + random(0,666)){}
    }
    delay(random(0,666));
    //turnOffScreen();
    delay(random(3000,6660));
    //turnOnScreen();
    clearScreen();
    delay(500);
    Serial3.print(F("Loser. Way To Go"));
    setToSecondLine();
    Serial3.print(F("Breaking Shit...")); 
    delay(random(3000,6660));    
    return 1;
  }
  else if (counter == 777)
  {
    //tmrpcm.play("Secret.wav");
    releaseBeer();
    clearScreen();
    Serial3.print(F(" FREE BEER BRO! "));
    setToSecondLine();
    Serial3.print(F(" LUCKY BASTARD! "));
    delay(3000);
    return 1;
  }
  return 0;
}

void turnOffScreen()
{
  Serial3.write(124);
  Serial3.write(128);
}

void turnOnScreen()
{
  Serial3.write(124);
  Serial3.write(157);
}

