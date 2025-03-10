/*
The MIT License (MIT)

Copyright © 2018-2023 Antonio Dias (https://github.com/antonypro)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "titlewidget.h"
#include "titlebar.h"

TitleWidget::TitleWidget(TitleBar *title_bar, QWidget *parent) : QWidget(parent)
{
    m_title_bar = title_bar;

    m_active = false;
    m_alignment = Qt::AlignLeft;
}

void TitleWidget::setText(const QString &text)
{
    m_title = text;
    update();
}

void TitleWidget::setActive(bool active)
{
    m_active = active;
    update();
}

void TitleWidget::setTitleAlignment(const Qt::Alignment &alignment)
{
    switch (alignment)
    {
    case Qt::AlignLeft:
    case Qt::AlignCenter:
    case Qt::AlignRight:
    {
        m_alignment = alignment;
        break;
    }
    default:
    {
        m_alignment = Qt::AlignLeft;
        break;
    }
    }

    update();
}

void TitleWidget::setTitleColor(const QColor &active_color, const QColor &inactive_color)
{
    m_active_color = active_color;
    m_inactive_color = inactive_color;

    update();
}

Qt::Alignment TitleWidget::titleAlignment()
{
    return m_alignment;
}

void TitleWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    const int spacing = m_title_bar->layoutSpacing();

    QRect left_rect;
    QRect right_rect;
    QRect center_rect;

    if (m_title_bar->m_icon_widget->isVisible())
        left_rect = left_rect.united(m_title_bar->m_icon_widget->geometry());
    if (m_title_bar->m_left_widget_place_holder->isVisible())
        left_rect = left_rect.united(m_title_bar->m_left_widget_place_holder->geometry());

    if (m_title_bar->m_right_widget_place_holder->isVisible())
        right_rect = right_rect.united(m_title_bar->m_right_widget_place_holder->geometry());
    if (m_title_bar->m_caption_buttons->isVisible())
        right_rect = right_rect.united(m_title_bar->m_caption_buttons->geometry());
    else
        right_rect = right_rect.united(QRect(width(), 0, 1, height()));

    if (m_title_bar->m_center_widget_place_holder->isVisible())
        center_rect = center_rect.united(m_title_bar->m_center_widget_place_holder->geometry());

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    QFont font;
    font.setPixelSize(12);
#ifdef Q_OS_WIN
    font.setFamily("Segoe UI");
#else
    font.setFamily(qApp->font().family());
#endif

    painter.setFont(font);

    QPen pen;
    pen.setColor(m_active ? m_active_color : m_inactive_color);

    painter.setPen(pen);

    bool center_widget_visible = m_title_bar->m_center_widget_place_holder->isVisible();

    Qt::Alignment alignment = m_alignment;

    if (center_widget_visible && alignment == Qt::AlignCenter)
        alignment = Qt::AlignLeft;

    QFontMetrics metrics(painter.font());

    int title_space_width;

    if (center_widget_visible)
    {
        if (alignment == Qt::AlignLeft)
            title_space_width = center_rect.left() - left_rect.right();
        else
            title_space_width = right_rect.left() - center_rect.right();
    }
    else
    {
        title_space_width = right_rect.left() - left_rect.right();
    }

    QString title_elided = metrics.elidedText(m_title, Qt::ElideRight, title_space_width - spacing);

    QSize title_size = metrics.size(0, title_elided);

    int title_width = title_size.width();
    int title_height = title_size.height();

    QRect title_rect = QRect(0, 0, title_width, title_height);

    switch (alignment)
    {
    case Qt::AlignLeft:
    {
        title_rect.setTop((height() - title_height) / 2);

        title_rect.setHeight(title_height);

        title_rect.setLeft(left_rect.right());
        title_rect.setRight(left_rect.right() + title_width);

        break;
    }
    case Qt::AlignRight:
    {
        title_rect.setTop((height() - title_height) / 2);

        title_rect.setHeight(title_height);

        title_rect.setLeft(right_rect.left() - title_width - spacing);
        title_rect.setRight(right_rect.left());

        break;
    }
    case Qt::AlignCenter:
    {
        title_rect.setWidth(title_width + spacing);
        title_rect.setHeight(title_height);

        title_rect.moveTop((height() - title_height) / 2);
        title_rect.moveLeft((width() - title_width) / 2);

        bool left_collide = (title_rect.left() < left_rect.right());

        bool right_collide = (title_rect.right() > right_rect.left());

        if (left_collide || (m_title != title_elided))
        {
            title_rect.moveLeft(left_rect.right());
        }
        else if (right_collide)
        {
            title_rect.moveRight(right_rect.left());
        }

        break;
    }
    default:
        break;
    }

    painter.drawText(title_rect, title_elided);
}
