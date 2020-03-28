#ifndef ZW_IMAGE_H
#define ZW_IMAGE_H

#include <opencv2/opencv.hpp>

namespace zw
{
/** @brief Transform a img order from [H,W,C] to [C,H,W], and multiple alpha then
add beta to img.

Note: You need to new {tensor} for size [3*width*height], and delete[] {tensor}
after inference is done.

The default data order of cv::Mat got after you called cv::imread in OpenCV is
[H,W,C], which is arranged like [BGRBGRBGR...]. This function transform cv::Mat
{img} to float *{tensor}, and change data order to [C,H,W], which is arranged
like [BBB...GGG...RRR...], then multiple each alpha to each channel, and add 
each beta to each channel.
*/
void transformImage(cv::Mat &img, float *tensor, std::vector<float> alpha, std::vector<float> beta)
{
    size_t row = img.rows;
    size_t col = img.cols;
    std::vector<cv::Mat> bgr;
    cv::split(img, bgr);

    float *tensorPtr = tensor;

    for (size_t i = 0; i < bgr.size(); i++)
    {
        bgr[i].convertTo(bgr[i], CV_32F, alpha[i], beta[i]);
        memcpy(tensorPtr, bgr[i].ptr<float>(0), row * col * sizeof(float));
        tensorPtr += row * col;
    }
}

/** @brief Resize and align a img to top left corner of a [{outw}, {outh}] canvas.

This function resize and align cv::Mat {src} to top left corner of cv::Mat &dst,
and edit the resize {ratio} of {src}. {ratio} = inw / outw or inh / outh. If size of
canvas {dst} is larger than {src}, then {src} will be copy to top left corner of {dst}
without resize, otherwise it will resize {src} and copy to {dst}.
*/
void alignToTopLeft(cv::Mat &src, cv::Mat &dst, float outw, float outh, float &ratio)
{
    dst = cv::Mat::zeros(outh, outw, src.type());
    float inw = src.cols;
    float inh = src.rows;
    float inratio = inw / inh;
    float outratio = outw / outh;

    if (outw > inw && outh > inh)
    {
        src.copyTo(dst(cv::Rect(0, 0, inw, inh)));
    }
    else
    {
        if (inratio > outratio)
        {
            int scaledH = outw / inratio;
            ratio = inw / outw;
            cv::resize(src, dst(cv::Rect(0, 0, outw, scaledH)), cv::Size(outw, scaledH));
        }
        else
        {
            int scaledW = outh * inratio;
            ratio = inh / outh;
            cv::resize(src, dst(cv::Rect(0, 0, scaledW, outh)), cv::Size(scaledW, outh));
        }
    }
}
/** @brief Resize and put a img on center of a [{outw}, {outh}] canvas.

This function resize and copy cv::Mat {src} to center of cv::Mat &dst, and edit
the resize {ratio} and begin point ({xb}, {yb}) of {src}. {ratio} = inw / outw or
inh / outh. If size of canvas {dst} is larger than {src}, then {src} will be copy to
center of {dst} without resize, otherwise it will resize {src} and copy to {dst}.
*/
void putOnCenter(cv::Mat &src, cv::Mat &dst, float outw, float outh, float &xb, float &yb, float &ratio)
{
    dst = cv::Mat::zeros(outh, outw, src.type());
    float inw = src.cols;
    float inh = src.rows;
    float inratio = inw / inh;
    float outratio = outw / outh;

    if (outw > inw && outh > inh)
    {
        xb = (outw - inw) / 2;
        yb = (outh - inh) / 2;
        src.copyTo(dst(cv::Rect(xb, yb, inw, inh)));
    }
    else
    {
        if (inratio > outratio)
        {
            int scaledH = outw / inratio;
            ratio = inw / outw;
            xb = 0;
            yb = (outh - scaledH) / 2;
            cv::resize(src, dst(cv::Rect(xb, yb, outw, scaledH)), cv::Size(outw, scaledH));
        }
        else
        {
            int scaledW = outh * inratio;
            ratio = inh / outh;
            xb = (outw - scaledW) / 2;
            yb = 0;
            cv::resize(src, dst(cv::Rect(xb, yb, scaledW, outh)), cv::Size(scaledW, outh));
        }
    }
}
} // namespace zw
#endif // ZW_IMAGE_H