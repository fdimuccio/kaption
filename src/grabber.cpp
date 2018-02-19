#include "grabber.h"

#include <QTimer>
#include <QApplication>
#include <QDesktopWidget>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QColor>
#include <QPoint>
#include <QDebug>
#include <QPen>
#include <QPushButton>
#include <KLocale>
#include <KWindowSystem>
#include <KWindowInfo>
#include <KIconEffect>

static void lf_getTopLevelWindowsInfo(QLinkedList<KWindowInfo> &tlwInfo)
{
    tlwInfo.clear();
    QList<WId> windows = KWindowSystem::stackingOrder();
    QList<WId>::ConstIterator it;
    for ( it = windows.begin(); it != windows.end(); ++it ) {
        if(!KWindowSystem::hasWId((*it))) {
            continue;
        }
        KWindowInfo info(*it,
                         NET::WMVisibleIconName |
                         NET::WMFrameExtents |
                         NET::WMState |
                         NET::XAWMState |
                         NET::WMGeometry |
                         NET::WMName |
                         NET::WMDesktop);
        if (!info.isMinimized() && info.isOnCurrentDesktop()) {
            tlwInfo.append(info);
        }
    }
}

Grabber::Grabber(QWidget *parent) :
    QWidget(parent, Qt::X11BypassWindowManagerHint |
                    Qt::WindowStaysOnTopHint |
                    Qt::FramelessWindowHint |
                    Qt::Tool )
{
    resetGrabberState();
    setMouseTracking(true);
    resize(0, 0);
}

/**
  @todo: This function is ugly. It should be done in a nicer way,
         actually it is like that: showing -> hiding -> screenshot -> showing.
         The main problem is that i can not always intercept left click on the
         systray (blame KStatusNotifierItem, DBus or me :) ) so i must hook on showEvent
*/
void Grabber::init()
{
    resetGrabberState();

    lf_getTopLevelWindowsInfo(m_tlwInfo);

    m_shooting = true;
    m_pixmap = QPixmap::grabWindow(QApplication::desktop()->winId());
    QImage bw = m_pixmap.toImage();
    KIconEffect::toGray(bw, 1);
    m_bwPixmap = QPixmap::fromImage(bw);
    resize(m_pixmap.size());
    move(0, 0);
    setCursor(Qt::CrossCursor);
    show();
    m_shooting = false;

    grabMouse();
    grabKeyboard();
}

void Grabber::resetGrabberState()
{
    m_selection = QRect();
    m_mouseDown = false;
    m_newSelection = false;
    m_dragStartPoint = QPoint();
    m_selectionBeforeDrag = QRect();
    m_mouseOverHandle = 0;
    m_crossCenter = QCursor::pos();
    m_shooting = false;
    m_crossCenterPos = Unknown;
    m_selectedWindow = 0;
    m_pointedWindow = 0;
    m_windowSelection = QRect();
}

void Grabber::showEvent(QShowEvent *e)
{
    if (!m_shooting) {
        hide();
        QTimer::singleShot(50, this, SLOT(init()));
    }
    QWidget::showEvent(e);
}

void Grabber::hideEvent(QHideEvent *e)
{
    if (!m_shooting) {
        releaseKeyboard();
        releaseMouse();
    }
    QWidget::hideEvent(e);
}

void Grabber::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Escape:
        Q_EMIT quitRequested();
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        grabRect();
        break;
    case Qt::Key_Left:
        translateSelection(-2, 0);
        break;
    case Qt::Key_Up:
        translateSelection(0, -2);
        break;
    case Qt::Key_Right:
        translateSelection(2, 0);
        break;
    case Qt::Key_Down:
        translateSelection(0, 2);
        break;
    default:
        e->ignore();
    }
}

void Grabber::grabRect()
{
    if (!m_selection.isNull() && m_selection.isValid()) {
        Q_EMIT regionGrabbed(m_pixmap.copy(m_selection), m_selection.topLeft());
    } else if (m_selectedWindow != 0) {
        if (KWindowSystem::hasWId(m_selectedWindow->win())) { // Does the window still exist?
            hide();
            KWindowSystem::raiseWindow(m_selectedWindow->win());
            QTimer::singleShot(50, this, SLOT(grabWindow()));
        } else { // Nope, window has gone away, use actual pixmap
            Q_EMIT regionGrabbed(m_pixmap.copy(m_windowSelection), m_windowSelection.topLeft());
        }
    }
}

void Grabber::grabWindow()
{
    QPixmap p = QPixmap::grabWindow(QApplication::desktop()->winId());
    Q_EMIT regionGrabbed(p.copy(m_windowSelection), m_windowSelection.topLeft());
}

