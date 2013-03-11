#ifndef INPUTLISTENERSPEAKER_H
#define INPUTLISTENERSPEAKER_H

#include "InputListener.h"
#include "AbstractSpeaker.h"

class InputListenerSpeaker  : public InputListener
{
    public:
        InputListenerSpeaker(AbstractSpeaker* speaker);
        virtual ~InputListenerSpeaker();
        void notify(int inputEvent);

    protected:
    private:
        AbstractSpeaker* speaker;
};

#endif // INPUTLISTENERSPEAKER_H
