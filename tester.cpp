#include "tester.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QDateTime>
#include <QElapsedTimer>
#include <QDebug>
#include <opencv2/aruco.hpp>
#include <chilitags/chilitags.hpp>
#include <QFile>
#include <QTextStream>
extern "C"{
#include <AR/ar.h>
#include <AR/config.h>
#include <AR/arFilterTransMat.h>
#include <AR/arMulti.h>
#include <AR/video.h>
}

ARdouble getSizeFactor(ARdouble dist_factor[], int xsize, int ysize, int dist_function_version)
{
    ARdouble  ox, oy, ix, iy;
    ARdouble  olen, ilen;
    ARdouble  sf, sf1;

    sf = 100.0;

    ox = 0.0;
    oy = dist_factor[7];
    olen = dist_factor[6];
    arParamObserv2Ideal( dist_factor, ox, oy, &ix, &iy, dist_function_version );
    ilen = dist_factor[6] - ix;
    //ARLOG("Olen = %f, Ilen = %f, s = %f\n", olen, ilen, ilen / olen);
    if( ilen > 0 ) {
        sf1 = ilen / olen;
        if( sf1 < sf ) sf = sf1;
    }

    ox = xsize;
    oy = dist_factor[7];
    olen = xsize - dist_factor[6];
    arParamObserv2Ideal( dist_factor, ox, oy, &ix, &iy, dist_function_version );
    ilen = ix - dist_factor[6];
    //ARLOG("Olen = %f, Ilen = %f, s = %f\n", olen, ilen, ilen / olen);
    if( ilen > 0 ) {
        sf1 = ilen / olen;
        if( sf1 < sf ) sf = sf1;
    }

    ox = dist_factor[6];
    oy = 0.0;
    olen = dist_factor[7];
    arParamObserv2Ideal( dist_factor, ox, oy, &ix, &iy, dist_function_version );
    ilen = dist_factor[7] - iy;
    //ARLOG("Olen = %f, Ilen = %f, s = %f\n", olen, ilen, ilen / olen);
    if( ilen > 0 ) {
        sf1 = ilen / olen;
        if( sf1 < sf ) sf = sf1;
    }

    ox = dist_factor[6];
    oy = ysize;
    olen = ysize - dist_factor[7];
    arParamObserv2Ideal( dist_factor, ox, oy, &ix, &iy, dist_function_version );
    ilen = iy - dist_factor[7];
    //ARLOG("Olen = %f, Ilen = %f, s = %f\n", olen, ilen, ilen / olen);
    if( ilen > 0 ) {
        sf1 = ilen / olen;
        if( sf1 < sf ) sf = sf1;
    }


    ox = 0.0;
    oy = 0.0;
    arParamObserv2Ideal( dist_factor, ox, oy, &ix, &iy, dist_function_version );
    ilen = dist_factor[6] - ix;
    olen = dist_factor[6];
    //ARLOG("Olen = %f, Ilen = %f, s = %f\n", olen, ilen, ilen / olen);
    if( ilen > 0 ) {
        sf1 = ilen / olen;
        if( sf1 < sf ) sf = sf1;
    }
    ilen = dist_factor[7] - iy;
    olen = dist_factor[7];
    //ARLOG("Olen = %f, Ilen = %f, s = %f\n", olen, ilen, ilen / olen);
    if( ilen > 0 ) {
        sf1 = ilen / olen;
        if( sf1 < sf ) sf = sf1;
    }

    ox = xsize;
    oy = 0.0;
    arParamObserv2Ideal( dist_factor, ox, oy, &ix, &iy, dist_function_version );
    ilen = ix - dist_factor[6];
    olen = xsize - dist_factor[6];
    //ARLOG("Olen = %f, Ilen = %f, s = %f\n", olen, ilen, ilen / olen);
    if( ilen > 0 ) {
        sf1 = ilen / olen;
        if( sf1 < sf ) sf = sf1;
    }
    ilen = dist_factor[7] - iy;
    olen = dist_factor[7];
    //ARLOG("Olen = %f, Ilen = %f, s = %f\n", olen, ilen, ilen / olen);
    if( ilen > 0 ) {
        sf1 = ilen / olen;
        if( sf1 < sf ) sf = sf1;
    }

    ox = 0.0;
    oy = ysize;
    arParamObserv2Ideal( dist_factor, ox, oy, &ix, &iy, dist_function_version );
    ilen = dist_factor[6] - ix;
    olen = dist_factor[6];
    //ARLOG("Olen = %f, Ilen = %f, s = %f\n", olen, ilen, ilen / olen);
    if( ilen > 0 ) {
        sf1 = ilen / olen;
        if( sf1 < sf ) sf = sf1;
    }
    ilen = iy - dist_factor[7];
    olen = ysize - dist_factor[7];
    //ARLOG("Olen = %f, Ilen = %f, s = %f\n", olen, ilen, ilen / olen);
    if( ilen > 0 ) {
        sf1 = ilen / olen;
        if( sf1 < sf ) sf = sf1;
    }

    ox = xsize;
    oy = ysize;
    arParamObserv2Ideal( dist_factor, ox, oy, &ix, &iy, dist_function_version );
    ilen = ix - dist_factor[6];
    olen = xsize - dist_factor[6];
    //ARLOG("Olen = %f, Ilen = %f, s = %f\n", olen, ilen, ilen / olen);
    if( ilen > 0 ) {
        sf1 = ilen / olen;
        if( sf1 < sf ) sf = sf1;
    }
    ilen = iy - dist_factor[7];
    olen = ysize - dist_factor[7];
    //ARLOG("Olen = %f, Ilen = %f, s = %f\n", olen, ilen, ilen / olen);
    if( ilen > 0 ) {
        sf1 = ilen / olen;
        if( sf1 < sf ) sf = sf1;
    }

    if( sf == 100.0 ) sf = 1.0;

    return sf;
}

