#include "VocalSynthesizerSpeaker.h"
#include <speak_lib.h>

VocalSynthesizerSpeaker::VocalSynthesizerSpeaker()
{
    //path="C:/Users/Ancmin/Desktop/consoletest";
    //path="/usr/share";
    path=NULL;
    Buflength = 500;
    Options = 0;
    position=0; end_position=0; flags=espeakCHARS_AUTO;

    output = AUDIO_OUTPUT_PLAYBACK;
    espeak_Initialize(output, Buflength, path, Options );
    espeak_SetVoiceByName("default");
}

VocalSynthesizerSpeaker::~VocalSynthesizerSpeaker()
{
    //dtor
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
void VocalSynthesizerSpeaker::say(std::string text)
{
   espeak_Synth( text.c_str(), text.size()+1, position, position_type, end_position, flags,
    unique_identifier, user_data );
    /*here, I MUST LEAVE A COMMENT... there were a bug: text.size() instead of text.size()+1... long life to C/C++ :)*/

    espeak_Synchronize( );

}

