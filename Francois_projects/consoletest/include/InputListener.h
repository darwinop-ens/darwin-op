#ifndef INPUTLISTENER_H
#define INPUTLISTENER_H


class InputListener
{
    public:
        InputListener();
        virtual ~InputListener();
        virtual void notify(InputEvent inputEvent);
    protected:
    private:
};

#endif // INPUTLISTENER_H
