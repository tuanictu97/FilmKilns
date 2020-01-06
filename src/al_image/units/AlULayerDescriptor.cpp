/*
* Copyright (c) 2018-present, aliminabc@gmail.com.
*
* This source code is licensed under the MIT license found in the
* LICENSE file in the root directory of this source tree.
*/

#include "AlULayerDescriptor.h"
#include "AlImageLayerDrawModel.h"
#include "AlMath.h"
#include "HwTexture.h"
#include "ObjectBox.h"
#include "AlImgLayerDescription.h"
#include "AlAbsMAction.h"
#include "AlMMosaicAction.h"

#define TAG "AlULayerDescriptor"

AlULayerDescriptor::AlULayerDescriptor(const string &alias) : Unit(alias) {
    registerEvent(EVENT_LAYER_MEASURE, reinterpret_cast<EventFunc>(&AlULayerDescriptor::onMeasure));
    registerEvent(EVENT_LAYER_MEASURE_CANVAS_SIZE,
                  reinterpret_cast<EventFunc>(&AlULayerDescriptor::onCanvasSizeUpdate));
}

AlULayerDescriptor::~AlULayerDescriptor() {

}

bool AlULayerDescriptor::onCreate(AlMessage *msg) {
    return true;
}

bool AlULayerDescriptor::onDestroy(AlMessage *msg) {
    return true;
}

bool AlULayerDescriptor::onMeasure(AlMessage *msg) {
    AlImageLayer *layer = msg->getObj<ObjectBox *>()->unWrap<AlImageLayer *>();
    AlImageLayerDrawModel *description = new AlImageLayerDrawModel();
    _measure(layer, description);
    notifyCanvas(description);
    return true;
}

bool AlULayerDescriptor::onCanvasSizeUpdate(AlMessage *msg) {
    aCanvasSize.width = msg->arg1;
    aCanvasSize.height = static_cast<int>(msg->arg2);
    return true;
}

HwResult AlULayerDescriptor::_measure(AlImageLayer *layer, AlImageLayerDrawModel *description) {
    if (nullptr == layer || nullptr == layer->model || nullptr == description) {
        return Hw::FAILED;
    }
    AlSize layerSize(layer->getWidth(), layer->getHeight());
    AlSize canvasSize = aCanvasSize;
    ///默认画布大小为最先添加的图层的大小
    if (canvasSize.width <= 0 || canvasSize.height <= 0) {
        canvasSize.width = layerSize.width;
        canvasSize.height = layerSize.height;
    }
    ///Copy一份layer model送入opt进行测量，在测量过程中opt可能会改变model数据
    AlImgLayerDescription model(*(layer->model));
    model.setSize(layerSize);
    HwResult ret = _measureOperate(layer->model->getAllOperators(), model, description);
    description->setLayerSize(model.getSize());
    ///经过各种各样的Operate后，layer size会被改变并更新到AlImageLayerDrawModel
    ///这里需要获取最新的layer size，不然会出错
    ///必须裁剪Operate会改变layer size，如果不更新则可能出现图像拉伸
    AlSize src = description->getLayerSize();
    /// 对图层和画布进行正交投影计算，转换坐标系，保证图像旋转缩放不会变形，并得到归一化的区域
    aMeasurer.updateOrthogonal(src, canvasSize);
    /// 缩放旋转位移顺序不能乱
    aMeasurer.setScale(model.getScale().x, model.getScale().y);
    aMeasurer.setRotation(static_cast<float>(model.getRotation().toFloat() * AlMath::PI));
    ///TODO 矩阵Y轴与正常坐标系Y轴相反
    aMeasurer.setTranslate(model.getPosition().x, -model.getPosition().y);
    aMeasurer.measure(*description);
    description->tex = HwTexture::wrap(dynamic_cast<HwTexture *>(layer->getTexture()));
    description->alpha = model.getAlpha();
    ///Update quad.
    AlVec2 lt;
    AlVec2 lb;
    AlVec2 rb;
    AlVec2 rt;
    ///获得经过位移旋转缩放变换后图像的位置坐标
    aMeasurer.measureTransLORectF(lt, lb, rb, rt);
    layer->model->setQuad(lt, lb, rb, rt);
    ///TODO 这里需要把Y轴翻转一次
    layer->model->getQuad().mirrorVertical();
    Logcat::i(TAG, "tran %f, %f", model.getPosition().x, model.getPosition().y);
    Logcat::i(TAG, "rect (%f,%f), (%f,%f)", lt.x, lt.y, rt.x, rt.y);
    Logcat::i(TAG, "rect (%f,%f), (%f,%f)", lb.x, lb.y, rb.x, rb.y);
    auto *actions = layer->model->getAllOperators();
    size_t size = actions->size();
    for (int i = 0; i < size; ++i) {
        AlAbsMAction *action = (*actions)[i];
        if (typeid(AlMMosaicAction) == typeid(*action)) {
            description->mosaicPath =dynamic_cast<AlMMosaicAction *>(action)->getPath();
        }
    }
    return ret;
}

HwResult AlULayerDescriptor::_measureOperate(std::vector<AlAbsMAction *> *opts,
                                            AlImgLayerDescription &model,
                                            AlImageLayerDrawModel *description) {
    if (nullptr == description) {
        return Hw::FAILED;
    }
    for (auto *opt : *opts) {
        if (opt) {
            opt->setCanvasSize(aCanvasSize);
            opt->measure(model, description);
        }
    }
    return Hw::SUCCESS;
}

void AlULayerDescriptor::notifyCanvas(AlImageLayerDrawModel *description) {
    AlMessage *msg = AlMessage::obtain(EVENT_LAYER_RENDER_DRAW, description,
                                       AlMessage::QUEUE_MODE_FIRST_ALWAYS);
    postEvent(msg);
}