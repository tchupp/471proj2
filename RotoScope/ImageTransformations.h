/**
 * \file ImageTransformations.h
 *
 * \author Theo Chupp
 *
 * \brief Header for Image Transformation Functions
 */

#pragma once

#include "graphics/GrImage.h"
#include <math.h>

// Rotate the Image rad radians counter-clockwise
inline void RotateImage(CGrImage& img, double rad)
{
	CGrImage outImg;

	// Fill the image with black
	outImg.SetSameSize(img);
	outImg.Fill();

	auto h = img.GetHeight();
	auto w = img.GetWidth();

	for (auto r = 0; r < h; ++r)
	{
		for (auto c = 0; c < w; ++c)
		{
			auto newX = int((c - w / 2) * cos(rad) - (r - h / 2) * sin(rad) + w / 2);
			auto newY = int((c - w / 2) * sin(rad) + (r - h / 2) * cos(rad) + h / 2);

			if (newY < h && newY >= 0 && newX < w && newX >= 0)
			{
				outImg[newY][newX * 3 + 0] = img[r][c * 3 + 0];
				outImg[newY][newX * 3 + 1] = img[r][c * 3 + 1];
				outImg[newY][newX * 3 + 2] = img[r][c * 3 + 2];
			}
		}
	}

	img = outImg;
}
