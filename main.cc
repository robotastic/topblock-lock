/*
 * Copyright 2011 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */


/*
 * GNU Radio C++ example creating dial tone
 * ("the simplest thing that could possibly work")
 *
 * Send a tone each to the left and right channels of stereo audio
 * output and let the user's brain sum them.
 *
 * GNU Radio makes extensive use of Boost shared pointers.  Signal processing
 * blocks are typically created by calling a "make" factory function, which
 * returns an instance of the block as a typedef'd shared pointer that can
 * be used in any way a regular pointer can.  Shared pointers created this way
 * keep track of their memory and free it at the right time, so the user
 * doesn't need to worry about it (really).
 *
 */

// Include header files for each block used in flowgraph

#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>



#include <boost/program_options.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/tokenizer.hpp>
#include <boost/intrusive_ptr.hpp>

 #include <boost/shared_ptr.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>


#include <gnuradio/top_block.h>

#include <gnuradio/blocks/vector_source_f.h>
 #include <gnuradio/blocks/null_sink.h>
 #include <gnuradio/blocks/throttle.h>
 #include <gnuradio/blocks/wavfile_sink.h>
 #include <gnuradio/blocks/file_sink.h>
 #include <cstdlib>
 #include <ctime>



 using namespace std;

 
 time_t last_monkey;
 
gr::top_block_sptr tb;
gr::blocks::vector_source_f::sptr src;
gr::blocks::throttle::sptr throttle;
	gr::blocks::wavfile_sink::sptr wav_sink;
	gr::blocks::file_sink::sptr raw_sink;
		char filename[160];
		char raw_filename[160];

volatile sig_atomic_t exit_flag = 0;

void exit_interupt(int sig){ // can be called asynchronously
  exit_flag = 1; // set flag
}



int main(int argc, char **argv)
{

	std::string device_addr;
	double  samp_rate=32000;

	signal(SIGINT, exit_interupt);

    tb = gr::make_top_block("Main");

    vector<float> floatVector;
	srand((unsigned)time(NULL));
	
	for (int i =0; i < 2000; i++){
        float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        floatVector.push_back(r);
    }
    src = gr::blocks::vector_source_f::make(floatVector,true);
    throttle = gr::blocks::throttle::make(sizeof(float), samp_rate);

	std::stringstream path_stream;
	path_stream << boost::filesystem::current_path().string() <<  "/recordings";

	boost::filesystem::create_directories(path_stream.str());
	sprintf(filename, "%s/main-0.wav", path_stream.str().c_str());
	wav_sink = gr::blocks::wavfile_sink::make(filename,1,samp_rate,16);
		sprintf(raw_filename, "%s/main-0.raw", path_stream.str().c_str());
	raw_sink = gr::blocks::file_sink::make(sizeof(float), raw_filename);


	tb->connect(src,0,throttle,0);
	tb->connect(throttle,0,wav_sink,0);
	tb->connect(throttle,0,raw_sink,0);

	last_monkey = time(NULL);

	tb->start();



	while (1) {
		if(exit_flag){ 
			printf("\n Signal caught!\n");
			tb->stop();
			return 0;
		}
		if ((time(NULL) - last_monkey) > 10) {
			last_monkey = time(NULL);
			tb->lock();
			tb->unlock();
		}
			
	}


  // Exit normally.
	return 0;
		
}
