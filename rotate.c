unsigned n = 32;

static unsigned rotr(unsigned x, unsigned n) {
    return (x >> n % 32) | (x << (32-n) % 32);
}

int main() {
   return rotr(n, 2);
}