void Grabber::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) { // Left button: start selection

        // If a window has been selected
        // deselect it if the mouse moves outside
        if (m_selectedWindow != 0) {
            if (!m_selectedWindow->frameGeometry().contains(e->pos())) {
                m_pointedWindow = 0;
                m_selectedWindow = 0;
            }
        }

        // Start tracking rectangular selection
        m_mouseDown = true;
        m_dragStartPoint = e->pos();
        m_selectionBeforeDrag = m_selection;
        if (!m_selection.contains(e->pos())) {
            m_newSelection = true;
            m_selection = QRect();
        } else {
            setCursor(Qt::ClosedHandCursor);
        }
    } else if (e->button() == Qt::RightButton) { // Right button: cancel selection
        m_selectedWindow = 0;
        m_newSelection = false;
        m_selection = QRect();
        m_hTL = QRect();
        m_hTR = QRect();
        m_hBL = QRect();
        m_hBR = QRect();
        setCursor(Qt::CrossCursor);
        updatePointedWindow(e->pos());
        m_crossCenter = e->pos();
    }
    //updatePointedWindow(e->pos());
    //m_crossCenter = e->pos();
    update();
}

void Grabber::mouseMoveEvent(QMouseEvent *e)
{
    if (m_mouseDown) {
        m_pointedWindow = 0;
        m_selectedWindow = 0;
        if (m_newSelection) {
            QPoint p = e->pos();
            QRect r = rect();
            m_selection = normalizeSelection(QRect(m_dragStartPoint, limitPointToRect(p, r)));
            m_crossCenter = p;
        } else {
            if (!m_mouseOverHandle) {
                QRect r = rect().normalized();
                QRect s = m_selectionBeforeDrag.normalized();
                QPoint p = s.topLeft() + e->pos() - m_dragStartPoint;
                r.setBottomRight(r.bottomRight() - QPoint(s.width(), s.height()) + QPoint(1, 1));
                if (!r.isNull() && r.isValid()) {
                    m_selection.moveTo(limitPointToRect(p, r));
                }
            } else {
                QRect r = m_selectionBeforeDrag;
                QPoint offset = e->pos() - m_dragStartPoint;
                if (m_mouseOverHandle == &m_hTL || m_mouseOverHandle == &m_hTR) {
                    r.setTop(r.top() + offset.y());
                }
                if (m_mouseOverHandle == &m_hTL || m_mouseOverHandle == &m_hBL) {
                    r.setLeft(r.left() + offset.x());
                }
                if (m_mouseOverHandle == &m_hTR || m_mouseOverHandle == &m_hBR) {
                    r.setRight(r.right() + offset.x());
                }
                if (m_mouseOverHandle == &m_hBL || m_mouseOverHandle == &m_hBR) {
                    r.setBottom(r.bottom() + offset.y());
                }
                r.setTopLeft(limitPointToRect(r.topLeft(), rect()));
                r.setBottomRight(limitPointToRect(r.bottomRight(), rect()));
                m_selection = normalizeSelection(r);
            }
            updateCrossCenterPosition();
        }
        updateHandles();
        update();
    } else if (!m_selection.isNull()) {
        QCursor cursor = Qt::CrossCursor;
        m_mouseOverHandle = 0;
        if (m_selection.contains(e->pos())) {
            if (m_hTL.contains(e->pos())) {
                cursor = Qt::SizeFDiagCursor;
                m_mouseOverHandle = &m_hTL;
            } else if (m_hTR.contains(e->pos())) {
                cursor = Qt::SizeBDiagCursor;
                m_mouseOverHandle = &m_hTR;
            } else if (m_hBL.contains(e->pos())) {
                cursor = Qt::SizeBDiagCursor;
                m_mouseOverHandle = &m_hBL;
            } else if (m_hBR.contains(e->pos())) {
                cursor = Qt::SizeFDiagCursor;
                m_mouseOverHandle = &m_hBR;
            } else {
                cursor = Qt::OpenHandCursor;
            }
        }
        setCursor(cursor);
    } else if (m_selectedWindow == 0) { // No rectangular selection, highlight window under cursor
        updatePointedWindow(e->pos());
        m_crossCenter = e->pos();
        update();
    }
}

