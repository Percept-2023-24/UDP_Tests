// g++ -std=c++14 -Wall -Wextra -pedantic -lfftw3f -lm -I../../src/ -o test test.cpp `pkg-config --cflags --libs opencv4`; ./test 
#include "../src/rpl/private-header.hpp"
#define INPUT_SIZE 64 * 512
#define OUTPUT_SIZE 0

using namespace std;
using namespace std::chrono;
int main(int argc, char* argv[])
{   

    // CONSTRUCTOR INITIATION
    DataAcquisition daq;

    RangeDoppler rdm("blackman");

    Visualizer vis(INPUT_SIZE,OUTPUT_SIZE);
	JSON_UDP client_mw;

    // BUFFER POINTER INITIATION
    uint16_t *in_bufferptr    = daq.getBufferPointer();
    float    *in_visualizeptr = rdm.getBufferPointer();
    float    *ang_visualizeptr = rdm.getAngleBufferPointer();
    int      *angidx_visptr = rdm.getAngleIndexPointer();
    
    rdm.setBufferPointer(in_bufferptr);
    vis.setBufferPointer(in_visualizeptr);
    vis.setAngleBufferPointer(ang_visualizeptr);
    vis.setAngleIndexPointer(angidx_visptr);

    // FRAME POINTER INITIATION
    auto frame_daq = daq.getFramePointer();
    rdm.setFramePointer(frame_daq);
    auto frame_rdm = rdm.getFramePointer();
    daq.setFramePointer(frame_rdm);
    // OTHER PARAMS
    if (argc > 1){
        if(argc != 3){
            cout << "Incorrect number of arguments, format should be : \n    --> ./test max_SNR_THRESHOLD min_SNR_THRESHOLD \n OR --> ./test" << std::endl;
            return 1;
        }
        float max = stof(argv[1]);
        float min = stof(argv[2]);
        rdm.setSNR(max,min);
    }
    vis.setWaitTime(1);
       
	if (client_mw.socket_setup() == 1) {
		cout << "# Frames To Capture: ";
		int num_frames;
		cin >> num_frames;
		int frame = 1;
		num_frames++;
		float frame_angle;
		
		auto start_demo = chrono::high_resolution_clock::now();
		rdm.process();
		while(frame < num_frames) {
			//printf("entered\n");
			daq.process();
			rdm.process();
			vis.process();
			frame_angle = *ang_visualizeptr;
			//printf("finished\n");
			client_mw.process(frame_angle, start_demo);
			frame++;
		}
		client_mw.end_stream();
	}
    return 0;
}
