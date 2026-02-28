#include "car.h"

namespace {
#define DIM 9
#define EDIM 9
#define MEDIM 9
typedef void (*Hfun)(double *, double *, double *);

double mass;

void set_mass(double x){ mass = x;}

double rotational_inertia;

void set_rotational_inertia(double x){ rotational_inertia = x;}

double center_to_front;

void set_center_to_front(double x){ center_to_front = x;}

double center_to_rear;

void set_center_to_rear(double x){ center_to_rear = x;}

double stiffness_front;

void set_stiffness_front(double x){ stiffness_front = x;}

double stiffness_rear;

void set_stiffness_rear(double x){ stiffness_rear = x;}
const static double MAHA_THRESH_25 = 3.8414588206941227;
const static double MAHA_THRESH_24 = 5.991464547107981;
const static double MAHA_THRESH_30 = 3.8414588206941227;
const static double MAHA_THRESH_26 = 3.8414588206941227;
const static double MAHA_THRESH_27 = 3.8414588206941227;
const static double MAHA_THRESH_29 = 3.8414588206941227;
const static double MAHA_THRESH_28 = 3.8414588206941227;
const static double MAHA_THRESH_31 = 3.8414588206941227;

/******************************************************************************
 *                       Code generated with SymPy 1.12                       *
 *                                                                            *
 *              See http://www.sympy.org/ for more information.               *
 *                                                                            *
 *                         This file is part of 'ekf'                         *
 ******************************************************************************/
void err_fun(double *nom_x, double *delta_x, double *out_1520141797497590686) {
   out_1520141797497590686[0] = delta_x[0] + nom_x[0];
   out_1520141797497590686[1] = delta_x[1] + nom_x[1];
   out_1520141797497590686[2] = delta_x[2] + nom_x[2];
   out_1520141797497590686[3] = delta_x[3] + nom_x[3];
   out_1520141797497590686[4] = delta_x[4] + nom_x[4];
   out_1520141797497590686[5] = delta_x[5] + nom_x[5];
   out_1520141797497590686[6] = delta_x[6] + nom_x[6];
   out_1520141797497590686[7] = delta_x[7] + nom_x[7];
   out_1520141797497590686[8] = delta_x[8] + nom_x[8];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_5704743212718226877) {
   out_5704743212718226877[0] = -nom_x[0] + true_x[0];
   out_5704743212718226877[1] = -nom_x[1] + true_x[1];
   out_5704743212718226877[2] = -nom_x[2] + true_x[2];
   out_5704743212718226877[3] = -nom_x[3] + true_x[3];
   out_5704743212718226877[4] = -nom_x[4] + true_x[4];
   out_5704743212718226877[5] = -nom_x[5] + true_x[5];
   out_5704743212718226877[6] = -nom_x[6] + true_x[6];
   out_5704743212718226877[7] = -nom_x[7] + true_x[7];
   out_5704743212718226877[8] = -nom_x[8] + true_x[8];
}
void H_mod_fun(double *state, double *out_4785566418507509536) {
   out_4785566418507509536[0] = 1.0;
   out_4785566418507509536[1] = 0;
   out_4785566418507509536[2] = 0;
   out_4785566418507509536[3] = 0;
   out_4785566418507509536[4] = 0;
   out_4785566418507509536[5] = 0;
   out_4785566418507509536[6] = 0;
   out_4785566418507509536[7] = 0;
   out_4785566418507509536[8] = 0;
   out_4785566418507509536[9] = 0;
   out_4785566418507509536[10] = 1.0;
   out_4785566418507509536[11] = 0;
   out_4785566418507509536[12] = 0;
   out_4785566418507509536[13] = 0;
   out_4785566418507509536[14] = 0;
   out_4785566418507509536[15] = 0;
   out_4785566418507509536[16] = 0;
   out_4785566418507509536[17] = 0;
   out_4785566418507509536[18] = 0;
   out_4785566418507509536[19] = 0;
   out_4785566418507509536[20] = 1.0;
   out_4785566418507509536[21] = 0;
   out_4785566418507509536[22] = 0;
   out_4785566418507509536[23] = 0;
   out_4785566418507509536[24] = 0;
   out_4785566418507509536[25] = 0;
   out_4785566418507509536[26] = 0;
   out_4785566418507509536[27] = 0;
   out_4785566418507509536[28] = 0;
   out_4785566418507509536[29] = 0;
   out_4785566418507509536[30] = 1.0;
   out_4785566418507509536[31] = 0;
   out_4785566418507509536[32] = 0;
   out_4785566418507509536[33] = 0;
   out_4785566418507509536[34] = 0;
   out_4785566418507509536[35] = 0;
   out_4785566418507509536[36] = 0;
   out_4785566418507509536[37] = 0;
   out_4785566418507509536[38] = 0;
   out_4785566418507509536[39] = 0;
   out_4785566418507509536[40] = 1.0;
   out_4785566418507509536[41] = 0;
   out_4785566418507509536[42] = 0;
   out_4785566418507509536[43] = 0;
   out_4785566418507509536[44] = 0;
   out_4785566418507509536[45] = 0;
   out_4785566418507509536[46] = 0;
   out_4785566418507509536[47] = 0;
   out_4785566418507509536[48] = 0;
   out_4785566418507509536[49] = 0;
   out_4785566418507509536[50] = 1.0;
   out_4785566418507509536[51] = 0;
   out_4785566418507509536[52] = 0;
   out_4785566418507509536[53] = 0;
   out_4785566418507509536[54] = 0;
   out_4785566418507509536[55] = 0;
   out_4785566418507509536[56] = 0;
   out_4785566418507509536[57] = 0;
   out_4785566418507509536[58] = 0;
   out_4785566418507509536[59] = 0;
   out_4785566418507509536[60] = 1.0;
   out_4785566418507509536[61] = 0;
   out_4785566418507509536[62] = 0;
   out_4785566418507509536[63] = 0;
   out_4785566418507509536[64] = 0;
   out_4785566418507509536[65] = 0;
   out_4785566418507509536[66] = 0;
   out_4785566418507509536[67] = 0;
   out_4785566418507509536[68] = 0;
   out_4785566418507509536[69] = 0;
   out_4785566418507509536[70] = 1.0;
   out_4785566418507509536[71] = 0;
   out_4785566418507509536[72] = 0;
   out_4785566418507509536[73] = 0;
   out_4785566418507509536[74] = 0;
   out_4785566418507509536[75] = 0;
   out_4785566418507509536[76] = 0;
   out_4785566418507509536[77] = 0;
   out_4785566418507509536[78] = 0;
   out_4785566418507509536[79] = 0;
   out_4785566418507509536[80] = 1.0;
}
void f_fun(double *state, double dt, double *out_3942824446318374622) {
   out_3942824446318374622[0] = state[0];
   out_3942824446318374622[1] = state[1];
   out_3942824446318374622[2] = state[2];
   out_3942824446318374622[3] = state[3];
   out_3942824446318374622[4] = state[4];
   out_3942824446318374622[5] = dt*((-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]))*state[6] - 9.8000000000000007*state[8] + stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*state[1]) + (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*state[4])) + state[5];
   out_3942824446318374622[6] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*state[4])) + state[6];
   out_3942824446318374622[7] = state[7];
   out_3942824446318374622[8] = state[8];
}
void F_fun(double *state, double dt, double *out_5534632192804848347) {
   out_5534632192804848347[0] = 1;
   out_5534632192804848347[1] = 0;
   out_5534632192804848347[2] = 0;
   out_5534632192804848347[3] = 0;
   out_5534632192804848347[4] = 0;
   out_5534632192804848347[5] = 0;
   out_5534632192804848347[6] = 0;
   out_5534632192804848347[7] = 0;
   out_5534632192804848347[8] = 0;
   out_5534632192804848347[9] = 0;
   out_5534632192804848347[10] = 1;
   out_5534632192804848347[11] = 0;
   out_5534632192804848347[12] = 0;
   out_5534632192804848347[13] = 0;
   out_5534632192804848347[14] = 0;
   out_5534632192804848347[15] = 0;
   out_5534632192804848347[16] = 0;
   out_5534632192804848347[17] = 0;
   out_5534632192804848347[18] = 0;
   out_5534632192804848347[19] = 0;
   out_5534632192804848347[20] = 1;
   out_5534632192804848347[21] = 0;
   out_5534632192804848347[22] = 0;
   out_5534632192804848347[23] = 0;
   out_5534632192804848347[24] = 0;
   out_5534632192804848347[25] = 0;
   out_5534632192804848347[26] = 0;
   out_5534632192804848347[27] = 0;
   out_5534632192804848347[28] = 0;
   out_5534632192804848347[29] = 0;
   out_5534632192804848347[30] = 1;
   out_5534632192804848347[31] = 0;
   out_5534632192804848347[32] = 0;
   out_5534632192804848347[33] = 0;
   out_5534632192804848347[34] = 0;
   out_5534632192804848347[35] = 0;
   out_5534632192804848347[36] = 0;
   out_5534632192804848347[37] = 0;
   out_5534632192804848347[38] = 0;
   out_5534632192804848347[39] = 0;
   out_5534632192804848347[40] = 1;
   out_5534632192804848347[41] = 0;
   out_5534632192804848347[42] = 0;
   out_5534632192804848347[43] = 0;
   out_5534632192804848347[44] = 0;
   out_5534632192804848347[45] = dt*(stiffness_front*(-state[2] - state[3] + state[7])/(mass*state[1]) + (-stiffness_front - stiffness_rear)*state[5]/(mass*state[4]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[6]/(mass*state[4]));
   out_5534632192804848347[46] = -dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*pow(state[1], 2));
   out_5534632192804848347[47] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_5534632192804848347[48] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_5534632192804848347[49] = dt*((-1 - (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*pow(state[4], 2)))*state[6] - (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*pow(state[4], 2)));
   out_5534632192804848347[50] = dt*(-stiffness_front*state[0] - stiffness_rear*state[0])/(mass*state[4]) + 1;
   out_5534632192804848347[51] = dt*(-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]));
   out_5534632192804848347[52] = dt*stiffness_front*state[0]/(mass*state[1]);
   out_5534632192804848347[53] = -9.8000000000000007*dt;
   out_5534632192804848347[54] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front - pow(center_to_rear, 2)*stiffness_rear)*state[6]/(rotational_inertia*state[4]));
   out_5534632192804848347[55] = -center_to_front*dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*pow(state[1], 2));
   out_5534632192804848347[56] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_5534632192804848347[57] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_5534632192804848347[58] = dt*(-(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*pow(state[4], 2)) - (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*pow(state[4], 2)));
   out_5534632192804848347[59] = dt*(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(rotational_inertia*state[4]);
   out_5534632192804848347[60] = dt*(-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])/(rotational_inertia*state[4]) + 1;
   out_5534632192804848347[61] = center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_5534632192804848347[62] = 0;
   out_5534632192804848347[63] = 0;
   out_5534632192804848347[64] = 0;
   out_5534632192804848347[65] = 0;
   out_5534632192804848347[66] = 0;
   out_5534632192804848347[67] = 0;
   out_5534632192804848347[68] = 0;
   out_5534632192804848347[69] = 0;
   out_5534632192804848347[70] = 1;
   out_5534632192804848347[71] = 0;
   out_5534632192804848347[72] = 0;
   out_5534632192804848347[73] = 0;
   out_5534632192804848347[74] = 0;
   out_5534632192804848347[75] = 0;
   out_5534632192804848347[76] = 0;
   out_5534632192804848347[77] = 0;
   out_5534632192804848347[78] = 0;
   out_5534632192804848347[79] = 0;
   out_5534632192804848347[80] = 1;
}
void h_25(double *state, double *unused, double *out_5403780092702045375) {
   out_5403780092702045375[0] = state[6];
}
void H_25(double *state, double *unused, double *out_3885432108132106592) {
   out_3885432108132106592[0] = 0;
   out_3885432108132106592[1] = 0;
   out_3885432108132106592[2] = 0;
   out_3885432108132106592[3] = 0;
   out_3885432108132106592[4] = 0;
   out_3885432108132106592[5] = 0;
   out_3885432108132106592[6] = 1;
   out_3885432108132106592[7] = 0;
   out_3885432108132106592[8] = 0;
}
void h_24(double *state, double *unused, double *out_7743310824429992972) {
   out_7743310824429992972[0] = state[4];
   out_7743310824429992972[1] = state[5];
}
void H_24(double *state, double *unused, double *out_7443357706934429728) {
   out_7443357706934429728[0] = 0;
   out_7443357706934429728[1] = 0;
   out_7443357706934429728[2] = 0;
   out_7443357706934429728[3] = 0;
   out_7443357706934429728[4] = 1;
   out_7443357706934429728[5] = 0;
   out_7443357706934429728[6] = 0;
   out_7443357706934429728[7] = 0;
   out_7443357706934429728[8] = 0;
   out_7443357706934429728[9] = 0;
   out_7443357706934429728[10] = 0;
   out_7443357706934429728[11] = 0;
   out_7443357706934429728[12] = 0;
   out_7443357706934429728[13] = 0;
   out_7443357706934429728[14] = 1;
   out_7443357706934429728[15] = 0;
   out_7443357706934429728[16] = 0;
   out_7443357706934429728[17] = 0;
}
void h_30(double *state, double *unused, double *out_3031302249336062567) {
   out_3031302249336062567[0] = state[4];
}
void H_30(double *state, double *unused, double *out_3756093160988866522) {
   out_3756093160988866522[0] = 0;
   out_3756093160988866522[1] = 0;
   out_3756093160988866522[2] = 0;
   out_3756093160988866522[3] = 0;
   out_3756093160988866522[4] = 1;
   out_3756093160988866522[5] = 0;
   out_3756093160988866522[6] = 0;
   out_3756093160988866522[7] = 0;
   out_3756093160988866522[8] = 0;
}
void h_26(double *state, double *unused, double *out_1970371018324783078) {
   out_1970371018324783078[0] = state[7];
}
void H_26(double *state, double *unused, double *out_143928789258050368) {
   out_143928789258050368[0] = 0;
   out_143928789258050368[1] = 0;
   out_143928789258050368[2] = 0;
   out_143928789258050368[3] = 0;
   out_143928789258050368[4] = 0;
   out_143928789258050368[5] = 0;
   out_143928789258050368[6] = 0;
   out_143928789258050368[7] = 1;
   out_143928789258050368[8] = 0;
}
void h_27(double *state, double *unused, double *out_4246654516741509101) {
   out_4246654516741509101[0] = state[3];
}
void H_27(double *state, double *unused, double *out_1581329849188441611) {
   out_1581329849188441611[0] = 0;
   out_1581329849188441611[1] = 0;
   out_1581329849188441611[2] = 0;
   out_1581329849188441611[3] = 1;
   out_1581329849188441611[4] = 0;
   out_1581329849188441611[5] = 0;
   out_1581329849188441611[6] = 0;
   out_1581329849188441611[7] = 0;
   out_1581329849188441611[8] = 0;
}
void h_29(double *state, double *unused, double *out_4403841185092638246) {
   out_4403841185092638246[0] = state[1];
}
void H_29(double *state, double *unused, double *out_132032877681109422) {
   out_132032877681109422[0] = 0;
   out_132032877681109422[1] = 1;
   out_132032877681109422[2] = 0;
   out_132032877681109422[3] = 0;
   out_132032877681109422[4] = 0;
   out_132032877681109422[5] = 0;
   out_132032877681109422[6] = 0;
   out_132032877681109422[7] = 0;
   out_132032877681109422[8] = 0;
}
void h_28(double *state, double *unused, double *out_5190164369643171536) {
   out_5190164369643171536[0] = state[0];
}
void H_28(double *state, double *unused, double *out_1831597393884216829) {
   out_1831597393884216829[0] = 1;
   out_1831597393884216829[1] = 0;
   out_1831597393884216829[2] = 0;
   out_1831597393884216829[3] = 0;
   out_1831597393884216829[4] = 0;
   out_1831597393884216829[5] = 0;
   out_1831597393884216829[6] = 0;
   out_1831597393884216829[7] = 0;
   out_1831597393884216829[8] = 0;
}
void h_31(double *state, double *unused, double *out_7797696391773471579) {
   out_7797696391773471579[0] = state[8];
}
void H_31(double *state, double *unused, double *out_3916078070009067020) {
   out_3916078070009067020[0] = 0;
   out_3916078070009067020[1] = 0;
   out_3916078070009067020[2] = 0;
   out_3916078070009067020[3] = 0;
   out_3916078070009067020[4] = 0;
   out_3916078070009067020[5] = 0;
   out_3916078070009067020[6] = 0;
   out_3916078070009067020[7] = 0;
   out_3916078070009067020[8] = 1;
}
#include <eigen3/Eigen/Dense>
#include <iostream>

