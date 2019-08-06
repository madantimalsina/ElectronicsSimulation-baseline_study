//
//  DataSparsifier.hpp
//  devices
//
//  Created by Cees Carels on 13/03/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#ifndef DataSparsifier_hpp
#define DataSparsifier_hpp

#include <stdio.h>

/**
 * Base class for Trigger and POD class interface.
 */

class DataSparsifier
{
public:
    DataSparsifier();
    virtual ~DataSparsifier() = 0;

private:
};

#endif /* DataSparsifier_hpp */