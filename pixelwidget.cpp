#include <QtGui>
#include <QApplication>
#include <QMainWindow>
#include <QMessageBox>
#include <QPainter>
#include <QLabel>
#include <QDesktopWidget>
#include <iostream>
#include <fstream>
#include "pixelwidget.hpp"

#include <math.h>       /* pow sqrt round */
using namespace std;    /* cout */

void PixelWidget::DefinePixelValues(){ //add pixels here; write methods like this for the assignments
    SetPixel(10,30,RGBVal(255,0,0));
}



void PixelWidget::DrawLine( float x1,float y1,
                            float x2,float y2,
                            int r1, int g1, int b1,
                            int r2, int g2, int b2){

    float x = 0, y = 0;
    float xDif=x1-x2,yDif=y1-y2;
    float steps = sqrt(pow(x2-x1,2)+pow(y2-y1,2));   // Distance between points
    float m = (y2 - y1) / (x2 - x1);                 // Gradiant

    for (float i = 0; i < steps; i++) {
        if (m){ // Line is Diagonal or Vertical
            y = (yDif*(i/steps)) + y2;
            x = ((yDif*(i/steps)) / m) + x2;

        } else { // Line is Horizontal
            y = y1;
            x = xDif*(i/steps) + x2;
        }

        // Assignment 2
        // Color interpolation (ternary operation)
        double stepRatio2 = ((steps-i)/steps);
        double stepRatio = (i/steps);

        int r = (r1>r2) ? ((r1-r2)*stepRatio)+r2 : ((r2-r1)*stepRatio2)+r1;
        int g = (g1>g2) ? ((g1-g2)*stepRatio)+g2 : ((g2-g1)*stepRatio2)+g1;
        int b = (b1>b2) ? ((b1-b2)*stepRatio)+b2 : ((b2-b1)*stepRatio2)+b1;

        SetPixel(x,y,RGBVal(r,g,b));
    }

    // Set start/end points of line
    SetPixel(x1,y1,RGBVal(r1,g1,b1));
    SetPixel(x2,y2,RGBVal(r2,g2,b2));
}

bool PixelWidget::IsInside( float x1,float y1,
                            float x2,float y2,
                            float x3,float y3,
                            float px,float py){

    /* Half Plane Test */
    // Using equation: d=(x-x1)(y2-y1)-(y-y1)(x2-x1)
    // A = (x1,y1)
    // B = (x2,y2)
    // C = (x3,y3)
    // P = (px,py)

    // Solve for every pair of vertices
    float d1, d2, d3;
    d1 = (px - x2) * (y1 - y2) - (x1 - x2) * (py - y2);  // For A & B
    d2 = (px - x3) * (y2 - y3) - (x2 - x3) * (py - y3);  // For B & C
    d3 = (px - x1) * (y3 - y1) - (x3 - x1) * (py - y1);  // For C & A

    // Point is within triangle if it lies on the same side for all planes, or on a plane
    // d > 0    - Point is above line
    // d < 0    - Point is below line
    // d = 0    - Point is on line
    if (((d1<=0)&&(d2<=0)&&(d3<=0)) || ((d1>=0)&&(d2>=0)&&(d3>=0))) return 1;
    return 0;
}


void PixelWidget::DrawHalfPlane(float x1,float y1,
                    float x2,float y2,
                    float x3,float y3){

    // Calculate Bounding box
    float maxX = max(x1, max(x2, x3));
    float minX = min(x1, min(x2, x3));
    float maxY = max(y1, max(y2, y3));
    float minY = min(y1, min(y2, y3));

    /* Create file to store Half-Plane co-ordinates */
    ofstream f;
    f.open ("DrawHalfPlane.txt");
    f << "Vertex 1: (" << x1 << "," << y1 << ")\n";
    f << "Vertex 2: (" << x2 << "," << y2 << ")\n";
    f << "Vertex 3: (" << x3 << "," << y3 << ")\n";
    f << "\nCo-ordinates:\n";

    /* Rasterization */
    // Iterate over co-ordinates in bounding box
    for (int py = minY; py <= maxY; py++){
        for (int px = minX; px <= maxX; px++){
            if (IsInside( x1,y1, x2, y2, x3, y3, px, py)){
                f << "(" << px << "," << py << ")\n";   // Write co-ordinate to file
                SetPixel(px,py,RGBVal(255,255,255));
            }
        }
    }
    f.close();  // Close file after writing
}



