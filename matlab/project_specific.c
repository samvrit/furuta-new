#include <math.h>
#include <stdbool.h>
#include "project_specific.h"

#define PI (3.1415f)
#define DEG_TO_RAD(x)	((x) * (PI / 180.0f))

#define SAT(x, max, min)	( (x) > (max) ? (max) : ((x) < (min) ? (min) : (x)))


const float A[N_STATES][N_STATES] = {	{0.000000,	0.000000,	0.000000,	1.000000,	0.000000,	0.000000},
										{0.000000,	0.000000,	0.000000,	0.000000,	1.000000,	0.000000},
										{0.000000,	0.000000,	0.000000,	0.000000,	0.000000,	1.000000},
										{0.000000,	82.411254,	-2.785977,	0.000000,	0.000000,	0.000000},
										{0.000000,	130.127738,	-22.802044,	0.000000,	0.000000,	0.000000},
										{0.000000,	-142.150128,	76.102558,	0.000000,	0.000000,	0.000000}};

const float B[N_STATES][N_STATES] = {	{0.000000,	0.000000,	0.000000,	0.000000,	0.000000,	0.000000},
										{0.000000,	0.000000,	0.000000,	0.000000,	0.000000,	0.000000},
										{0.000000,	0.000000,	0.000000,	0.000000,	0.000000,	0.000000},
										{147.219505,	0.000000,	0.000000,	0.000000,	0.000000,	0.000000},
										{168.419459,	0.000000,	0.000000,	0.000000,	0.000000,	0.000000},
										{-184.196878,	0.000000,	0.000000,	0.000000,	0.000000,	0.000000}};

const float C[N_STATES][N_STATES] = {	{1.000000,	0.000000,	0.000000,	0.000000,	0.000000,	0.000000},
										{0.000000,	1.000000,	0.000000,	0.000000,	0.000000,	0.000000},
										{0.000000,	0.000000,	1.000000,	0.000000,	0.000000,	0.000000},
										{0.000000,	0.000000,	0.000000,	0.000000,	0.000000,	0.000000},
										{0.000000,	0.000000,	0.000000,	0.000000,	0.000000,	0.000000},
										{0.000000,	0.000000,	0.000000,	0.000000,	0.000000,	0.000000}};

const float K[N_STATES][N_STATES] = {	{3.162278,	-677.257131,	-1339.746051,	33.700353,	-218.398551,	-198.544387},
										{0.000000,	0.000000,	0.000000,	0.000000,	0.000000,	0.000000},
										{0.000000,	0.000000,	0.000000,	0.000000,	0.000000,	0.000000},
										{0.000000,	0.000000,	0.000000,	0.000000,	0.000000,	0.000000},
										{0.000000,	0.000000,	0.000000,	0.000000,	0.000000,	0.000000},
										{0.000000,	0.000000,	0.000000,	0.000000,	0.000000,	0.000000}};

const float Q = 1e3f;
const float R = 1.0f;

float control_output_process(const float computed_output, const float x_hat[N_STATES], const float timestep)
{
	const bool linearity = (fabs(x_hat[1]) < DEG_TO_RAD(10.0f));
	
	return linearity ? SAT(computed_output, 4.0f, -4.0f) : 0.0f;
}