#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <iomanip> // setw...
#include <iterator>
#include <map>
#include <vector>
#include <set>
#include <string>
#include <algorithm>

using namespace std;

const int ITERATIONS = 10;

const set<int> EASY_IDS = {0, 36, 190, 462, 38, 1018, 272, 50, 672, 865, 345, 995, 90, 532, 903, 782, 461, 454, 73, 291, 124};

const set<int> TRICKY_IDS = {0, 36, 190, 462, 78, 38, 1018, 272, 50, 672, 865, 345, 912, 995, 90, 532, 903, 782, 461, 80, 454, 73, 291, 124};


// the vector of double contains detection durations for each run,
// the set of int contains the detected tags
typedef pair<vector<double>, set<int>> DetectionResults;

/*********************************************
              ALVAR INVOKATION
*********************************************/
#include <MarkerDetector.h>
DetectionResults alvar_detection(const string& path) {

    cv::Mat image = cv::imread(path);
    IplImage iplImage = image;

    // alvar initialization
    alvar::Camera cam;
    alvar::MarkerDetector<alvar::MarkerData> alvar;
    // second parameter = 0 -> auto-detection of the marker matrix resolution
    alvar.SetMarkerSize(1, 0, 2);

    vector<double> durations;

    for (int i = 0; i < ITERATIONS; i++) {
        int64 startCount = cv::getTickCount();
        alvar.Detect(&iplImage, &cam);
        int64 endCount = cv::getTickCount();
        durations.push_back(((double) endCount - startCount)*1000/cv::getTickFrequency());
    }

    set<int> foundIds;
    for (const auto &tag : *alvar.markers) foundIds.insert(tag.GetId());

    return make_pair(durations, foundIds);

}


/*********************************************
            ARToolkitPlus INVOKATION
*********************************************/
#include <ARToolKitPlus/TrackerSingleMarkerImpl.h>
#include <ARToolKitPlus/CameraImpl.h>
DetectionResults artoolkitplus_detection(const string& path) {

    cv::Mat image = cv::imread(path);
    cv::Size s = image.size();

    // ARToolkitPlus initialization
    
    ARToolKitPlus::CameraImpl cam;
    cam.xsize = s.width;
    cam.ysize = s.height;

    // create a tracker that does:
    //  - 6x6 sized marker images
    //  - samples at a maximum of 6x6
    //  - works with luminance (gray) images
    //  - can load a maximum of 1 pattern
    //  - can detect a maximum of 512 patterns in one image
    ARToolKitPlus::TrackerSingleMarker *tracker = new ARToolKitPlus::TrackerSingleMarkerImpl<6,6,6, 1, 512>(s.width, s.height);

    tracker->setPixelFormat(ARToolKitPlus::PIXEL_FORMAT_BGR);

    tracker->init(NULL, 0,0);

    tracker->setCamera(&cam);

    // the marker in the BCH test image has a thiner border...
    tracker->setBorderWidth(0.125f);
    tracker->activateAutoThreshold(true);
    tracker->setNumAutoThresholdRetries(10);

    tracker->setUndistortionMode(ARToolKitPlus::UNDIST_NONE);

    tracker->setImageProcessingMode(ARToolKitPlus::IMAGE_FULL_RES);

    tracker->setMarkerMode(ARToolKitPlus::MARKER_ID_BCH);


    ARToolKitPlus::ARMarkerInfo *markers= NULL;
    vector<double> durations;

    int numDetected;
    for (int i = 0; i < ITERATIONS; i++) {
        int64 startCount = cv::getTickCount();
        tracker->arDetectMarker(image.data, 160, &markers, &numDetected);
        int64 endCount = cv::getTickCount();
        durations.push_back(((double) endCount - startCount)*1000/cv::getTickFrequency());
    }

    set<int> foundIds;

    for(int i=0; i<numDetected; i++){
        foundIds.insert(markers[i].id);
    }

    return make_pair(durations, foundIds);
}

