#ifndef GRABBER_H
#define GRABBER_H

#include <QWidget>
#include <QLinkedList>
#include <KWindowInfo>

// TODO: this class should be rewritten and splitted
// TODO: Window grab logic should use composition when possible
class Grabber : public QWidget
{
    Q_OBJECT

public:
    explicit Grabber(QWidget *parent = 0);

public Q_SLOTS:
    void init();

Q_SIGNALS:
    void regionGrabbed(const QPixmap &pixmap, const QPoint &topLeft);
    void quitRequested();

private Q_SLOTS:
    void grabWindow();

private:
    enum CrossCenterPosition {
        Unknown,
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight
    };

    virtual void showEvent(QShowEvent *e);
    virtual void hideEvent(QHideEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *e);
    void drawBoundingSelection(QPainter *painter,
                               const QRect &rect,
                               const QColor &color);
    void drawSelectionRect(QPainter *painter, const QRect &r);
    void drawCrosslines(QPainter *painter,
                        const QRect &rect,
                        const QPoint &point,
                        const QColor &color);
    void drawHandles(QPainter *painter, const QColor &color);
    void drawTextIntoRect(QPainter *painter,
                          const QPixmap &pixmap,
                          const QString &text,
                          const QPoint &point,
                          const QPoint &offset,
                          const QRect &containerRect = QRect());
    void drawInstructionInfoRect(QPainter *painter);
    void drawGrabInfoRect(QPainter *painter, const QRect &r);
    void resetGrabberState();
    void grabRect();
    void updateCrossCenterPosition();
    void updateHandles();
    void updatePointedWindow(const QPoint &pos);
    void translateSelection(int dx, int dy);
    QPoint limitPointToRect(const QPoint &p, const QRect &r) const;
    QRect normalizeSelection(const QRect &s) const;

    QRect m_selection;
    QPixmap m_pixmap;
    QPixmap m_bwPixmap;
    bool m_mouseDown;
    bool m_newSelection;
    QPoint m_dragStartPoint;
    QRect m_selectionBeforeDrag;
    QRect *m_mouseOverHandle;
    QPoint m_crossCenter;
    bool m_shooting;
    CrossCenterPosition m_crossCenterPos;
    QLinkedList<KWindowInfo> m_tlwInfo;
    const KWindowInfo *m_selectedWindow;
    const KWindowInfo *m_pointedWindow;
    QRect m_windowSelection;
    QRect m_hTL;
    QRect m_hTR;
    QRect m_hBL;
    QRect m_hBR;

};

#endif // GRABBER_H
