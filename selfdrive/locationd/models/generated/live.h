#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void live_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_9(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_12(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_35(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_32(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_33(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_H(double *in_vec, double *out_6862610125693185305);
void live_err_fun(double *nom_x, double *delta_x, double *out_7431745732265456722);
void live_inv_err_fun(double *nom_x, double *true_x, double *out_6869470061904736112);
void live_H_mod_fun(double *state, double *out_3752023301761711956);
void live_f_fun(double *state, double dt, double *out_8968718372123693675);
void live_F_fun(double *state, double dt, double *out_6025965383751987410);
void live_h_4(double *state, double *unused, double *out_6774450264324261821);
void live_H_4(double *state, double *unused, double *out_8543271267342726198);
void live_h_9(double *state, double *unused, double *out_4702529068986247121);
void live_H_9(double *state, double *unused, double *out_8784460913972316843);
void live_h_10(double *state, double *unused, double *out_6248063499239873588);
void live_H_10(double *state, double *unused, double *out_5345752577761038170);
void live_h_12(double *state, double *unused, double *out_2423553014474820071);
void live_H_12(double *state, double *unused, double *out_4884016398334863623);
void live_h_35(double *state, double *unused, double *out_5063838178321404099);
void live_H_35(double *state, double *unused, double *out_2138453366009849914);
void live_h_32(double *state, double *unused, double *out_6687442077265715167);
void live_H_32(double *state, double *unused, double *out_4995068821021217218);
void live_h_13(double *state, double *unused, double *out_2930308456806553339);
void live_H_13(double *state, double *unused, double *out_4401101416592843803);
void live_h_14(double *state, double *unused, double *out_4702529068986247121);
void live_H_14(double *state, double *unused, double *out_8784460913972316843);
void live_h_33(double *state, double *unused, double *out_7518067506410931078);
void live_H_33(double *state, double *unused, double *out_1012103638629007690);
void live_predict(double *in_x, double *in_P, double *in_Q, double dt);
}