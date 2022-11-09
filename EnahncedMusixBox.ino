//#include <LiquidCrystal_I2C.h>
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

//LiquidCrystal_I2C lcd(0x3f,16,2);



//uint16_t noteFrequencies[ USED_NOTES ] = { 125, 262 , 294 , 330 , 349, 392, 440 , 494, 523 }; //frequencies for notes. LDR and Remote Control values are used to create indexes for this array
uint16_t noteFrequencies[ 18 ] = { 30, 70, 100 ,125, 150, 180, 210, 245,  262 , 294 , 330 , 349, 392, 440 , 494, 523 , 600}; //frequencies for notes. LDR and Remote Control values are used to create indexes for this array
uint16_t noteLength = 300;  //changes via PTM
unsigned long t0 = 0,t = 0; //used for measuring the silence duration
int8_t noteIndex = 0; // index for noteFrequencies array
int8_t menuResult = MENU_OK;// MENU_OK means that the menu item has been selected currectly

void setup() 
{

  pinMode(CS_PIN,OUTPUT);
  
  pinMode(BUZZER_PIN,OUTPUT); //Initialize Buzzer
  
  /*lcd.init(); 
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
  }*/
}


void loop() 
{
/*    lcd.clear();
    lcd.setCursor(6,0);
    lcd.print("1.New");
    lcd.setCursor(6,1);
    lcd.print("2.Play");*/
    //while( !irRec.decode(&irIn) ); //Wait until user presses a key
  if(menuResult != WRONG_INPUT && menuResult != SONG_PLAYED) //MARKED1//
  {
    if(noteIndex != NOT_IN_RANGE) //NOT_IN_RANGE value is used for handling unwanted input values while recording. when the noteIndex is NOT_IN_RANGE , no sound must be created from the buzzer
      t0 = micros();//Silence start time
    /*if( irRec.decode(&irIn) ) //if key-pressed
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
    else*/
    noteIndex = ldrGetIndex( analogRead(LDR_PIN) );//The input priority is first with the IR remote and then LDR
    int delta = analogRead( PTM_PIN ) / 3;
    if(noteIndex != NOT_IN_RANGE)
    {
        t = micros();
        tone(BUZZER_PIN, noteFrequencies[noteIndex] + delta , noteLength);
        
    
    }//Saving the note information in the file and playing it
    noteLength = 400;//NOTE_LENGTH_MIN + ( analogRead( PTM_PIN ) / 2); //Reading the note duration value
  }
}

int8_t irGetKey()
{


  return NOT_IN_RANGE; //return the NOT_IN_RANGE if the key isnt in the 1-9 range.
}

int8_t ldrGetIndex(uint16_t val)
{ 
  for(uint16_t i = 0;i <= LDR_MAX_RANGE; i += 50) 
    if(val >= i && val <= i + LDR_RANGE_LENGTH)
      return ( i / 50 );

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
