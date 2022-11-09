#include <LiquidCrystal_I2C.h>
#include <IRremote.h>
#include <SPI.h>
#include <SD.h>

#define IR_PIN 3
#define USED_KEYS 9
#define USED_NOTES 9
#define BUZZER_PIN 6
#define PTM_PIN A1
#define LDR_PIN A0
#define NOTE_LENGTH_MIN 60
#define LDR_MAX_RANGE 700
#define LDR_RANGE_LENGTH 50
#define NOT_IN_RANGE -1
#define WRONG_INPUT -2
#define MENU_OK 1
#define CS_PIN 10
#define FILE_NAME "musicBox.mcx"
#define SONG_PLAYED -2

IRrecv irRec(IR_PIN);
decode_results irIn;
LiquidCrystal_I2C lcd(0x3f,16,2);

unsigned long irKeys[2][USED_KEYS] = { { 0xFF30CF,0xFF18E7,0xFF7A85,0xFF10EF,0xFF38C7,0xFF5AA5,0xFF42BD,0xFF4AB5, 0xFF52AD } , 
                            { 0x9716BE3F, 0x3D9AE3F7, 0x6794579A, 0x8C22657B, 0x488F3CBB, 0x449E79F, 0x32C6FDF7, 0x1BC0157B , 0x1BC0157B } };
//Key codes for remote control ( 1-9 )

unsigned long irOkKey[2] = { 0xFF6897, 0xC101E57B }; //key codes for zero button

uint16_t noteFrequencies[ USED_NOTES ] = { 125, 262 , 294 , 330 , 349, 392, 440 , 494, 523 }; //frequencies for notes. LDR and Remote Control values are used to create indexes for this array
uint16_t noteLength = 300;  //changes via PTM
unsigned long t0 = 0,t = 0; //used for measuring the silence duration
int8_t noteIndex = 0; // index for noteFrequencies array
int8_t menuResult = MENU_OK;// MENU_OK means that the menu item has been selected currectly
bool newSong = true; 
File songFile;


void setup() 
{

  pinMode(CS_PIN,OUTPUT);
  irRec.enableIRIn(); //Initialize remote control.
  
  pinMode(BUZZER_PIN,OUTPUT); //Initialize Buzzer
  
  lcd.init(); 
  lcd.backlight();
  lcd.clear();
  //Initialize LCD
  
  uint8_t horn[8] = { 9 , 9 , 9 , 9 , 15 , 15 , 15 , 15 } ;
  lcd.createChar( 0, horn);
  lcd.setCursor(25,0);
  lcd.print(char(0));
  lcd.print(" MusicBox ");
  lcd.print(char(0));
  for(uint8_t i = 0;i < 31;i++)
  {
    lcd.scrollDisplayRight();
    delay(150);
  }
  // Creating the test for Start screen.
  
  if( !SD.begin(CS_PIN) ) //SD Card Init
  {// if true then there was a problem in starting the SD Card.
      lcd.clear();
      lcd.setCursor(4,0);
      lcd.print("INIT FAILED!");
      delay(2000);
  }
}


