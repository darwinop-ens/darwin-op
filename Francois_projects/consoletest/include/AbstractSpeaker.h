#ifndef ABSTRACTSPEAKER_H
#define ABSTRACTSPEAKER_H

#include <string>

class AbstractSpeaker
{
    public:
        AbstractSpeaker();
        virtual ~AbstractSpeaker();


        /*
        input: text, the text that has to be said
        effect:
        output:
        */
        virtual void say(std::string text) = 0;
    protected:
    private:
};

#endif // ABSTRACTSPEAKER_H
