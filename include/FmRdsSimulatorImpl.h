/*
 * This file is protected by Copyright. Please refer to the COPYRIGHT file
 * distributed with this source distribution.
 *
 * This file is part of REDHAWK librfsimulators.
 *
 * REDHAWK librfsimulators is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * REDHAWK librfsimulators is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */
#ifndef fmrdssimulatorimpl_H
#define fmrdssimulatorimpl_H

#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <stdio.h>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <queue>
#include <complex>
#include "RfSimulator.h"
#include "Transmitter.h"
#include "UserDataQueue.h"
#include "FIRFilter.h"

#include "CallbackInterface.h"

using namespace boost::filesystem;

namespace RfSimulators {

class FmRdsSimulatorImpl : public RfSimulator {
public:
	FmRdsSimulatorImpl();
	~FmRdsSimulatorImpl();

	/**
	 * Initializes the simulator.
	 * Input:
	 *  cfgFilePath - The path to a folder on the system that contains the XML configuration files as well as the wav files
	 *  userClass - A pointer to the class which implements the CallbackInterface used when data is available.
	 *  logLevel - The logging level of the library. Set to -1 to turn off, 0 for ERROR, 1 for WARN, 2 for DEBUG, 3 for TRACE.
	 * Returns 0 on success, -1 on failure.
	 */
	int init(std::string cfgFilePath, CallbackInterface * userClass, LogLevel logLevel);

	void setGain(float gain) throw(OutOfRangeException);
	void setGainRange(float freqMin, float freqMax);
	float getGain();

	/**
	 * Set the size of the data queue that can build up if users do not
	 * service the callback fast enough.
	 */
	void setQueueSize(unsigned short queueSize);


	void setCenterFrequency(float freq) throw(OutOfRangeException);
	void setCenterFrequencyRange(float minGain, float maxGain);
	float getCenterFrequency();

	void setSampleRate(unsigned int sampleRate) throw(InvalidValue) ;
	unsigned int getSampleRate();

	void start();

	void stop();

	void addNoise(bool shouldAddNoise);
	void setNoiseSigma(float sigma);
	float getNoiseSigma();

private:
	int loadCfgFile(path filPath);
	CallbackInterface *userClass;
	unsigned int maxQueueSize;

	void dataGrab(const boost::system::error_code& error, boost::asio::deadline_timer* alarm);
	void fillNoiseArray();

	boost::asio::io_service io;
	boost::asio::deadline_timer * alarm;
	void _start();
	boost::thread *io_service_thread;
	bool stopped, initialized, shouldAddNoise;
	float tunedFreq;
	float gain;
	unsigned int sampleRate;
	std::valarray<std::complex<float> > awgnNoise;
	std::valarray<std::complex<float> > postFiltArray, preFiltArray;
	float maxFreq, minFreq, minGain, maxGain, noiseSigma;
	std::vector<Transmitter*> transmitters;
	UserDataQueue *userDataQueue;
	FIRFilter *filter;
	std::vector<unsigned int> availableSampleRates;
	int pi; // The puncture index;

	boost::mutex sampleRateMutex, noiseArrayMutex;

};
} // End of namespace
#endif
