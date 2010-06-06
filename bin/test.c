// Egg typedefs
typedef void* Image;
typedef void* RenderStack;
typedef void* Sprite;
typedef void* Layer;
typedef unsigned char boolean;

#define KEY_UP 38
#define KEY_LEFT 37
#define KEY_RIGHT 39
#define KEY_DOWN 40

// Egg built-ins
void log(char *msg);
void logNumber(double num);
void testCall();
double addOne(double num);

double getResolutionX();
double getResolutionY();

void setRenderStack(RenderStack stack);
RenderStack getRenderStack();
RenderStack newRenderStack();
void RenderStack_addLayer(RenderStack stack, Layer layer);

Layer newLayer(double top, double bottom, double left, double right);
void Layer_add(Layer lyr, void *object);

Sprite newSprite(Image img, double x, double y, double w, double h);
double Sprite_getX(Sprite s);
double Sprite_getY(Sprite s);
void Sprite_setXY(Sprite s, double x, double y);

Image newImage(char *filename);
double Image_height(Image img);
double Image_width(Image img);
// End Egg built-ins

Sprite spr;
boolean btnUP = 0, btnLEFT = 0, btnRIGHT = 0, btnDOWN = 0;

int main() {
    RenderStack stack = getRenderStack();
    Image img = newImage("test.png");
    Layer lyr = newLayer(0, 240, 0, 320);
    
    logNumber(Image_width(img));
    
    spr = newSprite(img, 10, 10, 20, 20);
    
    Layer_add(lyr, spr);
    RenderStack_addLayer(stack, lyr);
}

void update(double dt) {
    double v = 100;
    double vx = 0, vy = 0;
    
    if ( btnUP && !btnDOWN ) vy -= v;
    if ( btnDOWN && !btnUP ) vy += v;
    
    if ( btnLEFT && !btnRIGHT ) vx -= v;
    if ( btnRIGHT && !btnLEFT ) vx += v;
    
    if ( vx != 0 || vy != 0 ) {
        double x, y;
        x = (Sprite_getX(spr) + vx*dt);
        y = (Sprite_getY(spr) + vy*dt);
        
        while ( x > 320 ) x -= 320; while ( x < 0 ) x += 320;
        while ( y > 240 ) y -= 240; while ( y < 0 ) y += 240;
        
        Sprite_setXY(spr, x, y);
    }
}

void keydown(double k) {
    // what keys do we care about?
    switch ( (int)k ) {
        case KEY_UP: btnUP = 1; break;
        case KEY_LEFT: btnLEFT = 1; break;
        case KEY_RIGHT: btnRIGHT = 1; break;
        case KEY_DOWN: btnDOWN = 1; break;
        default: break; // do nothing
    }
}

void keyup(double k) {
    // what keys do we care about?
    switch ( (int)k ) {
        case KEY_UP: btnUP = 0; break;
        case KEY_LEFT: btnLEFT = 0; break;
        case KEY_RIGHT: btnRIGHT = 0; break;
        case KEY_DOWN: btnDOWN = 0; break;
        default: break; // do nothing
    }
}