// Egg built-ins
void log(char *msg);
void logNumber(double num);
void testCall();
double addOne(double num);
// End Egg built-ins

int main() {
    logNumber(addOne(5));
}