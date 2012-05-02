#include <float.h>
#include "DoFConstraint.h"
#include "Math\PGSsolver.h"

DoFConstraint::DoFConstraint(Body* body, DoFType type, world_vars* vars):
   Constraint(body, Vector2::ORIGIN, NULL, Vector2::ORIGIN, vars), dof_type(type)
{
   sum_impulse = 0;
   A = std::vector<std::vector<double>>(3);
   A[0] = std::vector<double>(3);
   A[1] = std::vector<double>(3);
   A[2] = std::vector<double>(3);
   Eta = std::vector<double>(3);
   Lambda = std::vector<double>(3, 1);
   //impulseDirection = _collision->normal;

   if ((type & X_AXIS) != 0)
      A[0][0] = bodyA->iMass;
   if ((type & Y_AXIS) != 0)
      A[1][1] = bodyA->iMass;
   if ((type & ANGLE) != 0)
      A[2][2] = bodyA->iInert;
   
   //min_lambda = (1 + w_vars->RESTITUTION) / A[0][0] * w_vars->iTimeStep;
}

void DoFConstraint::Init(const ConstraintInit* init)
{
   //const DoFConstraintInit* dinit = static_cast<const DoFConstraintInit*>(init);
   if ((dof_type & X_AXIS) != 0)
       Eta[0] = -bodyA->velocity.v1;
   if ((dof_type & Y_AXIS) != 0)
      Eta[1] = -bodyA->velocity.v2;
   if ((dof_type & ANGLE) != 0)
      Eta[3] = -bodyA->angle_vel;
}

double DoFConstraint::_deltaImpulse(void)
{
   if (!Enough())
   {
      SolveLambda(A, Eta, Lambda, -DBL_MAX, DBL_MAX);
      impulseDirection = Vector2(1, 0) * Lambda[0] + Vector2(0, 1) * Lambda[1];
      impulse = impulseDirection.norm2();
      if (impulse > DBL_EPSILON)
          impulseDirection.normalize2();
      /*if (impulse + sum_impulse < 0)
         impulse = -sum_impulse;
      sum_impulse += impulse;*/
   }
   else
   {
      impulse = 0;
   }
   return impulse;
}

Vector2 DoFConstraint::_impulseDirection(void) const
{
   return impulseDirection;
}

size_t DoFConstraint::NumIter(void) const
{
   return 5;
}

bool DoFConstraint::Enough(void) const
{
   double sum_vel = 0;
   if ((dof_type & X_AXIS) != 0)
      sum_vel += abs(bodyA->velocity.v1);
   if ((dof_type & Y_AXIS) != 0)
      sum_vel += abs(bodyA->velocity.v2);
   return sum_vel <= DBL_EPSILON;
}