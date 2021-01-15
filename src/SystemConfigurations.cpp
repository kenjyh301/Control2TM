/*
 * SystemConfigurations.cpp
 *
 *  Created on: Sep 23, 2017
 *      Author: hungnv103
 */

#include "SystemConfigurations.h"
#include "ini_parser/dictionary.h"
#include "ini_parser/iniparser.h"

SystemConfigurations::SystemConfigurations() {
	// TODO Auto-generated constructor stub

}

SystemConfigurations::~SystemConfigurations() {
	// TODO Auto-generated destructor stub
}

void SystemConfigurations::init(std::string configFilePath){
	///Reading configuration file here
	dictionary * ini;
	ini = iniparser_load(configFilePath.c_str());
	if(ini == NULL ){
		std::cout<<"Could not found the configure file at: "<<configFilePath<<std::endl;
		exit(EXIT_FAILURE);
	}
//	const char ipKey[] = "connection:tcp_ip";
	const char portKey[] = "connection:tcp_port";

//	const char gatewayIpKey[] = "connection:gateway_ip";
//	const char gatewayPortKey[] = "connection:gateway_port";

//	char *ip = iniparser_getstring(ini, ipKey, "0.0.0.0");
	char *port = iniparser_getstring(ini, portKey, "0.0.0.0");

//	char *gatewayIp = iniparser_getstring(ini, gatewayIpKey, "0.0.0.0");
//	char *gatewayPort = iniparser_getstring(ini, gatewayPortKey, "0.0.0.0");

//	mHostIp = std::string(ip);
	mPort = std::string(port);
//	GatewayIp = std::string (gatewayIp);
//	GatewayPort = std::string ( gatewayPort);
	std::cout<<" Port - "<<mPort<<std::endl;
//	std::cout<<"Read configure file: Ip - "<<mHostIp<<" Port - "<<mPort<<std::endl;
//	std::cout<<"Read configure file: Ip - "<<GatewayIp<<" Port - "<<GatewayPort<<std::endl;

	find_char = iniparser_getint(ini, "feature:find_char", 253);
	std::cout<<"Specify exercise target value:"<<find_char<<std::endl;

	enable_video = iniparser_getint(ini, "feature:enable_video",0);
	std::cout<<"Video sending through tcp:"<<((enable_video==1)?"Yes":"No")<<std::endl;

	line_size = iniparser_getint(ini,"feature:line_size",150);
	std::cout<<"line_size: "<<line_size<<std::endl;
	num_of_line = iniparser_getint(ini,"feature:num_of_line",1);
	std::cout<<"num_of_line: "<<num_of_line<<std::endl;

}

SystemConfigurations *SystemConfigurations::getInstance()
{
	static SystemConfigurations instance;
	return &instance;
}

//std::string SystemConfigurations::getHostIp() const
//{
//	return mHostIp;
//}

//void SystemConfigurations::setHostIp(std::string mHostIp)
//{
//	this->mHostIp = mHostIp;
//}

std::string SystemConfigurations::getPort() const
{
	return mPort;
}

unsigned char SystemConfigurations::getFindChar(){
	return (unsigned char)find_char;
}

int SystemConfigurations::isVideo(){
	return enable_video;
}

void SystemConfigurations::setPort(std::string mPort)
{
	this->mPort = mPort;
}

int SystemConfigurations::get_line_size(){
	return line_size;
}

int SystemConfigurations::get_num_of_line(){
	return num_of_line;
}