/*********************************************
              ARUCO INVOKATION
*********************************************/
#include <aruco/aruco.h>
DetectionResults aruco_detection(const string& path) {

    cv::Mat image = cv::imread(path);

    // aruco initialization
    aruco::MarkerDetector aruco;

    aruco::CameraParameters camParam;
    float markerSize=-1;


    vector<aruco::Marker> tags;
    vector<double> durations;

    for (int i = 0; i < ITERATIONS; i++) {
        int64 startCount = cv::getTickCount();
        aruco.detect(image, tags, camParam, markerSize);
        int64 endCount = cv::getTickCount();
        durations.push_back(((double) endCount - startCount)*1000/cv::getTickFrequency());
    }

    set<int> foundIds;
    for (const auto &tag : tags) foundIds.insert(tag.id);

    return make_pair(durations, foundIds);

}


/*********************************************
              CHILITAGS INVOKATION
*********************************************/
#include <chilitags.hpp>
DetectionResults chilitags_detection(const string& path, chilitags::Chilitags::PerformancePreset level = chilitags::Chilitags::FAST ) {

    cv::Mat image = cv::imread(path);

    // chilitags initialization
    chilitags::Chilitags chilitags;
    chilitags.setPerformance(level);
    chilitags.setFilter(0, 0.);


    map<int, chilitags::Quad> tags;
    vector<double> durations;

    for (int i = 0; i < ITERATIONS; i++) {
        int64 startCount = cv::getTickCount();
        tags = chilitags.find(image);
        int64 endCount = cv::getTickCount();
        durations.push_back(((double) endCount - startCount)*1000/cv::getTickFrequency());
    }

    set<int> foundIds;
    for (const auto &tag : tags) foundIds.insert(tag.first);

    return make_pair(durations, foundIds);

}
/*********************************************/



/*********************************************
             HELPERS
*********************************************/
double mean(const vector<double>& vals)
{
    double sum = 0.;
    for(auto v : vals) sum += v;
    return sum/vals.size();
}

double variance(const vector<double>& vals)
{
    double current_mean = mean(vals);
    double temp = 0;
    for(auto v : vals)
        temp += (current_mean-v)*(current_mean-v);
    return temp/vals.size();
}

double sigma(const vector<double>& vals)
{
    return sqrt(variance(vals));
}


void run_detection(string name, string desc, function<DetectionResults(const string&)> detector, const set<int>& reference = EASY_IDS) {

    string path = "data/";
    path += name + "/"+ name + "-benchmark-easy-800.png";
    auto res = detector(path);

    cout << "| `" << name << "` (" << desc << ") | "
         << mean(res.first)
        << " | " 
        << std::setw(10) << std::fixed << std::setprecision(1) << sigma(res.first)
        << " | ";

        set<int> result;
        set_difference(reference.begin(), reference.end(), 
                       res.second.begin(), res.second.end(), 
                       std::inserter(result, result.end()));

        if (!result.empty()) {
            copy(result.begin(), result.end(), ostream_iterator<int>(cout, ", "));
        }
        cout << " |" << endl;
}



/*********************************************
             MAIN

Add here call to different detectors
*********************************************/

int main() {
    using namespace std::placeholders; // for _1, _2...

    cout << "|Library| Average processing time (ms) | Std deviation | Missed markers |" << endl;
    cout << "|-------|-----------------------------:|--------------:|----------------|" << endl;

    // ALVAR
    run_detection("alvar", "default", *alvar_detection);

    // ARToolkitPlus
    run_detection("artoolkitplus", "default", *artoolkitplus_detection);

    // ARUCO
    run_detection("aruco", "default", *aruco_detection);


    // CHILITAGS
    run_detection("chilitags", "`ROBUST` preset", bind(chilitags_detection, _1, chilitags::Chilitags::ROBUST));
    run_detection("chilitags", "`FAST` preset", bind(chilitags_detection, _1, chilitags::Chilitags::FAST));
    run_detection("chilitags", "`FASTER` preset", bind(chilitags_detection, _1, chilitags::Chilitags::FASTER));

    return 0;
}