void loop() 
{
  if( newSong ) //if true then Main Menu must be shown
  {
    lcd.clear();
    lcd.setCursor(6,0);
    lcd.print("1.New");
    lcd.setCursor(6,1);
    lcd.print("2.Play");
    while( !irRec.decode(&irIn) ); //Wait until user presses a key
    const unsigned long keyVal = irIn.value; // get key code
    irRec.resume(); //skip last key value
    lcd.clear();
    if ( keyVal == irKeys[0][0] || keyVal == irKeys[1][0] ) //if the user presses 1, new file song will be opened and program starts recording...
    {
      
      SD.remove(FILE_NAME); //Remove last saved song for creating a new song file
      songFile = SD.open(FILE_NAME, FILE_WRITE);
      if( !songFile ) // if true then there was a problem in openning the file
      {
        menuResult = WRONG_INPUT; //WRONG_INPUT value is used for handling unwanted events
        lcd.clear();
        lcd.setCursor(4,0);
        lcd.print("FILE ERROR!");
        delay(2000);
      }
      else
        menuResult = MENU_OK; 

    }
    else if( keyVal == irKeys[0][1] || keyVal == irKeys[1][1]) //if the user presses 2, then the last saved song will be played
    {
      
      songFile = SD.open(FILE_NAME);
      if( !songFile )
      {//if true , there is no saved song or there is a problem in openning the file.
        menuResult = WRONG_INPUT;
        lcd.clear();
        lcd.setCursor(1,0);
        lcd.print("FILE ERROR!");
        songFile.close();
        delay(2000);
      }
      else
      {
        lcd.setCursor( 3 , 1 );
        lcd.print("Playing...");
        unsigned long val,val2 ;
        while(songFile.available())
        {
          val = fileReadNextInt(songFile);
          delayMicroseconds(val); //silence duration
          val = fileReadNextInt(songFile); // note frequency
          val2 = fileReadNextInt(songFile); // note duration
          tone(BUZZER_PIN , val , val2 );
        }
        songFile.close();
        menuResult = SONG_PLAYED;//this value is used for skip the recording after this block ( See //MARKED1// )
        newSong = true;
      }
    }
    else
      menuResult = WRONG_INPUT; 
  }
  
  if(menuResult != WRONG_INPUT && menuResult != SONG_PLAYED) //MARKED1//
  {

    if( newSong )
    {
      lcd.clear();
      lcd.setCursor(4,1);
      lcd.print("Recording...");
      newSong = false;
      noteIndex = 0;
    }
    if(noteIndex != NOT_IN_RANGE) //NOT_IN_RANGE value is used for handling unwanted input values while recording. when the noteIndex is NOT_IN_RANGE , no sound must be created from the buzzer
      t0 = micros();//Silence start time
    if( irRec.decode(&irIn) ) //if key-pressed
    {
      if(irIn.value == irOkKey[0] || irIn.value == irOkKey[1]) //checking 0 pressed event for saving the song
      {
        noteIndex = NOT_IN_RANGE;
        newSong = true;
        songFile.close();
        lcd.clear();
        lcd.setCursor(6,0);
        lcd.print("Saved");
        irRec.resume();
        delay(2000);
      }
      else
        noteIndex = irGetKey(); // get the name of the pressed key
    }
    else
      noteIndex = ldrGetIndex( analogRead(LDR_PIN) );//The input priority is first with the IR remote and then LDR
    
    if(noteIndex != NOT_IN_RANGE)
    {
        t = micros();
        songFile.print( (t - t0) + 700);
        songFile.print(" ");
        songFile.print(noteFrequencies[noteIndex]);
        songFile.print(" ");
        songFile.print(noteLength);
        songFile.print("\n");
        tone(BUZZER_PIN, noteFrequencies[noteIndex] , noteLength);
    }//Saving the note information in the file and playing it
    noteLength = NOTE_LENGTH_MIN + ( analogRead( PTM_PIN ) / 4); //Reading the note duration value
  }
}

int8_t irGetKey()
{

   for(uint8_t i = 0;i < 2;i++) //get the name of the key by searching in key codes
     for(uint8_t j = 0;j < USED_KEYS;j++)
       if(irIn.value == irKeys[i][j])
       {
          irRec.resume(); //skip the current pressed key.
          return j;
       }     
  irRec.resume();
  return NOT_IN_RANGE; //return the NOT_IN_RANGE if the key isnt in the 1-9 range.
}

int8_t ldrGetIndex(uint16_t val)
{ 
  for(uint16_t i = 0;i <= LDR_MAX_RANGE; i += 100) 
    if(val >= i && val <= i + LDR_RANGE_LENGTH)
      return ( i / 100 );

  return NOT_IN_RANGE;
/* segmening the LDR values:
 *  value in [0,50] => return 0
 *  value in [100,150] => return 1
 *  .
 *  .
 *  .
 *  value in [700,750] => return 7
 *  Others => return NOT_IN_RANGE
 */
}

unsigned long fileReadNextInt(File f) //this function reads integer values in the songFile respectively.
{
  unsigned long r = 0;
  char c = 0;
  while( f.available() && ( c = f.read() ) != ' ' && c != '\n')
  {
    r = r * 10 + ( c - '0' );
  }
  return r;
}
