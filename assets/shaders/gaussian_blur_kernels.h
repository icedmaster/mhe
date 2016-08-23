#if BLUR_SAMPLES == 1
const float blur_weights[3] = float[3](0.25f, 0.5f, 0.25f);
#elif BLUR_SAMPLES == 3
const float blur_weights[7] = float[7]
(
	0.106595f, 0.140367f, 0.165569f, 0.174938f, 0.165569f, 0.140367f, 0.106595f
);
#elif BLUR_SAMPLES == 5
const float blur_weights[11] = float[11]
(
	0.000003, 0.000229, 0.005977, 0.060598, 0.24173, 0.382925, 0.24173, 0.060598, 0.005977, 0.000229, 0.000003
);
#endif
