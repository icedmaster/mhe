#if PCF_TAPS == 0
const float pcf_weights[1 * 1] = float[1 * 1](1.0f);
#elif PCF_TAPS == 2
const float pcf_weights[5 * 5] = float[5 * 5]
(
	0.0f, 0.5f, 1.0f, 0.5f, 0.0f,
	0.5f, 1.0f, 1.0f, 1.0f, 0.5f,
	1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	0.5f, 1.0f, 1.0f, 1.0f, 0.5f,
	0.0f, 0.5f, 1.0f, 0.5f, 0.0f
);
#else
float pcf_weights[7 * 7] = float[7 * 7]
(
	0.0f, 0.0f, 0.5f, 1.0f, 0.5f, 0.0f, 0.0f,
	0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
	0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.5f,
	1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
	0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.5f,
	0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.5f, 1.0f, 0.5f, 0.0f, 0.0f
);
#endif
