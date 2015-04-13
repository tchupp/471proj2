/**
* \file ImageProcessing.h
*
* \author Theo Chupp
*
* \brief Header for Image Processing Functions
*/

#pragma once

#include "graphics/GrImage.h"
#include <algorithm>
#include <vector>

using namespace std;

struct Color
{
	// all values from 0 - 255
	int hue;
	int sat;
	int val;

	int red;
	int grn;
	int blu;

	Color(int r, int g, int b)
	{
		red = r;
		grn = g;
		blu = b;

		val = max(max(red, blu), grn);
		auto min = min(min(red, blu), grn);
		double delta = val - min;
		sat = int(delta / val * 255);

		double h;
		if (val == red) // between yellow and magenta
		{
			h = (grn - blu) / delta;
		}
		else if (val == grn) // between cyan and yellow
		{
			h = 2. + (blu - red) / delta;
		}
		else // between magenta and cyan
		{
			h = 4. + (red - grn) / delta;
		}
		hue = int(h * 60.);
		if (hue < 0) hue += 360;
	}
};

inline CPoint FindTopOfHead(CGrImage& img, int xL, int xR, int yB, int yT)
{
	CPoint topOfHead;

	for (auto r = yT; r > yB; r--)
	{
		for (auto c = xL; c < xR; c++)
		{
			auto bl = BYTE(img[r][c * 3 + 0]);
			auto gr = BYTE(img[r][c * 3 + 1]);
			auto rd = BYTE(img[r][c * 3 + 2]);

			auto max = max(max(rd, bl), gr);

			if (max < 25)
			{
				topOfHead.x = c;
				topOfHead.y = r;
				return topOfHead;
			}
		}
	}
	return topOfHead;
}

/* Specifically for finding the top part of the lightsaber.
 * In order to extend the functionality of this method,
 * Color parameters would have to be an argument.
 */
inline CPoint FindLightsaberTop(CGrImage& inImg, int xL, int xR, int yB, int yT)
{
	CPoint point(xR, yB);
	for (auto r = yB; r < yT && r < inImg.GetHeight(); r++)
	{
		for (auto c = xL; c < xR && c < inImg.GetWidth(); c++)
		{
			auto bl = inImg[r][c * 3 + 0]; // blue
			auto gr = inImg[r][c * 3 + 1]; // green
			auto rd = inImg[r][c * 3 + 2]; // red

			Color color(rd, gr, bl);

			if (70 < color.val && color.val < 135 && 110 < color.hue && color.hue < 180)
			{
				if (point.x > c && point.y < r)
				{
					point.x = c;
					point.y = r;
				}
			}
		}
	}
	return point;
}

/* Specifically for finding the handle part of the lightsaber.
*  In order to extend the functionality of this method,
*  Color parameters would have to be an argument.
*/
inline CPoint FindLightsaberHandle(CGrImage& inImg, int xL, int xR, int yB, int yT)
{
	CPoint point(xL, yT);
	for (auto r = yB; r < yT && r < inImg.GetHeight(); r++)
	{
		for (auto c = xL; c < xR && c < inImg.GetWidth(); c++)
		{
			auto bl = inImg[r][c * 3 + 0]; // blue
			auto gr = inImg[r][c * 3 + 1]; // green
			auto rd = inImg[r][c * 3 + 2]; // red

			Color color(rd, gr, bl);

			if (200 < color.val && color.val < 225)
			{
				if (point.x < c && point.y > r)
				{
					point.x = c;
					point.y = r;
				}
			}
		}
	}
	return point;
}


inline void PrewittOperation(CGrImage& img, int xL = 0, int xR = -1, int yB = 0, int yT = -1, bool showImg = false)
{
	if (xR == -1) xR = img.GetWidth();
	if (yT == -1) yT = img.GetHeight();

	int gX[3][3] = {{-1, 0, 1},{-1, 0, 1},{-1, 0, 1}};
	int gY[3][3] = {{-1, -1, -1},{0, 0, 0},{1, 1, 1}};

	CGrImage outImg;
	showImg ? outImg.Copy(img) : outImg.SetSameSize(img);

	for (auto r = yB; r < yT; ++r)
	{
		for (auto c = xL * 3; c < xR * 3; ++c)
		{
			auto pixelX = 0;
			auto pixelY = 0;
			for (auto y = -1; y <= 1; y++)
			{
				if (r + y >= 0 && r + y < img.GetHeight())
				{
					for (auto x = -3; x <= 3; x += 3)
					{
						if (c + x >= 0 && c + x < img.GetWidth() * 3)
						{
							auto color = img[r + y][c + x];
							auto xMulti = gX[y + 1][x / 3 + 1];
							auto yMulti = gY[y + 1][x / 3 + 1];

							pixelX += color * xMulti;
							pixelY += color * yMulti;
						}
					}
				}
			}
			auto color = sqrt(pow(pixelX, 2) + pow(pixelY, 2));
			if (color > 125)
				outImg[r][c] = color;
		}
	}
	img = outImg;
}

inline void MedianFilter(CGrImage& img)
{
	CGrImage outImg;
	outImg.Copy(img);

	auto xL = 250;
	auto xR = 550;
	auto yB = 20;
	auto yT = 400;

	for (auto r = yB; r < yT; ++r)
	{
		for (auto c = xL * 3; c < xR * 3; ++c)
		{
			vector<BYTE> pixels;
			for (auto y = -1; y <= 1; y++)
			{
				for (auto x = -3; x <= 3; x += 3)
				{
					auto i = x;
					auto j = y;
					if (c + i < 0 && c + i >= img.GetWidth() * 3)
					{
						i = 0;
					}
					if (r + j < 0 && r + j >= img.GetHeight())
					{
						j = 0;
					}
					pixels.push_back(img[r + j][c + i]);
				}
			}
			sort(pixels.begin(), pixels.end());
			outImg[r][c] = pixels[4];
		}
	}
	img = outImg;
}