void PixelWidget::DrawPPM( float x1,float y1,
                            float x2,float y2,
                            float x3,float y3,
                            int r1, int g1, int b1,
                            int r2, int g2, int b2,
                            int r3, int g3, int b3){

    int pr,pg,pb;       // RGB values for each point
    float w1,w2,w3;     // Relative weighting of each vertex

    // Calculate Bounding box
    float maxX = max(x1, max(x2, x3));
    float maxY = max(y1, max(y2, y3));
    float minX = min(x1, min(x2, x3));
    float minY = min(y1, min(y2, y3));

    /* Create PPM file */
    int w = maxX-minX+1, h = maxY-minY+1;
    ofstream f;
    f.open ("DrawPPM.ppm");         // Filename
    f << "P3\n";
    f << h << " " << w << "\n";     // Height & Width
    f << "# Vertex 1: (" << x1 << "," << y1 << ")\n";  // Vertex Comments
    f << "# Vertex 2: (" << x2 << "," << y2 << ")\n";  // Vertex Comments
    f << "# Vertex 3: (" << x3 << "," << y3 << ")\n";  // Vertex Comments
    f << 255 << "\n";               // Maximum Value

    /* Rasterization */
    // Iterate over co-ordinates in bounding box
    for (int py = minY; py <= maxY; py++){
        for (int px = minX; px <= maxX; px++){

            /* Barycentric Interpolation */
            // Calculate the weighting for each vertex
            w1 = (((y2-y3)*(px-x3))+((x3-x2)*(py-y3))) / (((y2-y3)*(x1-x3))+((x3-x2)*(y1-y3)));
            w2 = (((y3-y1)*(px-x3))+((x1-x3)*(py-y3))) / (((y2-y3)*(x1-x3))+((x3-x2)*(y1-y3)));
            w3 = 1 - w1 - w2;

            // Point is within triangle if all weights are between 0 & 1
            if (w1>=0 && w2>=0 && w3>=0 && 1>=w1 && 1>=w2 && 1>=w3 ){
                // Calculate RGB values of point using weighting
                pr = (int)((r1*w1) + (r2*w2) + (r3*w3));
                pg = (int)((g1*w1) + (g2*w2) + (g3*w3));
                pb = (int)((b1*w1) + (b2*w2) + (b3*w3));
                f << pr << " " << pg << " " << pb << " ";
                SetPixel(px,py,RGBVal(pr,pg,pb));

            } else {
                f << "0 0 0 ";      // Set "background" pixels
            }
        }
        f << "\n";                  // New line of pixels
    }
    f.close();                      // Close file after writing
}



void PixelWidget::DrawBarycentric( float x1,float y1,
                            float x2,float y2,
                            float x3,float y3,
                            int r1, int g1, int b1,
                            int r2, int g2, int b2,
                            int r3, int g3, int b3){

    int   pr,pg,pb;     // RGB values for each point
    float w1,w2,w3;     // Relative weighting of each vertex

    /* Create file to store Barycentric co-ordinates */
    ofstream f;
    f.open ("DrawBarycentric.txt");
    f << "Vertex 1: (" << x1 << "," << y1 << ")\n";
    f << "Vertex 2: (" << x2 << "," << y2 << ")\n";
    f << "Vertex 3: (" << x3 << "," << y3 << ")\n";
    f << "\nCo-ordinates:\n";

    // Calculate Bounding box
    float maxX = max(x1, max(x2, x3));
    float minX = min(x1, min(x2, x3));
    float maxY = max(y1, max(y2, y3));
    float minY = min(y1, min(y2, y3));

    /* Rasterization */
    // Iterate over co-ordinates in bounding box
    for (int py = minY; py <= maxY; py++){
        for (int px = minX; px <= maxX; px++){
            /* Barycentric Interpolation */
            // Calculate the weighting for each vertex
            w1 = (((y2-y3)*(px-x3))+((x3-x2)*(py-y3))) / (((y2-y3)*(x1-x3))+((x3-x2)*(y1-y3)));
            w2 = (((y3-y1)*(px-x3))+((x1-x3)*(py-y3))) / (((y2-y3)*(x1-x3))+((x3-x2)*(y1-y3)));
            w3 = 1 - w1 - w2;

            // Point is within triangle if all weights are between 0 & 1
            if (w1>=0 && w2>=0 && w3>=0 && 1>=w1 && 1>=w2 && 1>=w3 ){
                f << "(" << px << "," << py << ")\n";   // Write co-ordinate to file

                // Calculate RGB values of point using weighting
                pr = (int)((r1*w1) + (r2*w2) + (r3*w3));
                pg = (int)((g1*w1) + (g2*w2) + (g3*w3));
                pb = (int)((b1*w1) + (b2*w2) + (b3*w3));
                SetPixel(px,py,RGBVal(pr,pg,pb));
            }
        }
    }
    f.close();  // Close file after writing
}





// -----------------Most code below can remain untouched -------------------------
// ------but look at where DefinePixelValues is called in paintEvent--------------

