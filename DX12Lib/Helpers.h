#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h> // For HRESULT

// From DXSampleHelper.h 
// Source: https://github.com/Microsoft/DirectX-Graphics-Samples
inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		throw std::exception();
	}
}

// Limit an Angle to [0, 360) degrees
inline float constrainAngle(float x) {
	x = fmodf(x, 360);
	if (x < 0)
		x += 360;
	return x;
}