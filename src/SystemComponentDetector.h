/*
 * SystemComponentDetector.h
 *
 *  Created on: Sep 25, 2017
 *      Author: hungnv103
 */

#ifndef SYSTEMCOMPONENTDETECTOR_H_
#define SYSTEMCOMPONENTDETECTOR_H_

#include <string>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <pthread.h>
#include <iostream>
#include <errno.h>

class SystemComponentDetector {
public:
	SystemComponentDetector();
	virtual ~SystemComponentDetector();

	static std::string getUk31Path();
	static std::string getUk43Path();
	static std::string getUk51Path();
	static std::string getUk61Path();
	static std::string getUk42SimFilePath();

	static void *targetChecking(void *args);
	static void run();
};

#endif /* SYSTEMCOMPONENTDETECTOR_H_ */
