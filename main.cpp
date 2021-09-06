#include <dirent.h>
#include <queue>
#include <mutex>          // std::mutex
#include "RecButton.h"
#include "env.h"

// Defined inside env.h
//#define WIDTH 1024
//#define HEIGHT 600
//#define REC_WIDTH 640
//#define REC_HEIGHT 480
//#define MAX_LENGHT 10
//std::string rtsp = "rtsp://user:pass@ip:port"
// End env.h

void displayAndRec();
void receive(cv::Mat* frame);
void mouseCallBack(int event, int x, int y, int flags, void* userdata);

enum recording {OFF, ON};

//--- INITIALIZE VIDEOCAPTURE
RecButton* rec_Button;
Button* reconnect_Button;
cv::VideoCapture cap;
cv::VideoWriter video;
std::queue<cv::Mat> frames;
std::mutex mtx;           // mutex for critical section
char* url; // rtsp://user:pass@ip:port/route

int main(int, char**){
    //printf("%s\n", cv::getBuildInformation().c_str());
    //Create window
    cv::namedWindow("Live", cv::WINDOW_NORMAL);
    cv::resizeWindow("Live", WIDTH, HEIGHT);
    //cv::setWindowProperty("Live", cv::WND_PROP_FULLSCREEN, cv::WINDOW_AUTOSIZE);
    //cv::createTrackbar( "Grabar", "Live", &value, 1, pressRecord, &keepRecording);
    ////cap.set(cv::CAP_PROP_BUFFERSIZE, 1);
    ////cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
    ////cap.set(cv::CAP_PROP_CODEC_PIXEL_FORMAT, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
    //cap.set(cv::CAP_PROP_FRAME_WIDTH, REC_WIDTH);
    //cap.set(cv::CAP_PROP_FRAME_HEIGHT, REC_HEIGHT);
    //set the callback function for any mouse event
    cv::setMouseCallback("Live", mouseCallBack, NULL);
    //createButton("b",NULL); //Need QT
    //cap.setExceptionMode(true);
    //cap.set(CAP_PROP_FPS, 15);
    //createButton("Grabar",record,NULL,QT_CHECKBOX,0);// Need QT

    cv::Mat frame;

    if(frames.empty()){
        printf("\nqueue empty\n");
    }
    // Define the codec and create VideoWriter object.The output is stored in 'video.avi' file.
	// Define the fps to be equal to 10. Also frame size is passed.
	//VideoWriter video;//("video.avi", VideoWriter::fourcc('M','J','P','G'),10, Size(WIDTH, HEIGHT));

    // open the default camera using default API
    // cap.open(0);
    // OR advance usage: select any API backend
    
    //int deviceID = 0;             // 0 = open default camera
    //int apiID = cv::CAP_ANY;      // 0 = autodetect default API

    // open selected camera using selected API
    //"rtsp://user:pass@ip:port"

    //int ex = static_cast<int>(video.get(CAP_PROP_FOURCC));
    //--- GRAB AND WRITE LOOP
    printf("Start grabbing\nPress ESC key or X button to terminate\n");
    //cap.read(frame);
    //printf("%d-%d\n", cap.get(CAP_PROP_FRAME_WIDTH), cap.get(CAP_PROP_FRAME_HEIGHT));
    std::thread read(receive, &frame);
    std::thread disp(displayAndRec);
    //thread recVideo(rec, &frame);
    read.detach();
    disp.detach();
    //recVideo.detach();

    while(true){
        //Cierra la ventana al apretar la X de la esquina superior derecha
        if(!cv::getWindowProperty("Live",cv::WND_PROP_VISIBLE)){
            break;
        }

        // show live and wait for a key with timeout long enough to show images
        if(cv::waitKey(0) == 27){
            break;
        }
        //printf("\nmain\n");
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    // When everything done, release the video capture and write object
    //Kill threads
    disp.~thread();
    read.~thread();

	cap.release();
	video.release();
    cv::destroyAllWindows();
    delete reconnect_Button;
    delete rec_Button;
    delete url;
    
    return 0;
}

void receive(cv::Mat* frame){
    printf("try to open camera o.o\n");
    while(true){
        try{
            //rtsp.c_str()
            cap.open(0, cv::CAP_ANY);
            if (!cap.isOpened()) {
                std::cerr << "ERROR! Unable to open camera\n";
                continue;
            }
            break;
        }
        catch(std::string err){
            printf("Trying to connect...\n");
        }
    }
    while(true){
        try{
            mtx.lock();
            cap.read(*frame);
            //printf("Read");
            //printf("\nMat size: %d\n", frame->size);
            resize(*frame, *frame, cv::Size(REC_WIDTH, REC_HEIGHT));
            if (frame->empty()) {
                std::cerr << "ERROR! blank frame grabbed\n";
                mtx.unlock();
                continue;
            }
            if(frames.size() < MAX_LENGHT){
                frames.push(*frame);
                //printf("push\n");
            }
            mtx.unlock();
            //printf("\nReceive\n");
            Sleep(50);
        }

        catch(std::string str){
            printf("Ha ocurrido un error al leer el frame\n");
        }
    }
}

void displayAndRec(){
    cv::Mat tmp, bar, err;
    cv::Scalar color(100, 50, 0); //BGR --> Azul 
    // Error
    err = cv::imread("./Grote.png", cv::IMREAD_COLOR);
    resize(err, err, cv::Size(640, 480), 0, 0, cv::INTER_CUBIC);
    // Barra
    bar = cv::Mat::zeros(cv::Size(REC_WIDTH, 50), CV_8UC3);
    bar = cv::Mat(bar.rows, bar.cols, CV_8UC3, color);
    rec_Button = new RecButton(bar, cv::Point(bar.cols/6, bar.rows/2), 10, cv::Scalar(0, 0, 255),
    cv::Scalar(0, 255, 0), &video);

    while(true){
        //printf("%d", frames.empty());
        try{
            if(!frames.empty()){
                //printf("display");
                mtx.lock();
                tmp = frames.front();
                frames.pop();
                if(rec_Button->getState() == ON){
                    printf("holo");
                    video.write(tmp);
                    printf("hola");
                    cv::putText(tmp, "Grabando", cv::Point(10, tmp.rows / 10), //top-left position
                    cv::FONT_HERSHEY_TRIPLEX, 1.0, CV_RGB(255, 0, 0), 2);
                }
                //hconcat(a, b, dst) // horizontal
                vconcat(bar, tmp, tmp); // vertical
                // Write the frame into the file 'video.avi'
                //printf("queue size: %d\n", frames.size());
                cv::imshow("Live", tmp);
                //printf("show");
                //waitKey(500);
                mtx.unlock();
            }
            Sleep(50);
        }
        catch(std::string str){
            printf("Hubo un error al grabar o mostrar la imagen");
            cv::imshow("Live", err);
        }
    }
    printf("morí :c");
}

void mouseCallBack(int event, int x, int y, int flags, void* userdata){
    if  ( event == cv::EVENT_LBUTTONDOWN ){
        rec_Button->pressed(x, y);
    }
    else if  ( event == cv::EVENT_RBUTTONDOWN ){
        
    }
    else if  ( event == cv::EVENT_MBUTTONDOWN ){

    }
    else if ( event == cv::EVENT_MOUSEMOVE ){

    }
}