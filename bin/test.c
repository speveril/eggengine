// Egg typedefs
typedef void* Image;
typedef void* RenderStack;
typedef void* Sprite;
typedef void* Layer;

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

Image newImage(char *filename);
double Image_height(Image img);
double Image_width(Image img);
// End Egg built-ins

int main() {
    RenderStack stack = getRenderStack();
    Image img = newImage("test.png");
    Layer lyr = newLayer(0, 240, 0, 320);
    
    Sprite spr = newSprite(img, 60, 182, 64, 64);
    
    Layer_add(lyr, spr);
    RenderStack_addLayer(stack, lyr);
}