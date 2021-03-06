// Stolen from MJP's bokeh example 
// https://mynameismjp.wordpress.com/2011/02/28/bokeh/
#define DISC_SAMPLES 16
const vec2 disc_kernel[DISC_SAMPLES] = vec2[DISC_SAMPLES](
    vec2(0.2069841f, 0.2245269f),
    vec2(0.525808f, -0.2310817f),
    vec2(0.1383943f, 0.9056122f),
    vec2(0.6633009f, 0.5129843f),
    vec2(-0.4002792f, 0.3727058f),
    vec2(0.07912822f, -0.6512921f),
    vec2(-0.7726067f, -0.5151217f),
    vec2(-0.3843193f, -0.1494132f),
    vec2(-0.9107782f, 0.2500633f),
    vec2(0.6940153f, -0.7098927f),
    vec2(-0.1964669f, -0.379389f),
    vec2(-0.4769286f, 0.1840863f),
    vec2(-0.247328f, -0.8798459f),
    vec2(0.4206541f, -0.714772f),
    vec2(0.5829348f, -0.09794202f),
    vec2(0.3691838f, 0.4140642f)
	);
