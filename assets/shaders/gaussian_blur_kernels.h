#if BLUR_SAMPLES == 1
const float blur_weights[3] = float[3](0.25f, 0.5f, 0.25f);
#elif BLUR_SAMPLES == 3
const float blur_weights[7] = float[7]
(
	0.106595f, 0.140367f, 0.165569f, 0.174938f, 0.165569f, 0.140367f, 0.106595f
);
#endif
