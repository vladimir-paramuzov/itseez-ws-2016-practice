#include "opencv_ptest/include/opencv2/ts/ts.hpp"

#include <iostream>

#include "skeleton_filter.hpp"

using namespace std;
using namespace perf;
using namespace cv;
using std::tr1::make_tuple;
using std::tr1::get;

//
// Test(s) for the ConvertColor_BGR2GRAY_BT709 function
//

// PERF_TEST(skeleton, ConvertColor_BGR2GRAY_BT709)
// {
//     Mat input = cv::imread("./bin/testdata/sla.png");
//
//     // Add code here
// }

//
// Test(s) for the ImageResize function
//

#define MAT_SIZES  ::perf::szVGA, ::perf::sz720p, ::perf::sz1080p

typedef perf::TestBaseWithParam<Size> Size_Only;

PERF_TEST_P(Size_Only, ImageResize, testing::Values(MAT_SIZES))
{
    Size sz = GetParam();
    Size sz_to(sz.width / 1.7, sz.height / 1.4);

    cv::Mat src(sz, CV_8UC1);
    cv::Mat dst(Size(sz_to), CV_8UC1);
    cv::Mat gold(Size(sz_to), CV_8UC1);
    declare.in(src, WARMUP_RNG).out(dst);

    cv::RNG rng(234231412);
    rng.fill(src, CV_8UC1, 0, 255);

    ImageResize(src, gold, sz_to);

    TEST_CYCLE()
    {
        ImageResize_optimized(src, dst, sz_to);
    }

    cv::Mat diff; cv::absdiff(dst, gold, diff);
    cv::threshold(diff, diff, 1, 0, cv::THRESH_TOZERO);
    ASSERT_EQ(0, cv::countNonZero(diff));

    SANITY_CHECK(dst);
}

//
// Test(s) for the skeletonize function
//

// #define IMAGES testing::Values( std::string("./bin/testdata/sla.png"),\
//                                 std::string("./bin/testdata/page.png"),\
//                                 std::string("./bin/testdata/schedule.png") )
//
// typedef perf::TestBaseWithParam<std::string> ImageName;
//
// PERF_TEST_P(ImageName, skeletonize, IMAGES)
// {
//     Mat input = cv::imread(GetParam());
//
//     // Add code here
// }

//
// Test(s) for the Thinning function
//

PERF_TEST_P(Size_Only, Thinning, testing::Values(MAT_SIZES))
{
    Size sz = GetParam();

    cv::Mat image(sz, CV_8UC1);
    declare.in(image, WARMUP_RNG).out(image);
    declare.time(40);

    cv::RNG rng(234231412);
    rng.fill(image, CV_8UC1, 0, 255);
    cv::threshold(image, image, 240, 255, cv::THRESH_BINARY_INV);

    cv::Mat gold; GuoHallThinning(image, gold);

    cv::Mat thinned_image;
    TEST_CYCLE()
    {
        GuoHallThinning_optimized(image, thinned_image);
    }

    cv::Mat diff; cv::absdiff(thinned_image, gold, diff);
    ASSERT_EQ(0, cv::countNonZero(diff));

    SANITY_CHECK(image);
}
