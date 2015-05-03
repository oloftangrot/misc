#ifndef STATELIST_H
#define STATELIST_H

class StateBase;

class StateList
{
public:
    StateList() : m_StatePtr(0), m_StepTime(0.0) { }
    void AddState(StateBase* s);
    void Initialize(double step_time);
    void Integrate();

    int FrameCount() { return m_FrameCount; }
    double StepTime() { return m_StepTime; }
    double Time() { return m_FrameCount*m_StepTime; }

private:
    StateBase* m_StatePtr;
    int m_FrameCount;
    double m_StepTime;
};

#endif // STATELIST_H