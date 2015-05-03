#ifndef STATE_H
#define STATE_H

#include <StateList.h>

#include <assert.h>

class StateBase
{
public:
	enum MethodType {
        UNDEFINED,
		DISCRETE,
		EULER, AB2, AB3, AB4,
		RK2, RK4
	};

    StateBase() : m_method(UNDEFINED)  { }
    virtual void SetIC() = 0;
    virtual void Initialize(StateList* sl, MethodType method = AB2) = 0;
    virtual void Update(StateList* sl) = 0;

    inline void SetMethod(MethodType method) { m_method = method; }
    inline MethodType GetMethod() { return m_method; }

    StateBase* GetNext() { return m_next; }
    void SetNext(StateBase* next) { m_next = next; }

private:
	MethodType m_method;
    StateBase *m_next;
};

template <class T = double> class State : public StateBase
{
public:
    State(StateList* sl, MethodType method = AB2)
        { Initialize(sl, method); }
    State(T state_ic, StateList* sl, MethodType method = AB2)
        { Initialize(sl, method); ic = state_ic; }

    void SetIC() { m_state = ic; }
    void Set(const T& value) { m_state = value; }
    void Initialize(StateList* sl, MethodType method = AB2);
    void Update(StateList* sl);

    operator T() const { return m_state; }

    T ic, der;

private:
	T m_state, m_prev_state, m_prev_der[3];
};

#include "State_Impl.h"

#endif // STATE_H
