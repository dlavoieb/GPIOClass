
#include "GPIOClass.h"
#include <exception>
#include <stdexcept>

using namespace std;

GPIOClass::GPIOClass():valuefd(-1),directionfd(-1),muxfd(-1),exportfd(-1),unexportfd(-1),gpionum("4")
{
        //GPIO4 is default
	try {
		this->unexport_gpio();
	}
	catch (runtime_error&) {};
	this->export_gpio();
}

GPIOClass::GPIOClass(string gnum):valuefd(-1),directionfd(-1),muxfd(-1),exportfd(-1),unexportfd(-1),gpionum(gnum)
{
	//Instatiate GPIOClass object for GPIO pin number "gnum"
	try {
		this->unexport_gpio();
	}
	catch (runtime_error&) {};
	this->export_gpio();
}

GPIOClass::~GPIOClass()
{
	this->unexport_gpio();
}


int GPIOClass::export_gpio()
{
	int statusVal = -1;
	string exportStr = "/sys/class/gpio/export";
	this->exportfd = statusVal = open(exportStr.c_str(),  O_WRONLY|O_SYNC);
	if (statusVal < 0){
		throw runtime_error("GPIOClass::export_gpio: could not open SYSFS GPIO export device");
        //exit(1);
	}
	
	stringstream ss;
	ss << this->gpionum;
	string numStr = ss.str();
	statusVal = write(this->exportfd, numStr.c_str(), numStr.length());
	if (statusVal < 0){
		throw runtime_error("GPIOClass::export_gpio: could not write to SYSFS GPIO export device");
        //exit(1);
	}
	
	statusVal = close(this->exportfd);
	if (statusVal < 0){
		throw runtime_error("GPIOClass::export_gpio: could not close SYSFS GPIO export device");
        //exit(1);
	}

    return statusVal;
}

int GPIOClass::unexport_gpio()
{
	int statusVal = -1;
	string unexportStr = "/sys/class/gpio/unexport";
	this->unexportfd = statusVal = open(unexportStr.c_str(),  O_WRONLY|O_SYNC);
	if (statusVal < 0){
		throw runtime_error("GPIOClass::uexport_gpio: could not open SYSFS GPIO unexport device");
        //exit(1);
	}

	stringstream ss;
	ss << this->gpionum;
	string numStr = ss.str();
	statusVal = write(this->unexportfd, numStr.c_str(), numStr.length());
	if (statusVal < 0){
		throw runtime_error("GPIOClass::uexport_gpio: could not write to SYSFS GPIO unexport device");
        //exit(1);
	}
	
	statusVal = close(this->unexportfd);
	if (statusVal < 0){
		throw runtime_error("GPIOClass::uexport_gpio: could not close SYSFS GPIO unexport device");
        //exit(1);
	}
	
	return statusVal;
}

int GPIOClass::setdir_gpio(string dir)
{
	int statusVal = -1;
	string setdirStr ="/sys/class/gpio/gpio" + this->gpionum + "/direction";
	
	
	this->directionfd = statusVal = open(setdirStr.c_str(), O_WRONLY|O_SYNC); // open direction file for gpio
		if (statusVal < 0){
			throw runtime_error("GPIOClass::setdir_gpio: could not open SYSFS GPIO direction device");
        //exit(1);
		}
		
	if (dir.compare("in") != 0 && dir.compare("out") != 0 && dir.compare("high") != 0 && dir.compare("low") != 0 ) {
		//fprintf(stderr, "Invalid direction value. Should be \"in\" or \"out\". \n");
		throw runtime_error("GPIOClass::setdir_gpio: Invalid direction value. Shoudl be \"in\" or \"out\".");
		//exit(1);
	}
		
	statusVal = write(this->directionfd, dir.c_str(), dir.length());
	if (statusVal < 0){
		throw runtime_error("GPIOClass::setdir_gpio: could not write to SYSFS GPIO direction device");
        //exit(1);
	}
	
	statusVal = close(this->directionfd);
	if (statusVal < 0){
		throw runtime_error("GPIOClass::setdir_gpio: could not close SYSFS GPIO direction device");
        //exit(1);
	}

	    return statusVal;
}


int GPIOClass::set_gpio_mux(string mode)
{
	int statusVal = -1;
	string mode_string = "/sys/kernel/debug/gpio_debug/gpio" + gpionum + "/current_pinmux";

	mixfd = statusVal = open(mode_string.c_str(), O_WRONLY|O_SYNC); 
	if (statusVal < 0){
			throw runtime_error("GPIOClass::setdir_gpio: could not open SYSFS GPIO direction device");
	}

	statusVal = write(mixfd, mode.c_str(), mode.length());
	if (statusVal < 0){
		throw runtime_error("GPIOClass::setdir_gpio: could not write to SYSFS GPIO direction device");
	}
	
	statusVal = close(mixfd);
	if (statusVal < 0){
		throw runtime_error("GPIOClass::setdir_gpio: could not close SYSFS GPIO direction device");
	}
    return statusVal;	

}


int GPIOClass::setval_gpio(string val)
{

    int statusVal = -1;
	string setValStr = "/sys/class/gpio/gpio" + this->gpionum + "/value";
	
	this->valuefd = statusVal = open(setValStr.c_str(), O_WRONLY|O_SYNC);
	if (statusVal < 0){
		throw runtime_error("GPIOClass::setval_gpio: could not open SYSFS GPIO value device");
	}
		
	if (val.compare("1") != 0 && val.compare("0") != 0 ) {
		throw runtime_error("GPIOClass::setval_gpio: invalid value - Should be 1 or 0");
	}
		
	statusVal = write(this->valuefd, val.c_str(), val.length());
	if (statusVal < 0){
		throw runtime_error("GPIOClass::setval_gpio: could not write to SYSFS GPIO value device");
	}
	
	statusVal = close(this->valuefd);
	if (statusVal < 0){
		throw runtime_error("GPIOClass::setval_gpio: could not close SYSFS GPIO value device");
	}

	    return statusVal;
}


int GPIOClass::getval_gpio(string& val){

	string getValStr = "/sys/class/gpio/gpio" + this->gpionum + "/value";
	char buff[10];
	int statusVal = -1;
	this->valuefd = statusVal = open(getValStr.c_str(), O_RDONLY|O_SYNC);
	if (statusVal < 0){
		throw runtime_error("GPIOClass::getval_gpio: could not open SYSFS GPIO value device");
	}

	statusVal = read(this->valuefd, &buff, 1);
	if (statusVal < 0){
		throw runtime_error("GPIOClass::getval_gpio: could not read SYSFS GPIO value device");
	}
	
	buff[1]='\0';
	
	val = string(buff);
	
	if (val.compare("1") != 0 && val.compare("0") != 0 ) {
		throw runtime_error("GPIOClass::getval_gpio: Ivalid value read");
	}
	
	statusVal = close(this->valuefd);
	if (statusVal < 0){
		throw runtime_error("GPIOClass::getval_gpio: could not close SYSFS GPIO value device");
	}

	return statusVal;
}


string GPIOClass::get_gpionum(){

return this->gpionum;

}
