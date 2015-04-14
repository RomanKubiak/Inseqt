#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

// GUItool: begin automatically generated code
AudioSynthNoiseWhite     noise1;         //xy=57.33332824707031,117.33332824707031
AudioSynthWaveform       waveform1;      //xy=64.5,36.33332824707031
AudioSynthWaveform       waveform2;      //xy=64.5,81.33332824707031
AudioSynthWaveformSine   sine1;          //xy=72.33332824707031,162.3333282470703
AudioSynthWaveform       waveform3;      //xy=105.5,309.33323669433594
AudioEffectEnvelope      envelope1;      //xy=110,262.3332977294922
AudioMixer4              mixer1;         //xy=237.33328247070312,62.33332824707031
AudioMixer4              mixer3;         //xy=288.33331298828125,294.3332824707031
AudioFilterStateVariable filter1;        //xy=377.3332214355469,183.33334350585938
AudioMixer4              mixer2;         //xy=524.3332977294922,222.3332977294922
AudioEffectEnvelope      envelope2;      //xy=547.3332977294922,100.33331298828125
AudioOutputAnalog        dac1;           //xy=721.3332977294922,246.3333282470703
AudioConnection          patchCord1(noise1, 0, mixer1, 2);
AudioConnection          patchCord2(waveform1, 0, mixer1, 0);
AudioConnection          patchCord3(waveform2, 0, mixer1, 1);
AudioConnection          patchCord4(sine1, 0, mixer1, 3);
AudioConnection          patchCord5(waveform3, 0, mixer3, 1);
AudioConnection          patchCord6(envelope1, 0, mixer3, 0);
AudioConnection          patchCord7(mixer1, 0, filter1, 0);
AudioConnection          patchCord8(mixer3, 0, filter1, 1);
AudioConnection          patchCord9(filter1, 0, mixer2, 0);
AudioConnection          patchCord10(filter1, 1, mixer2, 1);
AudioConnection          patchCord11(filter1, 2, mixer2, 2);
AudioConnection          patchCord12(mixer2, envelope2);
AudioConnection          patchCord13(envelope2, dac1);
// GUItool: end automatically generated code

String inData;

#define DBG(x) Serial.println (x)
#define LEN(x) String(x).length()

void setup()
{
  Serial.begin(9600);
  AudioMemory(8);
  filter1.octaveControl (0);
  envelope1.attack (0);
  envelope2.attack (0);
  envelope1.decay (250);
  envelope2.decay (250);
  envelope1.sustain (0.5);
  envelope2.sustain (0.5);
  envelope1.release (150);
  envelope2.release (150);
  envelope1.hold(0);
  envelope2.hold(0);
  envelope1.delay(0);
  envelope2.delay(0);
  
  waveform1.begin (0.3, 220, WAVEFORM_SAWTOOTH);
  waveform2.begin (0.3, 440, WAVEFORM_SAWTOOTH);
  waveform3.begin (0.5, 30, WAVEFORM_SINE);
  sine1.frequency (60);
  sine1.amplitude (0.15);
  noise1.amplitude(0.0);
  
  mixer2.gain (0,1.0);
  mixer2.gain (1,0.0);
  mixer2.gain (2,0.0);
  
  mixer3.gain (1,0.0);
  mixer3.gain (0,1.0);
}

void loop()
{
  while (Serial.available() > 0)
    {
        char recieved = Serial.read();
        inData += recieved; 

        // Process message when new line character is recieved
        if (recieved == '\n')
        {
            Serial.printf("-> %s", inData.c_str());

            // You can put some if and else here to process the message juste like that:

            if (inData.startsWith("non"))
              trigNoteOn(inData.remove(0, LEN("non")+1));
            if (inData.startsWith("noff"))
              trigNoteOff(inData);
            if (inData.startsWith ("cutoff"))
              setFilterCutoff(inData.remove(0, LEN("cutoff")+1));
            if (inData.startsWith ("q"))
              setFilterQ(inData.remove(0, LEN("q")+1));
            if (inData.startsWith ("filtEnv"))
              setFilterEnv (inData.remove(0, LEN("filtEnv")+1));
            if(inData == "+++\n"){ // DON'T forget to add "\n" at the end of the string.
              Serial.println("OK. Press h for help.");
            }


            inData = ""; // Clear recieved buffer
        }
    }
}

void trigNoteOn(const String &data)
{
  int duration = data.toInt();
  Serial.printf ("trigNoteOn %d\n", duration);
  envelope1.noteOn();
  envelope2.noteOn();
  delay (duration);
  trigNoteOff(String());
}

void trigNoteOff(const String &data)
{
  DBG("trigNoteOff\n");
  envelope1.noteOff();
  envelope2.noteOff();
}

void setFilterCutoff(const String &data)
{
  int cutoff = data.toInt();
  Serial.printf ("setFilterCutoff %d\n", cutoff);
  
  filter1.frequency (cutoff);
}

void setFilterQ(const String &data)
{
  float q = data.toFloat();
  Serial.printf ("setFilterQ %d\n", q);
  
  filter1.resonance (q);
}

void setFilterEnv(const String &data)
{
  // Read each command pair 
  byte token = 0;
  byte attack, decay, release;
  float sustain;
  
  char* command = strtok((char *)data.c_str(), ",");
  while (command != 0)
  {
    switch (token)
    {
      case 0:
        attack = atoi (command);
        break;
      case 1:
        decay = atoi (command);
        break;
      case 2:
        sustain = atof (command);  
        break;
      case 3:
        release = atoi (command);
        break;
     default:
         break;
    }
    token++;
    command = strtok(0, ",");
  }
  
  Serial.printf ("setFilterEnv %d %d %.2f %d", attack, decay, sustain, release);
  envelope1.attack (attack);
  envelope1.decay (decay);
  envelope1.sustain  (sustain);
  envelope1.release (release);
}
