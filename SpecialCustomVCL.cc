#include "vcl/CustomVCL.h"
#include <opencv2/imgproc.hpp>

void custom_vcl_function(VCL::Image& img, const Json::Value& op)
{
    cv::Mat tmp = img.get_cvmat(true);
    if(op.get("custom_function_type", 0).asString() == "hsv_threshold")
    { 
        cv::cvtColor(tmp, tmp, cv::COLOR_BGR2HSV);
        cv::inRange(tmp, cv::Scalar(op.get("h0", -1).asInt(), op.get("s0", -1).asInt(), op.get("v0", -1).asInt()), cv::Scalar(op.get("h1", -1).asInt(),op.get("s1", -1).asInt(),op.get("v1", -1).asInt()), tmp);
        img.deep_copy_cv(tmp);
    }
    else
    {
        img.deep_copy_cv(tmp);
    }

}