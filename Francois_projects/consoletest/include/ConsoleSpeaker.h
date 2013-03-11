#ifndef CONSOLESPEAKER_H
#define CONSOLESPEAKER_H

#include <AbstractSpeaker.h>


class ConsoleSpeaker : public AbstractSpeaker
{
    public:
        ConsoleSpeaker();
        virtual ~ConsoleSpeaker();
        virtual void say(std::string text);
    protected:
    private:
};

#endif // CONSOLESPEAKER_H
