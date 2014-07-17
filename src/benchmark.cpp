#include <opencv2/highgui/highgui.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <iomanip> // setw...
#include <iterator>
#include <map>
#include <vector>
#include <set>
#include <string>
#include <algorithm>

using namespace std;
using namespace std::placeholders; // for _1, _2...
namespace po = boost::program_options;

const int ITERATIONS = 10;

const set<int> EASY_IDS = {0, 36, 190, 462, 38, 1018, 272, 50, 672, 865, 345, 995, 90, 532, 903, 782, 461, 454, 73, 291, 124};

const set<int> TRICKY_IDS = {0, 36, 190, 462, 78, 38, 1018, 272, 50, 672, 865, 345, 912, 995, 90, 532, 903, 782, 461, 80, 454, 73, 291, 124};

const vector<int> RESOLUTIONS = {160, 320, 640, 800, 1280};

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


DetectionResults run_detection(string name, 
                   function<DetectionResults(const string&)> detector, 
                   int resolution = 800) {

    string path = "data/";
    path += name + "/"+ name + "-benchmark-easy-" + to_string(resolution) + ".png";
    return detector(path);
}

void print_md(const string& name, const DetectionResults& res, const string& desc = "default", 
                   const set<int>& reference = EASY_IDS) {

    cout << "| `" << name << "` (" << desc << ") | "
        << mean(res.first)
        << " | " 
        << sigma(res.first)
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


void print_csv(const string& name, const vector<DetectionResults>& all_res, 
                   const set<int>& reference = EASY_IDS) {

    cout << name << ", ";
    
    for (auto res : all_res) {
        cout << mean(res.first) << ", " << sigma(res.first) << ", ";

        set<int> result;
        set_difference(reference.begin(), reference.end(), 
                       res.second.begin(), res.second.end(), 
                       std::inserter(result, result.end()));

        cout << result.size() << ", ";
    }
    cout << endl;
}


/*********************************************
             MAIN

Add here call to different detectors
*********************************************/

int main(int ac, char** av) {

    bool csv = true;
    cout << std::setw(10) << std::fixed << std::setprecision(1);

    // Declare the supported options.
    po::options_description desc("Options");
    desc.add_options()
        ("help,h", "produce help message")
        ("markdown,md", "output benchmarks results in Markdown format")
        ("csv", "output benchmarks results as comma separated values (default)")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(ac, av, desc), vm);
    po::notify(vm); 

    if (vm.count("help")) {
        cout << desc << "\n";
        return 1;
    }

    if (vm.count("markdown")) csv = false;

    map<string, vector<DetectionResults>> results;

    cerr << "Starting now..." << endl;

    for(auto res : RESOLUTIONS) {
        cerr << "\nResolution: " << res << "px." << endl;

        // ALVAR
        cerr << "alvar...";
        auto alvar = run_detection("alvar", *alvar_detection, res);
        results["alvar"].push_back(alvar);

        // ARToolkitPlus
        cerr << "artoolkitplus...";
        auto artoolkit = run_detection("artoolkitplus", *artoolkitplus_detection, res);
        results["artoolkitplus"].push_back(artoolkit);

        // ARUCO
        cerr << "aruco...";
        auto aruco = run_detection("aruco", *aruco_detection, res);
        results["aruco"].push_back(aruco);

        // CHILITAGS
        cerr << "chilitags...";
        auto chilitags_robust = run_detection("chilitags", bind(chilitags_detection, _1, chilitags::Chilitags::ROBUST), res);
        results["chilitags_robust"].push_back(chilitags_robust);
        auto chilitags_fast = run_detection("chilitags", bind(chilitags_detection, _1, chilitags::Chilitags::FAST), res);
        results["chilitags_fast"].push_back(chilitags_fast);
        auto chilitags_faster = run_detection("chilitags", bind(chilitags_detection, _1, chilitags::Chilitags::FASTER), res);
        results["chilitags_faster"].push_back(chilitags_faster);

        cerr << endl;

        // Markdown output
        if (!csv) {
            cout << "\n### Resolution " << res << "x" << (int) (res * 1.414) << "\n" << endl;
            cout << "|Library| Average (ms) | σ | Missed markers |" << endl;
            cout << "|-------|-------------:|--:|----------------|" << endl;
            print_md("alvar", alvar);
            print_md("artoolkitplus", artoolkit);
            print_md("aruco", aruco);
            print_md("chilitags", chilitags_robust, "`ROBUST` preset");
            print_md("chilitags", chilitags_fast, "`FAST` preset");
            print_md("chilitags", chilitags_faster, "`FASTER` preset");
        }
    }

    // CSV output
    if (csv) {

        cout << endl << "name, ";
        for (auto res : RESOLUTIONS) {
            cout << res << "px, σ " << res << "px, misses " << res <<"px, ";
        }
        cout << endl;

        for (auto& kv : results) {
            print_csv(kv.first, kv.second);
        }
    }


    return 0;
}