typedef Eigen::Matrix<double, DIM, DIM, Eigen::RowMajor> DDM;
typedef Eigen::Matrix<double, EDIM, EDIM, Eigen::RowMajor> EEM;
typedef Eigen::Matrix<double, DIM, EDIM, Eigen::RowMajor> DEM;

void predict(double *in_x, double *in_P, double *in_Q, double dt) {
  typedef Eigen::Matrix<double, MEDIM, MEDIM, Eigen::RowMajor> RRM;

  double nx[DIM] = {0};
  double in_F[EDIM*EDIM] = {0};

  // functions from sympy
  f_fun(in_x, dt, nx);
  F_fun(in_x, dt, in_F);


  EEM F(in_F);
  EEM P(in_P);
  EEM Q(in_Q);

  RRM F_main = F.topLeftCorner(MEDIM, MEDIM);
  P.topLeftCorner(MEDIM, MEDIM) = (F_main * P.topLeftCorner(MEDIM, MEDIM)) * F_main.transpose();
  P.topRightCorner(MEDIM, EDIM - MEDIM) = F_main * P.topRightCorner(MEDIM, EDIM - MEDIM);
  P.bottomLeftCorner(EDIM - MEDIM, MEDIM) = P.bottomLeftCorner(EDIM - MEDIM, MEDIM) * F_main.transpose();

  P = P + dt*Q;

  // copy out state
  memcpy(in_x, nx, DIM * sizeof(double));
  memcpy(in_P, P.data(), EDIM * EDIM * sizeof(double));
}