void Grabber::mouseReleaseEvent(QMouseEvent *e)
{
    if (m_mouseOverHandle) {
        if (m_hTL.contains(e->pos()) || m_hBR.contains(e->pos())) {
            setCursor(Qt::SizeFDiagCursor);
        } else if (m_hTR.contains(e->pos()) || m_hBL.contains(e->pos())) {
            setCursor(Qt::SizeBDiagCursor);
        }
    } else if (m_selection.contains(e->pos())) { // Rectangular selection is finished
        setCursor(Qt::OpenHandCursor);
        if (m_crossCenter == m_selection.topLeft()) {
            m_crossCenterPos = TopLeft;
        } else if (m_crossCenter == m_selection.topRight()) {
            m_crossCenterPos = TopRight;
        } else if (m_crossCenter == m_selection.bottomLeft()) {
            m_crossCenterPos = BottomLeft;
        } else if (m_crossCenter == m_selection.bottomRight()) {
            m_crossCenterPos = BottomRight;
        }
    } else if (m_selection.isNull()) { // No rectangular selection
        if (e->button() == Qt::LeftButton) { // Left click: select window under cursor
            if (m_pointedWindow != 0) {
                m_selectedWindow = m_pointedWindow;
                m_crossCenter = m_windowSelection.topLeft();
                m_crossCenterPos = TopLeft;
            }
        }
    }

    // If no rectangular selection and no window has been selected, show cross cursor lines
    if (m_selection.isNull() && m_selectedWindow == 0) {
        m_crossCenterPos = Unknown;
        m_crossCenter = e->pos();
        setCursor(Qt::CrossCursor);
    }

    m_mouseDown = false;
    m_newSelection = false;

    update();
}
void Grabber::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);

    QRect selection;
    bool shouldDrawHandles = false;
    if (!m_selection.isNull()) {
        selection = m_selection;
        shouldDrawHandles = true;
    } else if (m_pointedWindow != 0) {
        selection = m_windowSelection;
    }

    QPainter painter(this);
    painter.drawPixmap(0, 0, m_pixmap);

    QColor overlayColor(0, 0, 0, 160);
    QColor linesColor(113, 207, 66);

    drawBoundingSelection(&painter, selection, linesColor);
    if (shouldDrawHandles) {
        drawHandles(&painter, linesColor);
    }
    QRegion selected(rect());
    selected = selected.subtracted(selection);
    painter.setClipRegion(selected);
    painter.setPen(Qt::NoPen);
    painter.setBrush(overlayColor);
    painter.drawPixmap(0, 0, m_bwPixmap);
    painter.drawRect(rect());
    painter.setClipRect(rect());
    drawSelectionRect(&painter, selection);

    if (!m_selectedWindow) {
        drawCrosslines(&painter, rect(), m_crossCenter, linesColor);
    }

    drawInstructionInfoRect(&painter);

    drawGrabInfoRect(&painter, selection);
}

