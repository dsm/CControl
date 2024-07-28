/*
 * mpc.c
 *
 *  Created on: 1 mars 2020
 *      Author: Daniel Mårtensson
 */

#include "controlengineering.h"

static void obsv(float PHI[], float A[], float C[], size_t ADIM, size_t YDIM, size_t HORIZON);
static void cab(float GAMMA[], float PHI[], float B[], float C[], size_t ADIM, size_t YDIM, size_t RDIM, size_t HORIZON);

/*
 * Model predictive control with linear programming
 * Hint: Look up lmpc.m in Matavecontrol
 */
void lmpc(float A[], float B[], float C[], float x[], float u[], float r[], size_t ADIM, size_t YDIM, size_t RDIM, size_t HORIZON, bool has_integration){
	/* TODO: This is under development 
	
	// Create the extended observability matrix
	float PHI[HORIZON*YDIM*ADIM];
	obsv(PHI, A, C, ADIM, YDIM, HORIZON);

	// Create the lower triangular toeplitz matrix
	float GAMMA[HORIZON*YDIM*HORIZON*RDIM];
	memset(GAMMA, 0, HORIZON*YDIM*HORIZON*RDIM*sizeof(float)); // We need memset here
	cab(GAMMA, PHI, B, C, ADIM, YDIM, RDIM, HORIZON);

	// Find the input value from GAMMA and PHI
	// R_vec = R*r
	float R_vec[HORIZON * YDIM];
	for (size_t i = 0; i < HORIZON * YDIM; i++) {
		for (size_t j = 0; j < YDIM; j++) {
			R_vec[i + j] = r[j];
		}
		i += YDIM - 1;
	}

	// PHI_vec = PHI*x
	float PHI_vec[HORIZON * YDIM];
	mul(PHI, x, PHI_vec, HORIZON * YDIM, ADIM, 1);

	// R_PHI_vec = R_vec - PHI_vec
	float R_PHI_vec[HORIZON * YDIM];
	for(size_t i = 0; i < HORIZON * YDIM; i++){
		*(R_PHI_vec + i) = *(R_vec + i) - *(PHI_vec + i);
	}

	// Transpose gamma
	float GAMMAT[HORIZON*YDIM*HORIZON*RDIM];
	memcpy(GAMMAT, GAMMA, HORIZON*YDIM*HORIZON*RDIM*sizeof(float)); // GAMMA -> GAMMAT
	tran(GAMMAT, HORIZON*YDIM, HORIZON*RDIM);

	// b = GAMMAT*R_PHI_vec
	float b[HORIZON * YDIM];
	mul(GAMMAT, R_PHI_vec, b, HORIZON * RDIM, HORIZON*YDIM, 1);

	// GAMMATGAMMA = GAMMAT*GAMMA = A
	float GAMMATGAMMA[HORIZON * RDIM*HORIZON * RDIM];
	mul(GAMMAT, GAMMA, GAMMATGAMMA, HORIZON * RDIM, HORIZON*YDIM, HORIZON * RDIM);

	// Copy A and call it AT
	float AT[HORIZON * RDIM*HORIZON * RDIM];
	memcpy(AT, GAMMATGAMMA, HORIZON * RDIM*HORIZON * RDIM*sizeof(float)); // A -> AT
	tran(AT, HORIZON*RDIM, HORIZON*RDIM);

	// Now create c = AT*R_PHI_vec
	float c[HORIZON * YDIM];
	mul(AT, R_PHI_vec, c, HORIZON*RDIM, HORIZON*RDIM, 1);

	// Do linear programming now
	linprog(c, GAMMATGAMMA, b, R_vec, HORIZON*YDIM, HORIZON*RDIM, 0);

	// We select the best input values, depending on if we have integration behavior or not in our model
	if(has_integration == true){
		// Set first R_vec to u - Done
		for(size_t i = 0; i < RDIM; i++){
			u[i] = R_vec[i];
		}
	}else{
		// Set last R_vec to u - Done
		for(size_t i = 0; i < RDIM; i++){
			u[i] = R_vec[HORIZON * RDIM - RDIM + i];
		}
	}
	*/
}




/*
 * Lower triangular toeplitz of extended observability matrix
 * CAB stands for C*A^i*B because every element is C*A*B
 */
void cab(float GAMMA[], float PHI[], float B[], float C[], size_t ADIM, size_t YDIM, size_t RDIM, size_t HORIZON){
	/* Decleration */
	size_t i, j;

	/* First create the initial C*A^0*B == C*I*B == C*B */
	float *CB = (float*)malloc(YDIM * RDIM * sizeof(float));
	mul(C, B, CB, YDIM, ADIM, RDIM);

	/* Take the transpose of CB so it will have dimension RDIM*YDIM instead */
	tran(CB, YDIM, RDIM);

	/* Create the CAB matrix from PHI*B */
	float *PHIB = (float*)malloc(HORIZON * YDIM * RDIM * sizeof(float));
	mul(PHI, B, PHIB, HORIZON*YDIM, ADIM, RDIM); /* CAB = PHI*B */
	tran(PHIB, HORIZON*YDIM, RDIM);

	/*
	 * We insert GAMMA = [CB PHI;
	 *                    0  CB PHI;
	 *            		  0   0  CB PHI;
	 *            		  0   0   0  CB PHI] from left to right
	 */
	for(i = 0; i < HORIZON; i++) {
		for(j = 0; j < RDIM; j++) {
			memcpy(GAMMA + HORIZON*YDIM*(i*RDIM+j) + YDIM*i, CB + YDIM*j, YDIM*sizeof(float)); /* Add CB */
			memcpy(GAMMA + HORIZON*YDIM*(i*RDIM+j) + YDIM*i + YDIM, PHIB + HORIZON*YDIM*j, (HORIZON-i-1)*YDIM*sizeof(float)); /* Add PHI*B */
		}
	}

	/* Transpose of gamma */
	tran(GAMMA, HORIZON*RDIM, HORIZON*YDIM);

	/* Free */
	free(CB);
	free(PHIB);
}