Tester::Tester(QObject *parent) : QObject(parent)
{

}

void Tester::run(QString img_path,QString type, qreal param)
{

    if(type!="rotate" && type!="blur" && type!="shift" ){
        qWarning()<<"Wrong type";
        return ;
    }

    //READ IMAGE

    cv::Mat image = cv::imread(QString(img_path).append("/img_artoolkit.png").toStdString());
    if(!image.data){
        qWarning()<<"Can't read image img_artoolkit.png";
        return ;
    }
    cv::Mat artoolkit_img;
    cv::cvtColor(image, artoolkit_img, cv::COLOR_BGR2GRAY);

    image = cv::imread(QString(img_path).append("/img_aruco.png").toStdString());
    if(!image.data){
        qWarning()<<"Can't read image img_aruco.png";
        return ;
    }
    cv::Mat aruco_img;
    cv::cvtColor(image, aruco_img, cv::COLOR_BGR2GRAY);

    image = cv::imread(QString(img_path).append("/img_chilitags.png").toStdString());
    if(!image.data){
        qWarning()<<"Can't read image img_chilitags.png";
        return ;
    }
    cv::Mat chilitags_img;
    cv::cvtColor(image, chilitags_img, cv::COLOR_BGR2GRAY);


    cv::Mat shift_lookUpTable(1, 256, CV_8U);
    if(type=="shift"){
        uchar* p = shift_lookUpTable.ptr();
        for( int i = 0; i < 256; ++i)
            p[i] = cv::saturate_cast<uchar>(i + param);
    }

    QElapsedTimer timer;
    if(!timer.isMonotonic()){
        qWarning()<<"Non monotonic timer";
        return;
    }

    QFile result_file;
    result_file.setFileName(QString(img_path).append("Results.csv"));
    result_file.open(QFile::Append);
    if(!result_file.isOpen()){
        qWarning()<<"Can't save results";
        return ;
    }
    QTextStream results_stream;
    results_stream.setDevice(&result_file);

    QSet<int> marker_set;
    for(int i=1;i<22;i++)
        marker_set<<i;
    marker_set<<23;
    marker_set<<24;
    marker_set<<25;

    //ARTOOLKIT
    AR2VideoBufferT* ar_buffer;
    ARParamLT* ar_param;
    ARHandle* ar_handle;
    ARPattHandle* ar_patt_handle;

    ar_buffer=(AR2VideoBufferT*)malloc(sizeof(AR2VideoBufferT));
    ARParam cparam,scaled_cparam;
    cparam.xsize=640;
    cparam.ysize=480;
    cparam.dist_function_version=4;
    for(int i=0;i<4;i++)
        cparam.dist_factor[i]=0;
    cparam.dist_factor[4]=600;
    cparam.dist_factor[5]=600;
    cparam.dist_factor[6]=0;
    cparam.dist_factor[7]=0;
    cparam.dist_factor[8]=1;

    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++)
            cparam.mat[i][j]=0;
        cparam.mat[i][3]=0;
    }
    cparam.mat[0][0]=600;
    cparam.mat[1][1]=600;
    cparam.mat[0][2]=cparam.xsize/2;
    cparam.mat[1][2]=cparam.ysize/2;

    ARdouble s = getSizeFactor(cparam.dist_factor, cparam.xsize, cparam.ysize, cparam.dist_function_version);
    cparam.mat[0][0] /= s;
    cparam.mat[0][1] /= s;
    cparam.mat[1][0] /= s;
    cparam.mat[1][1] /= s;
    cparam.dist_factor[8] = s;

    bool scaling=false;
    if(artoolkit_img.cols!=640 || artoolkit_img.rows!=480){
        //Scaling
        scaling=true;
        arParamChangeSize(&cparam,artoolkit_img.cols,artoolkit_img.rows,&scaled_cparam);
    }

    if(scaling){
        cparam.xsize=scaled_cparam.xsize;
        cparam.ysize=scaled_cparam.ysize;
        for(int i=0;i<3;i++)
            for(int j=0;j<3;j++){
                cparam.mat[i][j]=scaled_cparam.mat[i][j];
            }
        cparam.dist_factor[4] = scaled_cparam.dist_factor[4];
        cparam.dist_factor[5] = scaled_cparam.dist_factor[5];
        cparam.dist_factor[6] = scaled_cparam.dist_factor[6];
        cparam.dist_factor[7] = scaled_cparam.dist_factor[7];
        cparam.dist_factor[8] = scaled_cparam.dist_factor[8];

        cparam.dist_factor[0]=scaled_cparam.dist_factor[0];
        cparam.dist_factor[1]=scaled_cparam.dist_factor[1];
        cparam.dist_factor[2]=scaled_cparam.dist_factor[2];
        cparam.dist_factor[3]=scaled_cparam.dist_factor[3];


    }


    ar_param = arParamLTCreate(&cparam, AR_PARAM_LT_DEFAULT_OFFSET);
    ar_handle = arCreateHandle(ar_param);
    arSetPixelFormat(ar_handle, AR_PIXEL_FORMAT_MONO);
    arSetLabelingThreshMode(ar_handle, AR_LABELING_THRESH_MODE_MANUAL);
    arSetLabelingThresh(ar_handle,AR_DEFAULT_LABELING_THRESH);
    arSetPatternDetectionMode(ar_handle,AR_TEMPLATE_MATCHING_MONO_AND_MATRIX);
    arSetMatrixCodeType(ar_handle, AR_MATRIX_CODE_4x4_BCH_13_9_3);
    ar_patt_handle=arPattCreateHandle();
    arPattAttach(ar_handle, ar_patt_handle);
    ar_buffer->buff=(ARUint8*)malloc(sizeof(ARUint8)*cparam.xsize*cparam.ysize);
    //***************

    //Aruco
    cv::Ptr<cv::aruco::Dictionary> dictionary =
    cv::aruco::getPredefinedDictionary(cv::aruco::PREDEFINED_DICTIONARY_NAME(cv::aruco::DICT_4X4_250));
    std::vector< int > ids;
    std::vector< std::vector< cv::Point2f > > corners;
    //*********

    //Chilitags
    chilitags::Chilitags chilitags_robust;
    chilitags_robust.setPerformance(chilitags::Chilitags::ROBUST);
    chilitags_robust.setFilter(0, 0.);

    chilitags::Chilitags chilitags_default;
    chilitags_default.setFilter(0, 0.);
    //**********


    //ARTOOLKIT DETECT

    for(int i=0;i<20;i++){
        if(type=="rotate" && !qFuzzyCompare(param,0.0)){
            cv::Point2f pc(artoolkit_img.cols/2., artoolkit_img.rows/2.);
            cv::Mat r = cv::getRotationMatrix2D(pc, param, 1.0);
            cv::warpAffine(artoolkit_img, artoolkit_img, r, artoolkit_img.size());
        }
        else if(type=="blur" && i==0){
            cv::GaussianBlur( artoolkit_img, artoolkit_img, cv::Size( param, param ),0);
        }
        else if(type=="shift" && i==0){
            cv::LUT(artoolkit_img, shift_lookUpTable,artoolkit_img);
        }

        memcpy(ar_buffer->buff,artoolkit_img.data,sizeof(ARUint8)*cparam.xsize*cparam.ysize);
        ar_buffer->bufPlaneCount=0;
        ar_buffer->bufPlanes=NULL;
        ar_buffer->fillFlag=1;
        ar_buffer->time_sec=(ARUint32)QDateTime::currentMSecsSinceEpoch()/1000;
        ar_buffer->time_usec=(ARUint32)QDateTime::currentMSecsSinceEpoch()-ar_buffer->time_sec*1000;

        timer.restart();
        arDetectMarker(ar_handle, ar_buffer->buff);
        auto msec=timer.elapsed();

        auto marker_set_clone=marker_set;
        ARMarkerInfo* marker_info=arGetMarker(ar_handle);
        for(int tag=0; tag<arGetMarkerNum(ar_handle);tag++)
           marker_set_clone.remove(marker_info[tag].idMatrix);

        results_stream<<QDateTime::currentDateTime().toString()<<";"
                     <<"Artoolkit"<<";"<<type<<";"<<param<<";"<<msec<<";";

        Q_FOREACH(auto missed, marker_set_clone)
                  results_stream<<missed<<"-";
        results_stream<<";"<<(float)marker_set_clone.size()/marker_set.size()<<";"<<marker_set_clone.size()<<"\n";

    }
    //ARUCO DETECT
    for(int i=0;i<20;i++){
        if(type=="rotate" && !qFuzzyCompare(param,0.0)){
            cv::Point2f pc(aruco_img.cols/2., aruco_img.rows/2.);
            cv::Mat r = cv::getRotationMatrix2D(pc, param, 1.0);
            cv::warpAffine(aruco_img, aruco_img, r, aruco_img.size());
        }
        else if(type=="blur"&& i==0){
            cv::GaussianBlur( aruco_img, aruco_img, cv::Size( param, param ),0);
        }
        else if(type=="shift"&& i==0){
            cv::LUT(aruco_img, shift_lookUpTable,aruco_img);
        }

        timer.restart();
        cv::aruco::detectMarkers(aruco_img,dictionary,corners,ids);
        auto msec=timer.elapsed();

        auto marker_set_clone=marker_set;
        for(auto tag=0; tag<ids.size();tag++)
           marker_set_clone.remove(ids[tag]);

        results_stream<<QDateTime::currentDateTime().toString()<<";"
                     <<"Aruco"<<";"<<type<<";"<<param<<";"<<msec<<";";

        Q_FOREACH(auto missed, marker_set_clone)
                  results_stream<<missed<<"-";
        results_stream<<";"<<(float)marker_set_clone.size()/marker_set.size()<<";"<<marker_set_clone.size()<<"\n";
    }


    //Chilitags DETECT
    std::map<int, chilitags::Quad> tags;
    for(int i=0;i<20;i++){
        if(type=="rotate" && !qFuzzyCompare(param,0.0)){
            cv::Point2f pc(chilitags_img.cols/2., chilitags_img.rows/2.);
            cv::Mat r = cv::getRotationMatrix2D(pc, param, 1.0);
            cv::warpAffine(chilitags_img, chilitags_img, r, chilitags_img.size());
        }
        else if(type=="blur"&& i==0){
            cv::GaussianBlur( chilitags_img, chilitags_img, cv::Size( param, param ),0);
        }
        else if(type=="shift"&& i==0){
            cv::LUT(chilitags_img, shift_lookUpTable,chilitags_img);
        }

        timer.restart();
        tags = chilitags_robust.find(chilitags_img,chilitags::Chilitags::TRACK_AND_DETECT);
        auto msec=timer.elapsed();

        auto marker_set_clone=marker_set;
        for (std::map<int, chilitags::Quad>::iterator it=tags.begin(); it!=tags.end(); ++it)
           marker_set_clone.remove(it->first);

        results_stream<<QDateTime::currentDateTime().toString()<<";"
                     <<"Chilitags_Robust"<<";"<<type<<";"<<param<<";"<<msec<<";";

        Q_FOREACH(auto missed, marker_set_clone)
                  results_stream<<missed<<"-";
        results_stream<<";"<<(float)marker_set_clone.size()/marker_set.size()<<";"<<marker_set_clone.size()<<"\n";

        timer.restart();
        tags= chilitags_default.find(chilitags_img,chilitags::Chilitags::TRACK_AND_DETECT);
        msec=timer.elapsed();

        marker_set_clone=marker_set;
        for (std::map<int, chilitags::Quad>::iterator it=tags.begin(); it!=tags.end(); ++it)
            marker_set_clone.remove(it->first);

        results_stream<<QDateTime::currentDateTime().toString()<<";"
                     <<"Chilitags_default"<<";"<<type<<";"<<param<<";"<<msec<<";";

        Q_FOREACH(auto missed, marker_set_clone)
            results_stream<<missed<<"-";
        results_stream<<";"<<(float)marker_set_clone.size()/marker_set.size()<<";"<<marker_set_clone.size()<<"\n";

    }

    results_stream.flush();
    result_file.close();

    arPattDeleteHandle(ar_patt_handle);
    arPattDetach(ar_handle);
    arParamLTFree(&ar_param);
    delete ar_buffer->buff;
    delete ar_buffer;
    emit finished();
}
