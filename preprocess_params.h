#ifndef PPPARAMS_H
#define PPPARAMS_H

#ifndef N_FEATURE
#define N_FEATURE 13
#endif

#ifndef N_ORIG_FEATURE
#define N_ORIG_FEATURE 13
#endif

extern float pca_components[N_FEATURE][N_ORIG_FEATURE];

#define STANDARD_NORMALIZATION

extern float s_x[N_ORIG_FEATURE];
extern float u_x[N_ORIG_FEATURE];
#endif