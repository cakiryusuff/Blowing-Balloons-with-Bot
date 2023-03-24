#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <Windows.h>
#include <iostream>

using namespace std;
using namespace cv;

Mat Screenshots(HWND hwnd)
{ 
    HDC hwindowDC, hwindowCompatibleDC;

    int height, width, srcheight, srcwidth;
    HBITMAP hbwindow;
    Mat src;
    BITMAPINFOHEADER  bi;

    hwindowDC = GetDC(hwnd);
    hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
    SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

    RECT windowsize;   
    GetClientRect(hwnd, &windowsize);

    srcheight = windowsize.bottom;
    srcwidth = windowsize.right;
    height = windowsize.bottom / 1;  
    width = windowsize.right / 1;

    src.create(height, width, CV_8UC4);

    hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
    bi.biSize = sizeof(BITMAPINFOHEADER);    //http://msdn.microsoft.com/en-us/library/windows/window/dd183402%28v=vs.85%29.aspx
    bi.biWidth = width;
    bi.biHeight = -height;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    SelectObject(hwindowCompatibleDC, hbwindow);

    StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, 0, 0, srcwidth, srcheight, SRCCOPY);
    GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, src.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    DeleteObject(hbwindow);
    DeleteDC(hwindowCompatibleDC);
    ReleaseDC(hwnd, hwindowDC);

    return src;
}

int main()
{
    vector<Rect> holder;
    vector<Point> locations_vec;

    Mat result, locations, mainImage;
    Mat img1 = imread("ablon.png", IMREAD_GRAYSCALE);
    HWND hwndDesktop = GetDesktopWindow();
    namedWindow("output", WINDOW_NORMAL);
    int key = 0;

    while (key != 27)
    {
        Mat src = Screenshots(hwndDesktop);
        cvtColor(src, mainImage, COLOR_BGR2GRAY);

        int balonWidth = img1.cols;
        int balonHeight = img1.rows;

        matchTemplate(mainImage, img1, result, TM_CCOEFF_NORMED);
        threshold(result, locations, 0.60, 1.0, THRESH_BINARY);
        findNonZero(locations, locations_vec);

        for (int i = 0; i < locations_vec.size(); i++)
            holder.push_back({ locations_vec[i].x, locations_vec[i].y, balonWidth ,balonHeight });

        groupRectangles(holder, 1, 0.5);

        for (Rect i : holder) {
            int setcursorX = i.x + (i.width / 2);
            int setcursorY = i.y + (i.height / 2);
            SetCursorPos(setcursorX, setcursorY);
            mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
            Sleep(200);
        }
        
        cv::imshow("output", src);
        key = waitKey(1);
    }
    
    return 0;
}