PixelWidget::PixelWidget(unsigned int n_vertical, unsigned int n_horizontal):
    _n_vertical  (n_vertical),
    _n_horizontal (n_horizontal),
    _vec_rects(0)
{
    // all pixels are initialized to black
         for (unsigned int i_col = 0; i_col < n_vertical; i_col++)
             _vec_rects.push_back(std::vector<RGBVal>(n_horizontal));
}



void PixelWidget::SetPixel(unsigned int i_column, unsigned int i_row, const RGBVal& rgb)
{

    // if the pixel exists, set it
    if (i_column < _n_vertical && i_row < _n_horizontal)
        _vec_rects[i_column][i_row] = rgb;
}


void PixelWidget::paintEvent( QPaintEvent * )
{

    QPainter p( this );
    // no antialiasing, want thin lines between the cell
    p.setRenderHint( QPainter::Antialiasing, false );

    // set window/viewport so that the size fits the screen, within reason
    p.setWindow(QRect(-1.,-1.,_n_vertical+2,_n_horizontal+2));
    int side = qMin(width(), height());
    p.setViewport(0, 0, side, side);

    // black thin boundary around the cells
    QPen pen( Qt::black );
    pen.setWidth(0.);

    // // here the pixel values defined by the user are set in the pixel array
    // DefinePixelValues();


    // // Assignment 1 Test
    // DrawLine( 5, 5, 30,55);
    // DrawLine(30,55, 55,55);
    // DrawLine(55,55,  5, 5);

    // Assignment 2 Test
    DrawLine( 5, 5, 30,55,   255,0,0, 0,255,0);   // Triangle
    DrawLine(30,55, 55,55,   0,255,0, 0,0,255);
    DrawLine(55,55,   5,5,   0,0,255, 255,0,0);

    DrawLine(30,30,30, 0,  255,0,0,  0,255,0); // Vertical - North

    DrawLine(30,30,40, 5,  255,0,0,  0,255,0); // Diagonal - North East
    DrawLine(30,30,45,10,  255,0,0,  0,255,0); // Diagonal - North East
    DrawLine(30,30,50,15,  255,0,0,  0,255,0); // Diagonal - North East
    DrawLine(30,30,55,20,  255,0,0,  0,255,0); // Diagonal - North East

    DrawLine(30,30,60,30,  255,0,0,  0,255,0); // Horizontal - East

    DrawLine(30,30,55,40,  255,0,0,  0,255,0); // Diagonal - South East
    DrawLine(30,30,50,45,  255,0,0,  0,255,0); // Diagonal - South East
    DrawLine(30,30,45,50,  255,0,0,  0,255,0); // Diagonal - South East
    DrawLine(30,30,40,55,  255,0,0,  0,255,0); // Diagonal - South East

    DrawLine(30,30,30,60,  255,0,0,  0,255,0); // Vertical - South

    DrawLine(30,30,20,55,  255,0,0,  0,255,0); // Diagonal - South West
    DrawLine(30,30,15,50,  255,0,0,  0,255,0); // Diagonal - South West
    DrawLine(30,30,10,45,  255,0,0,  0,255,0); // Diagonal - South West
    DrawLine(30,30, 5,40,  255,0,0,  0,255,0); // Diagonal - South West

    DrawLine(30,30, 0,30,  255,0,0,  0,255,0); // Horizontal - West

    DrawLine(30,30,20, 5,  255,0,0,  0,255,0); // Diagonal - North West
    DrawLine(30,30,15,10,  255,0,0,  0,255,0); // Diagonal - North West
    DrawLine(30,30,10,15,  255,0,0,  0,255,0); // Diagonal - North West
    DrawLine(30,30, 5,20,  255,0,0,  0,255,0); // Diagonal - North West

    // Assignment 3 Test
    DrawBarycentric(30,5,       // Large Triangle
        5,55,
        55,55,
        255,0,0,
        0,255,0,
        0,0,255);

    DrawBarycentric(5,5,       // Obtuse Triangle
        30,55,
        55,55,
        255,0,0,
        0,255,0,
        0,0,255);

    // Assignment 4
    DrawHalfPlane(  30,5,
                    5,55,
                    55,55);
    DrawBarycentric(  30,5,
                    5,55,
                    55,55,
                    255,0,0,
                    0,255,0,
                    0,0,255);

    // Assignment 5
    DrawPPM(250,0,       // Very Large Triangle
        0,500,
        500,500,
        255,0,0,
        0,255,0,
        0,0,255);


    for (unsigned int i_column = 0 ; i_column < _n_vertical; i_column++)
        for(unsigned int i_row = 0; i_row < _n_horizontal; i_row++){
            QRect rect(i_column,i_row,1,1);
            QColor c = QColor(_vec_rects[i_column][i_row]._red, _vec_rects[i_column][i_row]._green, _vec_rects[i_column][i_row]._blue);

            // fill with color for the pixel
            p.fillRect(rect, QBrush(c));
            p.setPen(pen);
            p.drawRect(rect);
        }
}
