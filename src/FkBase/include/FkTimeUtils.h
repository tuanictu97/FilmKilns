/*
* Copyright (c) 2018-present, aliminabc@gmail.com.
*
* This source code is licensed under the MIT license found in the
* LICENSE file in the root directory of this source tree.
*/

#ifndef FK_BASE_FKTIMEUTILS_H
#define FK_BASE_FKTIMEUTILS_H

#include "FkObject.h"

FK_CLASS FkTimeUtils FK_EXTEND FkObject {
public:
    static int64_t getCurrentTimeUS();

public:
    FkTimeUtils() = delete;

    FkTimeUtils(const FkTimeUtils &o) = delete;

    ~FkTimeUtils() {};
};


#endif //FK_BASE_FKTIMEUTILS_H