void Grabber::drawBoundingSelection(QPainter *painter,
                                    const QRect &rect,
                                    const QColor &color)
{
    painter->save();
    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter->setPen(QPen(color, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawRoundedRect(rect, 5, 5);
    painter->restore();
}

void Grabber::drawSelectionRect(QPainter *painter, const QRect &r)
{
    QRegion clip(r);
    clip = clip.subtracted(r);

    painter->save();
    painter->setClipRegion(clip);
    painter->setPen(Qt::NoPen);
    painter->drawRect(r);
    painter->restore();
}

void Grabber::drawCrosslines(QPainter *painter,
                           const QRect &rect,
                           const QPoint &point,
                           const QColor &color)
{
    painter->save();
    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter->setPen(QPen(color, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawLine(QPointF(point.x(), 0), QPointF(point.x(), rect.height()));
    painter->drawLine(QPointF(0, point.y()), QPointF(rect.width(), point.y()));
    painter->restore();
}

void Grabber::drawHandles(QPainter *painter, const QColor &color)
{
    QRect rects[] = {m_hTL, m_hTR, m_hBL, m_hBR};

    painter->save();
    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    QColor bColor(color);
    bColor.setAlpha(100);
    painter->setBrush(QBrush(bColor, Qt::SolidPattern));
    painter->setPen(QPen(color, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawRects(rects, 4);
    painter->restore();
}

void Grabber::drawTextIntoRect(QPainter *painter,
                               const QPixmap &pixmap,
                               const QString &text,
                               const QPoint &point,
                               const QPoint &offset,
                               const QRect &containerRect)
{
    int margin = !pixmap.isNull() ? 7 : 0;

    QFont font;
    font.setBold(true);
    font.setPointSize(8);
    QFontMetrics fm(font);

    QRect br = fm.boundingRect(QRect(), Qt::AlignLeft, text);
    br.moveTo(point + QPoint(pixmap.width() + margin + offset.x(), offset.y()));

    int w = br.width() + pixmap.width() + margin;
    int h = pixmap.height() > br.height() ? pixmap.height() : br.height();
    QRect infoRect(point + offset, QSize(w, h));
    br.setHeight(h);

    infoRect.adjust(-6, -6, 6, 6);

    int dx = 0;
    int dy = 0;
    if (!containerRect.isNull() && !containerRect.contains(infoRect, true)) {
        dx = infoRect.right() - containerRect.right();
        dy = infoRect.bottom() - containerRect.bottom();
        if (dx < 0) dx = 0;
        if (dy < 0) dy = 0;
        infoRect.translate(-dx, -dy);
        br.translate(-dx, -dy);
    }

    painter->save();
    painter->setRenderHints(QPainter::Antialiasing
                            | QPainter::HighQualityAntialiasing);

    QBrush b;
    b.setStyle(Qt::SolidPattern);
    b.setColor(QColor(0, 0, 0, 180));

    painter->setPen(Qt::NoPen);
    painter->setBrush(b);

    painter->drawRoundedRect(infoRect, 5, 5);
    if (!pixmap.isNull()) {
        painter->drawPixmap(point + offset - QPoint(dx, dy), pixmap);
    }

    QPen p;
    p.setColor(QColor(218, 218, 218));
    p.setStyle(Qt::SolidLine);
    p.setCapStyle(Qt::RoundCap);
    p.setWidth(1);

    painter->setPen(p);
    painter->setFont(font);

    painter->drawText(br, Qt::AlignLeft | Qt::AlignVCenter, text);

    painter->restore();
}

void Grabber::drawInstructionInfoRect(QPainter *painter)
{
    QString text = i18n("Click on a window to select it or click and drag to grab a screen selection\n"
                        "Press Enter key when done\n"
                        "Press Esc key to exit");
    drawTextIntoRect(painter, QPixmap(), text, QPoint(0, 0), QPoint(20, 20));
}

void Grabber::drawGrabInfoRect(QPainter *painter, const QRect &rect)
{
    if (rect.isNull() || !rect.isValid()) {
        return;
    }

    QString text = QString("%1 x %2").arg(QString::number(rect.width()))
                                     .arg(QString::number(rect.height()));
    QPixmap icon;
    if (m_pointedWindow != 0) {
        icon = KWindowSystem::icon(m_pointedWindow->win(), 32, 32, true);
        text += "\n" + m_pointedWindow->iconName();
    }

    drawTextIntoRect(painter,
                     icon,
                     text,
                     m_crossCenter,
                     QPoint(15, 15),
                     this->rect().adjusted(5, 5, -5, -5));
}

void Grabber::translateSelection(int dx, int dy)
{
    if (m_selection.isNull() || !m_selection.isValid()) {
        return;
    }

    QRect r = m_selection.translated(dx, dy);
    if (rect().normalized().contains(r)) {
        m_selection = r;
        updateCrossCenterPosition();
        updateHandles();
        update();
    }
}

void Grabber::updateCrossCenterPosition()
{
    switch(m_crossCenterPos) {
    case TopLeft:
        m_crossCenter = m_selection.topLeft();
        break;
    case TopRight:
        m_crossCenter = m_selection.topRight();
        break;
    case BottomLeft:
        m_crossCenter = m_selection.bottomLeft();
        break;
    case BottomRight:
        m_crossCenter = m_selection.bottomRight();
        break;
    case Unknown:
        break;
    }
}

void Grabber::updateHandles()
{
    int size = 12;
    QSize handleSize(size, size);
    m_hTL = QRect(m_selection.topLeft(), handleSize);
    m_hTR = QRect(m_selection.topRight() + QPoint(-size, 0), handleSize);
    m_hBL = QRect(m_selection.bottomLeft() + QPoint(0, -size), handleSize);
    m_hBR = QRect(m_selection.bottomRight() + QPoint(-size,-size), handleSize);
}

void Grabber::updatePointedWindow(const QPoint &pos)
{
    QLinkedListIterator<KWindowInfo> iterator(m_tlwInfo);
    iterator.toBack();
    while(iterator.hasPrevious()) {
        const KWindowInfo *wi = &iterator.previous();
        if (!wi->frameGeometry().contains(pos)) continue;
        if (wi->geometry().contains(pos)) {
            m_windowSelection = wi->geometry().intersected(rect());
        } else {
            m_windowSelection = wi->frameGeometry().intersected(rect());
        }
        m_pointedWindow = wi;
        break;
    }
}

QPoint Grabber::limitPointToRect(const QPoint &p, const QRect &r) const
{
    QPoint q;
    q.setX(p.x() < r.x() ? r.x() : p.x() < r.right() ? p.x() : r.right());
    q.setY(p.y() < r.y() ? r.y() : p.y() < r.bottom() ? p.y() : r.bottom());
    return q;
}

QRect Grabber::normalizeSelection(const QRect &s) const
{
    QRect r = s;
    if (r.width() <= 0) {
        int l = r.left();
        int w = r.width();
        r.setLeft(l + w - 1);
        r.setRight(l);
    }
    if (r.height() <= 0) {
        int t = r.top();
        int h = r.height();
        r.setTop(t + h - 1);
        r.setBottom(t);
    }
    return r;
}
