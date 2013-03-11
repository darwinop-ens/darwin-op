#ifndef VOCALSYNTHESIZERSPEAKER_H
#define VOCALSYNTHESIZERSPEAKER_H

#include <string>
#include "AbstractSpeaker.h"
#include <speak_lib.h>


using namespace std;

class VocalSynthesizerSpeaker : public AbstractSpeaker
{
    public:
        /*constructor*/
        VocalSynthesizerSpeaker();

        /*destructor*/
        virtual ~VocalSynthesizerSpeaker();

        virtual void say(string text);

    protected:
    private:

        espeak_POSITION_TYPE position_type;
        espeak_AUDIO_OUTPUT output;
        char *path;
        int Buflength; int Options;
        void* user_data;
        t_espeak_callback *SynthCallback;
        espeak_PARAMETER Parm;
        unsigned int Size,position, end_position, flags, *unique_identifier;
        char Voice[];
};

#endif // VOCALSYNTHESIZERSPEAKER_H
