#include <stdio.h>
#include <math.h>

int UTA_ID = 1001904809;

/*
this function replicates the experiment where the dynamic of the system only include gravity 
acceleration (theta_dot_dot) = 0
velocity (theta_dot) = 0

this system will be different with the one show in class because the gravity term
is no longer static, but changing dynamically in regard to the angle theta

to make the system simple, let assume the mass is distributed only the end of the rod

from physic, we know: torque = r x F_g = rmg*sin(alpha) = rmg*cos(theta)
  where alpha is the angle between r and F_g, 
  alpha = pi/2 - theta -> sin(alpha) = cos(theta)

our task is to find the term "rmg", we can only do that when the system is static:
  1) apply torque = kp*(theta_ref-theta)
  2) print out: rmg = torque/cos(theta)
  3) record rmg until it's stable (the system no longer oscillating)

see Hand-down for visualize
*/
double experiment_gravity(double theta, double theta_ref){
  double torque = 10.0*(theta_ref-theta);
  double rmg = torque/cos(theta);
  printf("%f\n", rmg);
  return torque;
}


/*
this function replicates how we compensate the gravity to identify the friction

if we apply a constant torqe, the system will eventualy reach constant velocity
  torque = b*theta_dot (when theta_dot is constant)
  -> b = torque/theta_dot

our task is find the b:
  1) apply a constant torque (large torque will make the system reach 
                              constant velocity faster,
                              however, dangerous in practice)
  2) print out: b = torque/theta_dot
  3) record b until it's stable (when system reach constant velocity)
  
*/
double experiment_friction(double theta_dot){
  double constant_torque = 300.0;
  double b = constant_torque/theta_dot;
  printf("%f\n", b);
  return constant_torque;
}

/*
this function replicates the experiment when we compensate gravity and friction
to find the inertial

if we apply constant torque, the system will have constant acceleration imediately
  theta_dot_dot = delta_theta_dot / delta_time
  inertial = constant_torque/theta_dot_dot

our task is find the inertial:
 1) apply different constant torque (100, 200, 300, etc)
 2) record the first 10 inertial for 
 3) take the average
*/
double prev_theta_dot = 0; //previous velocity
double experiment_inertial(double theta_dot){
  double constant_torque = 500.0;
  // the PD_control() was called at rate of 50Hz -> delta_time = 1/50
  double delta_time = 1.0/50;
  double theta_dot_dot = (theta_dot-prev_theta_dot)/delta_time;
  double inertial = constant_torque/theta_dot_dot;
  printf("%f\n", inertial);

  prev_theta_dot = theta_dot; //updating previous velocity
  return constant_torque;
}

// this function should return the amount of torqe want to apply for the dynamic system
// un-comment out part 1, 2, or 3 to simulate the corresponding experiment
// the terminal will print out system identification term
// make sure the comment out the PD controller implemention before doing that
double PD_control(theta, theta_dot, theta_ref, theta_dot_ref)
double theta, theta_dot, theta_ref, theta_dot_ref;
{
  double torque;

  // ------------- 1 ------------------------------
  //torque = experiment_gravity(theta, theta_ref);
  double rmg = 4.96082;
  // ----------------------------------------------
  
  //---------------- 2 ----------------------------
  //torque = experiment_friction(theta_dot); 
  double b = 0.1856;
  // ----------------------------------------------

  //--------------- 3 -----------------------------
  //torque = experiment_inertial(theta_dot);
  double inertial = 1.115; // average
  //-----------------------------------------------


  // PD controller implementation - extra credit
  double theta_dot_dot = theta_dot - prev_theta_dot;
  double compensate = inertial*theta_dot_dot + b*theta_dot + rmg*cos(theta);

  double kp = 1.0; //this one is adjustable to make the system move faster
  double kd = 2*sqrt(kp);
  double PD = kp*(theta_ref-theta) + kd*(theta_dot_ref-theta_dot);

  torque = compensate + PD;
  // --------------------------------------------------

  return(torque);
}
