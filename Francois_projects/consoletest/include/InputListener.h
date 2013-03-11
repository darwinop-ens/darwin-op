#ifndef INPUTLISTENER_H
#define INPUTLISTENER_H


class InputListener
{
    public:
        InputListener();
        virtual ~InputListener();
        virtual void notify(int inputEvent) = 0;
    protected:
    private:
};

#endif // INPUTLISTENER_H