// note: extra_args dim only correct when null space projecting
// otherwise 1
template <int ZDIM, int EADIM, bool MAHA_TEST>
void update(double *in_x, double *in_P, Hfun h_fun, Hfun H_fun, Hfun Hea_fun, double *in_z, double *in_R, double *in_ea, double MAHA_THRESHOLD) {
  typedef Eigen::Matrix<double, ZDIM, ZDIM, Eigen::RowMajor> ZZM;
  typedef Eigen::Matrix<double, ZDIM, DIM, Eigen::RowMajor> ZDM;
  typedef Eigen::Matrix<double, Eigen::Dynamic, EDIM, Eigen::RowMajor> XEM;
  //typedef Eigen::Matrix<double, EDIM, ZDIM, Eigen::RowMajor> EZM;
  typedef Eigen::Matrix<double, Eigen::Dynamic, 1> X1M;
  typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> XXM;

  double in_hx[ZDIM] = {0};
  double in_H[ZDIM * DIM] = {0};
  double in_H_mod[EDIM * DIM] = {0};
  double delta_x[EDIM] = {0};
  double x_new[DIM] = {0};


  // state x, P
  Eigen::Matrix<double, ZDIM, 1> z(in_z);
  EEM P(in_P);
  ZZM pre_R(in_R);

  // functions from sympy
  h_fun(in_x, in_ea, in_hx);
  H_fun(in_x, in_ea, in_H);
  ZDM pre_H(in_H);

  // get y (y = z - hx)
  Eigen::Matrix<double, ZDIM, 1> pre_y(in_hx); pre_y = z - pre_y;
  X1M y; XXM H; XXM R;
  if (Hea_fun){
    typedef Eigen::Matrix<double, ZDIM, EADIM, Eigen::RowMajor> ZAM;
    double in_Hea[ZDIM * EADIM] = {0};
    Hea_fun(in_x, in_ea, in_Hea);
    ZAM Hea(in_Hea);
    XXM A = Hea.transpose().fullPivLu().kernel();


    y = A.transpose() * pre_y;
    H = A.transpose() * pre_H;
    R = A.transpose() * pre_R * A;
  } else {
    y = pre_y;
    H = pre_H;
    R = pre_R;
  }
  // get modified H
  H_mod_fun(in_x, in_H_mod);
  DEM H_mod(in_H_mod);
  XEM H_err = H * H_mod;

  // Do mahalobis distance test
  if (MAHA_TEST){
    XXM a = (H_err * P * H_err.transpose() + R).inverse();
    double maha_dist = y.transpose() * a * y;
    if (maha_dist > MAHA_THRESHOLD){
      R = 1.0e16 * R;
    }
  }

  // Outlier resilient weighting
  double weight = 1;//(1.5)/(1 + y.squaredNorm()/R.sum());

  // kalman gains and I_KH
  XXM S = ((H_err * P) * H_err.transpose()) + R/weight;
  XEM KT = S.fullPivLu().solve(H_err * P.transpose());
  //EZM K = KT.transpose(); TODO: WHY DOES THIS NOT COMPILE?
  //EZM K = S.fullPivLu().solve(H_err * P.transpose()).transpose();
  //std::cout << "Here is the matrix rot:\n" << K << std::endl;
  EEM I_KH = Eigen::Matrix<double, EDIM, EDIM>::Identity() - (KT.transpose() * H_err);

  // update state by injecting dx
  Eigen::Matrix<double, EDIM, 1> dx(delta_x);
  dx  = (KT.transpose() * y);
  memcpy(delta_x, dx.data(), EDIM * sizeof(double));
  err_fun(in_x, delta_x, x_new);
  Eigen::Matrix<double, DIM, 1> x(x_new);

  // update cov
  P = ((I_KH * P) * I_KH.transpose()) + ((KT.transpose() * R) * KT);

  // copy out state
  memcpy(in_x, x.data(), DIM * sizeof(double));
  memcpy(in_P, P.data(), EDIM * EDIM * sizeof(double));
  memcpy(in_z, y.data(), y.rows() * sizeof(double));
}




}
extern "C" {

void car_update_25(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_25, H_25, NULL, in_z, in_R, in_ea, MAHA_THRESH_25);
}
void car_update_24(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<2, 3, 0>(in_x, in_P, h_24, H_24, NULL, in_z, in_R, in_ea, MAHA_THRESH_24);
}
void car_update_30(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_30, H_30, NULL, in_z, in_R, in_ea, MAHA_THRESH_30);
}
void car_update_26(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_26, H_26, NULL, in_z, in_R, in_ea, MAHA_THRESH_26);
}
void car_update_27(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_27, H_27, NULL, in_z, in_R, in_ea, MAHA_THRESH_27);
}
void car_update_29(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_29, H_29, NULL, in_z, in_R, in_ea, MAHA_THRESH_29);
}
void car_update_28(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_28, H_28, NULL, in_z, in_R, in_ea, MAHA_THRESH_28);
}
void car_update_31(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_31, H_31, NULL, in_z, in_R, in_ea, MAHA_THRESH_31);
}
void car_err_fun(double *nom_x, double *delta_x, double *out_1520141797497590686) {
  err_fun(nom_x, delta_x, out_1520141797497590686);
}
void car_inv_err_fun(double *nom_x, double *true_x, double *out_5704743212718226877) {
  inv_err_fun(nom_x, true_x, out_5704743212718226877);
}
void car_H_mod_fun(double *state, double *out_4785566418507509536) {
  H_mod_fun(state, out_4785566418507509536);
}
void car_f_fun(double *state, double dt, double *out_3942824446318374622) {
  f_fun(state,  dt, out_3942824446318374622);
}
void car_F_fun(double *state, double dt, double *out_5534632192804848347) {
  F_fun(state,  dt, out_5534632192804848347);
}
void car_h_25(double *state, double *unused, double *out_5403780092702045375) {
  h_25(state, unused, out_5403780092702045375);
}
void car_H_25(double *state, double *unused, double *out_3885432108132106592) {
  H_25(state, unused, out_3885432108132106592);
}
void car_h_24(double *state, double *unused, double *out_7743310824429992972) {
  h_24(state, unused, out_7743310824429992972);
}
void car_H_24(double *state, double *unused, double *out_7443357706934429728) {
  H_24(state, unused, out_7443357706934429728);
}
void car_h_30(double *state, double *unused, double *out_3031302249336062567) {
  h_30(state, unused, out_3031302249336062567);
}
void car_H_30(double *state, double *unused, double *out_3756093160988866522) {
  H_30(state, unused, out_3756093160988866522);
}
void car_h_26(double *state, double *unused, double *out_1970371018324783078) {
  h_26(state, unused, out_1970371018324783078);
}
void car_H_26(double *state, double *unused, double *out_143928789258050368) {
  H_26(state, unused, out_143928789258050368);
}
void car_h_27(double *state, double *unused, double *out_4246654516741509101) {
  h_27(state, unused, out_4246654516741509101);
}
void car_H_27(double *state, double *unused, double *out_1581329849188441611) {
  H_27(state, unused, out_1581329849188441611);
}
void car_h_29(double *state, double *unused, double *out_4403841185092638246) {
  h_29(state, unused, out_4403841185092638246);
}
void car_H_29(double *state, double *unused, double *out_132032877681109422) {
  H_29(state, unused, out_132032877681109422);
}
void car_h_28(double *state, double *unused, double *out_5190164369643171536) {
  h_28(state, unused, out_5190164369643171536);
}
void car_H_28(double *state, double *unused, double *out_1831597393884216829) {
  H_28(state, unused, out_1831597393884216829);
}
void car_h_31(double *state, double *unused, double *out_7797696391773471579) {
  h_31(state, unused, out_7797696391773471579);
}
void car_H_31(double *state, double *unused, double *out_3916078070009067020) {
  H_31(state, unused, out_3916078070009067020);
}
void car_predict(double *in_x, double *in_P, double *in_Q, double dt) {
  predict(in_x, in_P, in_Q, dt);
}
void car_set_mass(double x) {
  set_mass(x);
}
void car_set_rotational_inertia(double x) {
  set_rotational_inertia(x);
}
void car_set_center_to_front(double x) {
  set_center_to_front(x);
}
void car_set_center_to_rear(double x) {
  set_center_to_rear(x);
}
void car_set_stiffness_front(double x) {
  set_stiffness_front(x);
}
void car_set_stiffness_rear(double x) {
  set_stiffness_rear(x);
}
}

