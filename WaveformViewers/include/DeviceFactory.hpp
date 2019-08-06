//
//  DeviceFactory.hpp
//  devices
//
//  Created by Cees Carels on 14/08/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#ifndef DeviceFactory_hpp
#define DeviceFactory_hpp

#include "Amplifier.hpp"
#include "Cable.hpp"
#include "Digitizer.hpp"
#include "FPGATrigger.hpp"
#include "PMT.hpp"
#include <map>
#include <stdio.h>
#include <string>

//Base class required for factory method
#include "Device.hpp"

class DeviceFactory
{
public:
    DeviceFactory();
    ~DeviceFactory();

    //Signal chain types.
    enum DEVICE_TYPE
    {
        ePMT,
        eCABLE,
        eAMPLIFIER,
        eDIGITIZER,
        eFPGATRIGGER,
        eNONE
    };

    static const std::map<std::string, DEVICE_TYPE> deviceTypeMap;
  static std::shared_ptr<Device> getDevice(std::string stringType, der::DeviceModel model);
    static DEVICE_TYPE returnDeviceType(std::string stringType);
};

#endif /* DeviceFactory_hpp */
