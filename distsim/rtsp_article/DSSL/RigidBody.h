#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include <dssl.h>

class RigidBody
{
public:
    RigidBody(StateList* sl);

    // Initialize the states
    void Initialize(Vector<3> pos, Vector<3> vel, Vector<3> euler,
        Vector<3> body_rate);

    // Translational and angular acceleration are in body coordinates
    void Compute(Vector<3> translational_accel, Vector<3> angular_accel);

    Vector<3> GetEuler() { return m_Euler; }
    Matrix<3,3> GetInertialtoBody() { return m_InertialtoBody; }

    Vector<3> GetPos();
    Vector<3> GetVel();
    Vector<3> GetBodyRate();

private:
    State<Vector<3> > m_Pos, m_Vel, m_BodyRate;
    State<Vector<4> > m_Quat;
    Vector<3> m_Euler;
    Matrix<3,3> m_InertialtoBody;
};

#endif