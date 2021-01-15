/*
 * SystemConfigurations.h
 *
 *  Created on: Sep 23, 2017
 *      Author: hungnv103
 */

#ifndef SYSTEMCONFIGURATIONS_H_
#define SYSTEMCONFIGURATIONS_H_

#include <string>
#include <iostream>

class SystemConfigurations {
public:
	void init(std::string configFilePath);
	virtual ~SystemConfigurations();

	static SystemConfigurations *getInstance();
    std::string getHostIp() const;

//    void setHostIp(std::string mHostIp);

    std::string getPort() const;

    void setPort(std::string mPort);
//	std::string GatewayIp;
	std::string GatewayPort;

	unsigned char getFindChar();
	int isVideo();
	int get_line_size();
	int get_num_of_line();
	int get_normal_color();
	int get_iff_color();

private:
	SystemConfigurations();

private:
//	std::string mHostIp;
	std::string mPort;
	int find_char;
	int enable_video; // enable send video through tcp ip and save to write_data file in /root/minh_data in QNX 6.4
	int num_of_line;
	int line_size;

	int normal_color;
	int iff_color;

};

#endif /* SYSTEMCONFIGURATIONS_H_ */
