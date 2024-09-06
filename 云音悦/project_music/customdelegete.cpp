#include "customdelegete.h"

customDelegete::customDelegete()
{

}

void customDelegete::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QVariant ret = index.model()->data(index); // index.model()获取表格的数据模型，在通过该模型获取索引为index的数据
    QRect rect = option.rect; // option.rect是表格的矩形，包含了格子在整个表格的位置和矩形的宽高

    // 设置 painter 的渲染提示为抗锯齿
    painter->setRenderHint(QPainter::Antialiasing, true);

    if (index.column() == 4) { // 假设最后一列是图片列
        QImage image;
        if (ret == "未收藏")
        {
            image.load(":/未收藏.png");
        }
        else if (ret == "已收藏")
        {
            image.load(":/收藏.png");
        }

        if (!image.isNull()) {
            // 计算单元格的一半大小
            QSize halfSize = rect.size() / 2;

            // 缩放图片以适应单元格一半大小
            QImage scaledImage = image.scaled(halfSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

            // 计算图片的中心位置
            QPoint center = rect.center() - scaledImage.rect().center();

            // 绘制图片
            painter->drawImage(center, scaledImage);
        }
    }
    else
    {
        // 设置文本对齐方式为居中
        QStyleOptionViewItem opt = option;
        opt.displayAlignment = Qt::AlignCenter;

        // 创建一个临时的 QItemDelegate 对象并调用其 paint 方法
        QItemDelegate delegate;
        delegate.paint(painter, opt, index);
    }
}

