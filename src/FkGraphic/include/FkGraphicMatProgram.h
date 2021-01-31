/*
* Copyright (c) 2018-present, aliminabc@gmail.com.
*
* This source code is licensed under the MIT license found in the
* LICENSE file in the root directory of this source tree.
*/

#ifndef FK_GRAPHIC_FKGRAPHICMATPROGRAM_H
#define FK_GRAPHIC_FKGRAPHICMATPROGRAM_H

#include "FkGraphicProgram.h"

FK_CLASS FkGraphicMatProgram FK_EXTEND FkGraphicProgram {
public:
    FkGraphicMatProgram(const FkProgramDescription &desc);

    FkGraphicMatProgram(const FkGraphicMatProgram &o) = delete;

    virtual ~FkGraphicMatProgram();

    virtual std::string getVertex() override;

    virtual std::string getFragment() override;

};

#endif //FK_GRAPHIC_FKGRAPHICMATPROGRAM_H