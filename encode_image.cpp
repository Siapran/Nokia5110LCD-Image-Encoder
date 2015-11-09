#include <stdio.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

using namespace cv;

void print_encoded(Mat &image)
{
	CV_Assert(image.depth() == CV_8U); // accept only uchar images
	CV_Assert(image.channels() == 1); // accept only B&W image

	int rows = (image.rows + 7) / 8;
	int cols = image.cols;

	// table to store bitwise image
	// each byte represents a vertical line
	uchar **table = new uchar*[rows];
	for (int i = 0; i < rows; ++i)
		table[i] = new uchar[cols]();

	for (int j = 0; j < image.rows; ++j)
	{

		uchar* current = image.ptr<uchar>(j);
		int row = j / 8;
		int offset = j % 8;

		for (int i = 0; i < image.cols; ++i)
		{
			table[row][i] |= (*current++ & 1) << offset;
		}
	}

	printf("typedef struct Buffer {\n\
    int w;\n\
    int h;\n\
    const unsigned char data[];\n\
} Buffer;\n\n");

	printf("Buffer image = {\n");
	printf("\t.w = %d,\n", image.cols);
	printf("\t.h = %d,\n", image.rows);
	printf("\t.data = {\n");
	for (int i = 0; i < rows; ++i)
	{
		printf("\t\t");
		for (int j = 0; j < cols; ++j)
		{
			printf("0x%.2X, ", table[i][j]);
		}
		printf("\n");
		delete table[i];
	}
	printf("\t}\n};\n");
	delete table;

}

int main(int argc, char** argv )
{
	if (argc != 2)
	{
		printf("usage: DisplayImage <Image_Path>\n");
		return -1;
	}

	Mat image;
	image = imread( argv[1], 1 );

	if (!image.data)
	{
		printf("No image data \n");
		return -1;
	}

	cvtColor(image, image, CV_RGB2GRAY);
	threshold(image, image, 127, 255, 1);

	print_encoded(image);

	return 0;
}