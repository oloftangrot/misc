// Implementations of the template functions defined in State.h

template <class T>
void State<T>::Update(StateList* sl)
{
    switch (GetMethod())
    {
    case DISCRETE:
        m_state = der;
        break;

    case EULER:
        m_state += sl->StepTime() * der;
        break;

    case AB2:
        if (sl->FrameCount() == 0)
            m_state += sl->StepTime() * der;
        else
            m_state += sl->StepTime() * (1.5*der - 0.5*m_prev_der[0]);

        m_prev_der[0] = der;
        break;

    case AB3:
        if (sl->FrameCount() == 0)
        {
            m_state += sl->StepTime() * der;
            m_prev_der[0] = der;
        }
        else
            m_state += (sl->StepTime() / 12) * (23*der - 16*m_prev_der[0] +
                5*m_prev_der[1]);

        m_prev_der[1] = m_prev_der[0];
        m_prev_der[0] = der;
        break;

    case AB4:
        if (sl->FrameCount() == 0)
        {
            m_state += sl->StepTime() * der;
            m_prev_der[0] = der;
            m_prev_der[1] = der;
        }
        else
            m_state += (sl->StepTime() / 24) * (55*der - 59*m_prev_der[0] +
                37*m_prev_der[1] - 9*m_prev_der[2]);

        m_prev_der[2] = m_prev_der[1];
        m_prev_der[1] = m_prev_der[0];
        m_prev_der[0] = der;
        break;

    case RK2:
        if (!(sl->FrameCount() & 1))    // even number frame: first step
        {
            m_prev_state = m_state;
            m_state += 2 * sl->StepTime() * der;
            m_prev_der[0] = der;
        }
        else                        // odd number frame: 2nd step
            m_state = m_prev_state + sl->StepTime() * (der + m_prev_der[0]);

        break;

    case RK4:
        switch (sl->FrameCount() & 3)
        {
        case 0:
            m_prev_state = m_state;
            m_state += 2 * sl->StepTime() * der;
            m_prev_der[2] = der;
            break;

        case 1:
            m_state = m_prev_state + 2 * sl->StepTime() * der;
            m_prev_der[1] = der;
            break;

        case 2:
            m_state = m_prev_state + 4 * sl->StepTime() * der;
            m_prev_der[0] = der;
            break;

        case 3:
            m_state = m_prev_state + (sl->StepTime() / 6) * (4*der + 
                8*m_prev_der[0] + 8*m_prev_der[1] + 4*m_prev_der[2]);
            break;
        }

        break;

    default:
        assert(0);
        break;
    }
}

template <class T>
void State<T>::Initialize(StateList* sl, MethodType method)
{
    sl->AddState(this);
    SetMethod(method);

    der = 0.0;
}
