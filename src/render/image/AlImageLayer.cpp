/*
* Copyright (c) 2018-present, aliminabc@gmail.com.
*
* This source code is licensed under the MIT license found in the
* LICENSE file in the root directory of this source tree.
*/

#include "AlImageLayer.h"
#include "HwTexture.h"
#include "AlVec4.h"
#include "Logcat.h"

#define TAG "AlImageLayer"

AlImageLayer *AlImageLayer::create(AlImageLayerModel *model, HwAbsTexture *tex) {
    return new AlImageLayer(model, tex);
}

AlImageLayer::AlImageLayer(AlImageLayerModel *model, HwAbsTexture *tex) : Object() {
    this->model = model;
    this->tex = tex;
}

AlImageLayer::~AlImageLayer() {
    model = nullptr;
}

int32_t AlImageLayer::getWidth() {
    return this->tex->getWidth();
}

int32_t AlImageLayer::getHeight() {
    return this->tex->getHeight();
}

HwResult AlImageLayer::measure(AlImageLayerDrawModel &drawModel) {
    AlSize canvasSize = drawModel.getCanvasSize();
    AlSize src(this->tex->getWidth(), this->tex->getHeight());
    /// 对图层和画布进行正交投影计算，转换坐标系，保证图像旋转缩放不会变形，并得到归一化的区域
    aMeasure.updateOrthogonal(src, canvasSize);
    aMeasure.setScale(model->getScale().x, model->getScale().y);
    aMeasure.setRotation(model->getRotation());
    ///TODO 矩阵Y轴与正常坐标系Y轴相反
    aMeasure.setTranslate(model->getPosition().x, -model->getPosition().y);
    aMeasure.measure(drawModel);
    drawModel.tex = HwTexture::wrap(dynamic_cast<HwTexture *>(tex));
    drawModel.alpha = model->getAlpha();
    ///Update quad.
    AlVec2 lt;
    AlVec2 lb;
    AlVec2 rb;
    AlVec2 rt;
    ///获得经过位移旋转缩放变换后图像的位置坐标
    aMeasure.getTransLORectF(lt, lb, rb, rt);
    model->setQuad(lt, lb, rb, rt);
    ///TODO 这里需要把Y轴翻转一次
    model->getQuad().mirrorVertical();
    return Hw::SUCCESS;
}