const EKF car = {
  .name = "car",
  .kinds = { 25, 24, 30, 26, 27, 29, 28, 31 },
  .feature_kinds = {  },
  .f_fun = car_f_fun,
  .F_fun = car_F_fun,
  .err_fun = car_err_fun,
  .inv_err_fun = car_inv_err_fun,
  .H_mod_fun = car_H_mod_fun,
  .predict = car_predict,
  .hs = {
    { 25, car_h_25 },
    { 24, car_h_24 },
    { 30, car_h_30 },
    { 26, car_h_26 },
    { 27, car_h_27 },
    { 29, car_h_29 },
    { 28, car_h_28 },
    { 31, car_h_31 },
  },
  .Hs = {
    { 25, car_H_25 },
    { 24, car_H_24 },
    { 30, car_H_30 },
    { 26, car_H_26 },
    { 27, car_H_27 },
    { 29, car_H_29 },
    { 28, car_H_28 },
    { 31, car_H_31 },
  },
  .updates = {
    { 25, car_update_25 },
    { 24, car_update_24 },
    { 30, car_update_30 },
    { 26, car_update_26 },
    { 27, car_update_27 },
    { 29, car_update_29 },
    { 28, car_update_28 },
    { 31, car_update_31 },
  },
  .Hes = {
  },
  .sets = {
    { "mass", car_set_mass },
    { "rotational_inertia", car_set_rotational_inertia },
    { "center_to_front", car_set_center_to_front },
    { "center_to_rear", car_set_center_to_rear },
    { "stiffness_front", car_set_stiffness_front },
    { "stiffness_rear", car_set_stiffness_rear },
  },
  .extra_routines = {
  },
};

ekf_lib_init(car)
