/**
 * float_rndm.c - Generates random floating point number
 *
 * Generates random floating point numbers within a specified range
 * Acknowledgement: Code by Thang Nguyen ( https://www.mycompiler.io/view/2go3N4CaLQJ )
 */
 
#include <stdlib.h>

/**
 * Generate positive random floating point number
 * NOTE: we do not care if a > b & vice versa
 */
double positive_float_rand_in_range(double pos_a, double pos_b) {
	double pos_start, pos_end, float_rand;

	// Random number within [start, end] */
	if (pos_a >= pos_b) {
		pos_start = pos_b;
		pos_end = pos_a;
	}
	else {
		pos_start = pos_a;
		pos_end = pos_b;
	}

	// Random float in [a, b] = a + random float in [0, b - a], with c = b - a.
	// Random float in [0, c] = c * random float in [0, 1].
	// Random float in [0, 1] = random int / RAND_MAX
	float_rand = pos_start + (pos_end - pos_start) * ((double)rand() / RAND_MAX);
	return float_rand;
}


/**
 * Generate positive or negative random float number
 * NOTE: we do not care if a > b & vice versa
 */
double float_rand_in_range(double a, double b) {
	// Process input to generate random float in positive range, then process output
	if (a < 0 && b < 0) {
		return -positive_float_rand_in_range(-a, -b);
	}
	else if (a < 0 && b > 0) {
		// Random float in [-x, y] = random float in [0, y + x] - x, with x positive
		return positive_float_rand_in_range(-a, b) - (-a);
	}
	else {
		// a > 0 && b > 0
		return positive_float_rand_in_range(a, b);
	}
}

