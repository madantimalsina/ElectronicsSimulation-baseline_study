//
//  DeviceFactory.cpp
//  devices
//
//  Created by Cees Carels on 14/08/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#include "DeviceFactory.hpp"

const std::map<std::string, DeviceFactory::DEVICE_TYPE> DeviceFactory::deviceTypeMap = { { "PMT", ePMT }, { "Cable", eCABLE }, { "Amplifier", eAMPLIFIER }, { "Digitizer", eDIGITIZER }, { "None", eNONE } };

DeviceFactory::DeviceFactory()
{
    /**
     * Constructor for DeviceFactory.
     */
}

DeviceFactory::~DeviceFactory()
{
    /**
     * Destructor for DeviceFactory.
     */
}

DeviceFactory::DEVICE_TYPE DeviceFactory::returnDeviceType(std::string deviceType)
{
    if (deviceTypeMap.find(deviceType) == deviceTypeMap.end())
    {
        std::cout << "NOTICE: Device type not found - return null device" << std::endl;
        return DeviceFactory::DEVICE_TYPE::eNONE;
    }
    else
        return deviceTypeMap.find(deviceType)->second;
}

std::shared_ptr<Device> DeviceFactory::getDevice(std::string stringType, der::DeviceModel model)
{

    std::shared_ptr<Device> device;
    DEVICE_TYPE type = returnDeviceType(stringType);
    switch (type)
    {
    case DEVICE_TYPE::ePMT:
        return std::make_shared<PMT>(model);
    case DEVICE_TYPE::eCABLE:
        return std::make_shared<Cable>(model);
    case DEVICE_TYPE::eAMPLIFIER:
        return std::make_shared<Amplifier>(model);
    case DEVICE_TYPE::eDIGITIZER:
        return std::make_shared<Digitizer>(model);
    }
}
