/*
* Copyright (c) 2018-present, aliminabc@gmail.com.
*
* This source code is licensed under the MIT license found in the
* LICENSE file in the root directory of this source tree.
*/

#ifndef FK_FRAMEWORK_FKONCREATEPROT_H
#define FK_FRAMEWORK_FKONCREATEPROT_H

#include "FkProtocol.h"

FK_CLASS FkOnCreatePrt FK_EXTEND FkProtocol {
public:
    FkOnCreatePrt();

    FkOnCreatePrt(const FkOnCreatePrt &o);

    virtual ~FkOnCreatePrt();

};

#endif //FK_FRAMEWORK_FKONCREATEPROT_H