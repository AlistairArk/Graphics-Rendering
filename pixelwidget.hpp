#ifndef _PIXEL_WIDGET_
#define _PIXEL_WIDGET_

#include <QtGui>
#include <QApplication>
#include <QMainWindow>
#include <QMessageBox>
#include <QPainter>
#include <QLabel>
#include <QDesktopWidget>
#include <iostream>
#include <vector>
#include "RGBVal.hpp"


class PixelWidget : public QWidget {
public:


    // set the number of pixels that the widget is meant to display
    PixelWidget
    (
     unsigned int n_horizontal, // the first integer determines the number of horizontal pixels
     unsigned int n_vertical        // the second integer determines the number of vertical pixels
     );

    // sets a pixel at the specified RGBVal value; ignores non-existing pixels without warning
    void SetPixel
    (
     unsigned int    i_x, // horizontal pixel coordinate
     unsigned int    i_y, // vertical pixel coordinate
     const RGBVal& c        // RBGVal object for RGB values
        );

    // Use the body of this function to experiment with rendering algorithms
    void DefinePixelValues();

    void DrawLine(float x1,float y1,float x2,float y2,int r1, int g1, int b1, int r2, int g2, int b2);

    bool IsInside(float x1,float y1,
                float x2,float y2,
                float x3,float y3,
                float px,float py);

    void DrawBarycentric(float x1,float y1,
                        float x2,float y2,
                        float x3,float y3,
                        int r1, int g1, int b1,
                        int r2, int g2, int b2,
                        int r3, int g3, int b3);

    void DrawPPM(float x1,float y1,
                float x2,float y2,
                float x3,float y3,
                int r1, int g1, int b1,
                int r2, int g2, int b2,
                int r3, int g3, int b3);

    void DrawHalfPlane(float x1,float y1,
                        float x2,float y2,
                        float x3,float y3);
protected:

    virtual void paintEvent(QPaintEvent*);


private:

    unsigned int _n_vertical;
    unsigned int _n_horizontal;

    std::vector<std::vector<RGBVal> > _vec_rects;
};

#endif
