/*
 * fisherfaces.c
 *
 *  Created on: 27 mars 2023
 *      Author: Daniel Mårtensson
 */

#include "../../Headers/Functions.h"

/* Special case */
#ifdef _WIN32
#include <Windows.h>
#endif // _WIN32


void fisherfaces_train(float X[], size_t y[], float W[], float P[], size_t components, size_t row, size_t column) {
	/* Compute PCA */
	float* Wpca = (float*)malloc(row * components * sizeof(float));
	float* Ppca = (float*)malloc(row * components * sizeof(float));
	pca(X, Wpca, Ppca, components, row, column);

	/* Compute LDA of Ppca */
	float* Wlda = (float*)malloc(row * components * sizeof(float));
	float* Plda = (float*)malloc(row * components * sizeof(float));
	lda(Ppca, y, Wlda, Plda, components, row, components);
	free(Plda);

	/* Compute W = Wpca*Wlda */
	mul(Wpca, Wlda, W, row, components, components);

	/* Compute P = W'*X - Notice that we keep W as transpose because we are going to use that later in prediction */
	/*tran(W, ? , ? );
	mul(W, X, P, ? , ? , column);*/
}

void fisherfaces_read_images(const char folderPaths[]) {
	printf("Counted folders: %i\n", count_folders(folderPaths));
}

