#include "bios-update.hpp"
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <gpiod.hpp>
#include <sdbusplus/bus.hpp>
#include <sdbusplus/exception.hpp>
#include <sdbusplus/message/types.hpp>
#include <sys/ioctl.h>
#include <mtd/mtd-user.h>
#include <array>

constexpr auto SYSTEMD_SERVICE = "org.freedesktop.systemd1";
constexpr auto SYSTEMD_ROOT = "/org/freedesktop/systemd1";
constexpr auto SYSTEMD_INTERFACE = "org.freedesktop.systemd1.Manager";

constexpr auto MONITOR_PROGRESS_SERVICE = "monitor-bios-update@";

constexpr auto MTD_NUM_MAX = 15;
constexpr auto FLASHCP_CNT = 50000;
constexpr auto OP_PAUSE = 1; // unit: sec
constexpr auto EDAF_MODE = true; // superion-ast enable EDAF(SAFS) mode

const std::array <std::string,SPI_PIN_COUNT> spiLineName = {"SPI_BIOS_MUXA_SEL" , "SPI_BIOS_MUXB_SEL"};

int8_t BiosUpdateManager::setBiosMtdDevice(uint8_t state)
{
    int ret = 0;
    std::string spi = BIOS_SPI;
    std::string path = BIOS_DRIVER_PATH;
    int fd;

    switch (state)
    {
    case unbind:
        path = path + "unbind";
        break;
    case bind:
        path = path + "bind";
        break;
    default:
        std::cerr << "Fail to get state failed\n";
        return -1;
    }

    fd = open(path.c_str(), O_WRONLY);
    if (fd < 0)
    {
        std::cerr << "Fail in " << __func__ << "\n";
        return -1;
    }

    write(fd, spi.c_str(), spi.size());
    close(fd);

    return 0;
}

int8_t BiosUpdateManager::biosUpdatePrepare()
{
    int i,ret = 0;
    
    if(!EDAF_MODE) // It is not neccessary in EDAF mode: 1.switch SPI mux 2.mount SPI driver
    {
        unsigned int gpioPin = 0;
        char * chipName = new char [CHIP_BUFFER_SIZE];
        // spi Mux
        for(i=0 ; i<spiLineName.size() ; i++)
        {
            ret = gpiod_ctxless_find_line(spiLineName[i].c_str(), chipName, CHIP_BUFFER_SIZE, &gpioPin);
            if (ret < 0) {
                std::cerr << "Can't find line:" << spiLineName[i] << "\n";
                return ret;
            }else{
                std::cerr << spiLineName[i] << "is found at " << gpioPin << " in " << chipName << "\n";
            }
            // Set BIOS SPI MUX path to BMC (H) 
            gpiod_ctxless_set_value(chipName,             // Label of the gpiochip.
                                    gpioPin,	      // Number of GPIO pin.
                                    1,                    // GPIO set value.
                                    false,                // The active state of this line - true if low.
                                    "bios-update",        // Name of comsumer.
                                    NULL,                 // Callback function.
                                    NULL);                // value passed to callback function.
            sleep(OP_PAUSE);
        }
        delete [] chipName;
     
        ret = setBiosMtdDevice(bind);
        if (ret < 0)
        {
            std::cerr << "Failed in bind mtd partition\n";
            return -1;
        }
        sleep(OP_PAUSE);
    }
    return 0;
}
/*the function start report-update-progress service , only can invoke while object is found*/
int8_t BiosUpdateManager::setMonitorProgress(std::string object, enum service_action action)
{
    if (object == "") {
        return -1;		
    }
    std::string serviceAction="StopUnit";
    /*format of object : "object:/xyz/openbmc_project/software/verId"*/
    std::string match = "software/";
    std::size_t pos =match.size() + object.find(match);
    std::string verId = object.substr(pos);
    BIOS_UPDATE_DEBUG("verId:%s\n", verId.c_str());
    if (action == SERVICE_START) {
	serviceAction="StartUnit";
    } else {
	serviceAction="StoptUnit";
    }    
    auto bus = sdbusplus::bus::new_default();
    auto method = bus.new_method_call(SYSTEMD_SERVICE, SYSTEMD_ROOT,
                                      SYSTEMD_INTERFACE, serviceAction.c_str());
    auto reportService = MONITOR_PROGRESS_SERVICE + verId + ".service";
    method.append(reportService, "replace");
    try
    {
        auto reply = bus.call(method);
    }
    catch (const sdbusplus::exception::SdBusError& e)
    {
        printf("sd bus call fail\n");
        return -1;
    }
    return 0;
}

