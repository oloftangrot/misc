#include <StateList.h>
#include <State.h>

void StateList::AddState(StateBase* s)
{
    s->SetNext(m_StatePtr);
    m_StatePtr = s;
}

void StateList::Initialize(double step_time)
{
    assert(step_time > 0.0);

    m_FrameCount = 0;
    m_StepTime = step_time;

    for (StateBase* sb=m_StatePtr; sb; sb=sb->GetNext())
        sb->SetIC();
}

void StateList::Integrate()
{
    for (StateBase* sb=m_StatePtr; sb; sb=sb->GetNext())
        sb->Update(this);

    m_FrameCount++;
}