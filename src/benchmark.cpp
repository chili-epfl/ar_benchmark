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


void check_marker_set(const set<int>& foundIds, const set<int>& reference) {


        set<int> result;
        set_difference(reference.begin(), reference.end(), 
                        foundIds.begin(), foundIds.end(), 
                        std::inserter(result, result.end()));
        if (!result.empty()) {
            cout << "Some markers were not detected! ";
            copy(result.begin(), result.end(), ostream_iterator<int>(cout, ", "));
            cout << "\n";
        }
}

void run_detection(string name, string desc, function<DetectionResults(const string&)> detector) {
    cout << "*** Testing " << name << " (" << desc << ") ***" << endl;

    string path = "data/";
    path += name + "/benchmark-"+ name + "-easy.png";
    auto res = detector(path);

    check_marker_set(res.second, EASY_IDS);
    cout << "Processing times (ms) results (" << ITERATIONS << " runs):\n";
    cout << "chilitags-easy"
         << std::setw(10) << std::fixed << std::setprecision(1) << mean(res.first)
        << "ms (sd: " 
        << std::setw(10) << std::fixed << std::setprecision(1) << sigma(res.first)
        << "ms)\n";
}



/*********************************************
             MAIN

Add here call to different detectors
*********************************************/

int main() {
    using namespace std::placeholders; // for _1, _2...

    // ALVAR
    run_detection("alvar", "default mode", *alvar_detection);


    // ARUCO
    run_detection("aruco", "default mode", *aruco_detection);


    // CHILITAGS
    run_detection("chilitags", "robust mode", bind(chilitags_detection, _1, chilitags::Chilitags::ROBUST));
    run_detection("chilitags", "fast mode", bind(chilitags_detection, _1, chilitags::Chilitags::FAST));
    run_detection("chilitags", "faster mode", bind(chilitags_detection, _1, chilitags::Chilitags::FASTER));

    return 0;
}
