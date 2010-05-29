// Egg built-ins
void log(char *msg);
void logNumber(double num);
void testCall();
double addOne(double num);

double getResolutionX();
double getResolutionY();
// End Egg built-ins

int main() {
    logNumber(getResolutionX());
}