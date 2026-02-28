#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void car_update_25(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_24(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_30(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_26(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_27(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_29(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_28(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_31(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_err_fun(double *nom_x, double *delta_x, double *out_1520141797497590686);
void car_inv_err_fun(double *nom_x, double *true_x, double *out_5704743212718226877);
void car_H_mod_fun(double *state, double *out_4785566418507509536);
void car_f_fun(double *state, double dt, double *out_3942824446318374622);
void car_F_fun(double *state, double dt, double *out_5534632192804848347);
void car_h_25(double *state, double *unused, double *out_5403780092702045375);
void car_H_25(double *state, double *unused, double *out_3885432108132106592);
void car_h_24(double *state, double *unused, double *out_7743310824429992972);
void car_H_24(double *state, double *unused, double *out_7443357706934429728);
void car_h_30(double *state, double *unused, double *out_3031302249336062567);
void car_H_30(double *state, double *unused, double *out_3756093160988866522);
void car_h_26(double *state, double *unused, double *out_1970371018324783078);
void car_H_26(double *state, double *unused, double *out_143928789258050368);
void car_h_27(double *state, double *unused, double *out_4246654516741509101);
void car_H_27(double *state, double *unused, double *out_1581329849188441611);
void car_h_29(double *state, double *unused, double *out_4403841185092638246);
void car_H_29(double *state, double *unused, double *out_132032877681109422);
void car_h_28(double *state, double *unused, double *out_5190164369643171536);
void car_H_28(double *state, double *unused, double *out_1831597393884216829);
void car_h_31(double *state, double *unused, double *out_7797696391773471579);
void car_H_31(double *state, double *unused, double *out_3916078070009067020);
void car_predict(double *in_x, double *in_P, double *in_Q, double dt);
void car_set_mass(double x);
void car_set_rotational_inertia(double x);
void car_set_center_to_front(double x);
void car_set_center_to_rear(double x);
void car_set_stiffness_front(double x);
void car_set_stiffness_rear(double x);
}