int8_t BiosUpdateManager::getMtdNum()
{
    int8_t num;
    int8_t i;
    char path[32]; 
    char buf[10]; 
    int fd = 0; 
    int len = 0;

    for (i = 0; i < MTD_NUM_MAX; i++) {    
        memset(path, '\0', sizeof(path));
        memset(buf, 0, sizeof(buf));
        
        snprintf(path, sizeof(path), "/sys/class/mtd/mtd%d/name", i); 
        if ((fd = open(path, O_RDONLY, S_IRWXU)) < 0)  {
            BIOS_UPDATE_DEBUG("fail fd:%d\n", fd);  
	    continue;
	}
        len = read(fd, buf, sizeof(buf));
        if (len > 0) {
	    buf[len-1] = '\0';
            std::string name(buf);
            if (name.compare(BIOS_MTD_NAME) == 0) {
                BIOS_UPDATE_DEBUG("find name %s\n", name.c_str());  
                return i;
            } 	
        } 
    }

    printf("could not find mtd device!\n");  
    return -1;    
} 

int8_t BiosUpdateManager::biosUpdate(const char* image_str)
{
    int ret = 0;
    char buf[128];  
    FILE *pp;  
    int8_t num = getMtdNum();
    uint32_t cnt = 0;
    if (num < 0) {
        return -1;
    }

    std::string mtdNum = std::to_string(num);

    std::string flashcp_str = "/usr/sbin/flashcp -v ";
    /*mtd6: 04000000 00010000 "pnor"*/
    flashcp_str = flashcp_str + image_str + " /dev/mtd" + mtdNum;
     
    printf("start bios-update\n %s\n", flashcp_str.c_str());  
    if ((pp = popen(flashcp_str.c_str(), "r")) == NULL) {  
       printf("popen() error!\n");  
       return -1;
    }  
    while (fgets(buf, sizeof(buf), pp)) {  
        if (cnt++ <= FLASHCP_CNT) {
            cnt = 0;
	    printf("%s", buf); 
	        printf("\nflashcp is running...\n"); 
	    } 
    } 
    
    ret = WEXITSTATUS(pclose(pp)); 
    if (ret == 0) {
        printf("flashcp pass!\n"); 
    } else {
        printf("flashcp fail!\n"); 
    }
    BIOS_UPDATE_DEBUG("Exit code: %d\n", ret);
  
    return ret;
}

int8_t BiosUpdateManager::biosUpdateFinished(const char* image_str)
{
    int i,ret = 0;
 
    sleep(OP_PAUSE);
    if(!EDAF_MODE) //if EDAF mode is not enable, switch SPI mux back to CPU
    {
        ret = setBiosMtdDevice(unbind);
        if (ret < 0)
        {
            std::cerr << "Failed in unbind mtd partition\n";
            return -1;
        }
        sleep(OP_PAUSE);
        
        // SPI MUX A
        unsigned int gpioPin = 0;
        char * chipName = new char [CHIP_BUFFER_SIZE];
        for(i=0 ; i<spiLineName.size() ; i++)
        {
            ret = gpiod_ctxless_find_line(spiLineName[i].c_str(), chipName, CHIP_BUFFER_SIZE, &gpioPin);
            if (ret < 0) {
                std::cerr << "Can't find line:" << spiLineName[i] << "\n";
                return ret;
            }else{
                std::cerr << spiLineName[i] << " is found at " << gpioPin << " in " << chipName << "\n";
            }
            // Set BIOS SPI MUX path to Host (L)
            gpiod_ctxless_set_value(chipName,          // Label of the gpiochip.
                                    gpioPin,     // Number of GPIO pin.
                                    0,                    // GPIO set value.
                                    false,                // The active state of this line - true if low.
                                    "bios-update",       // Name of comsumer.
                                    NULL,                 // Callback function.
                                    NULL);                // value passed to callback function.
            sleep(OP_PAUSE);
        }
        delete [] chipName;
    }
        return 0;
}
