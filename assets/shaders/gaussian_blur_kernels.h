#if BLUR_SAMPLES == 1
const float blur_weights[3] = float[3](0.25f, 0.5f, 0.25f);
#elif BLUR_SAMPLES == 3
const float blur_weights[7] = float[7]
(
	0.03f, 0.07f, 0.2f, 0.4f, 0.2f, 0.07f, 0.03f
);
#endif
