# MusicBox
cool music designed by arduino
# Language: C++
# Micro: Arduino
MusixBox uses a buzzer to create sounds with special frequencies equivalent to music notes!
Inputs can be given in two ways:
  1- Using a IR remote control: Each number on the remote control plays a specific note.
  2- Using Hand waving :) : Wave your hand upon a LDR sensor, the amount of light that passes through you hands and is received 
    by the LDR, determines which note should be played! Cool right?
    
 # Features: (LinearControlProject.ino)
  1- Creating music through the two methods described before.
  2- Save the music: Actualy it doesnt record the music with microphone; musixbox will save the frequencies, durations of the notes and the amount of delay between each note.
  3- Replay the saved music: plays the music by reading frequencies, durations and delays and pushing them to the buzzer.
  4- The box has a 2x16 LCD screen for displaying the menu
  
  # Extra Note:
  This was a project for my linear control course; I decided to write it again later (file: EnhancedMusixBox.ino), in a better, optimized way with lower memory usage. The second one is note completed yet.
