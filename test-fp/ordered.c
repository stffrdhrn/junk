#include <math.h>

int sfun(float a, float b) {
  return isunordered(a, b);
}
int sfult(float a, float b) {
  return !isgreaterequal(a, b);
}
int sfule(float a, float b) {
  return !isgreater(a, b);
}
int sfueq(float a, float b) {
  return !islessgreater(a, b);
}

/* the above outputs:
 *
000022c8 <sfun>:                                                                
    22c8:       c8 03 20 2e     lf.sfun.s r3,r4                                 
    22cc:       a9 60 00 01     l.ori r11,r0,0x1                                
    22d0:       44 00 48 00     l.jr r9                                         
    22d4:       e1 6b 00 0e     l.cmov r11,r11,r0                               
                                                                                
000022d8 <sfult>:                                                               
    22d8:       c8 03 20 2c     lf.sfult.s r3,r4                                
    22dc:       a9 60 00 01     l.ori r11,r0,0x1                                
    22e0:       44 00 48 00     l.jr r9                                         
    22e4:       e1 6b 00 0e     l.cmov r11,r11,r0                               
                                                                                
000022e8 <sfule>:                                                               
    22e8:       c8 03 20 2d     lf.sfule.s r3,r4                                
    22ec:       a9 60 00 01     l.ori r11,r0,0x1                                
    22f0:       44 00 48 00     l.jr r9                                         
    22f4:       e1 6b 00 0e     l.cmov r11,r11,r0                               
                                                                                
000022f8 <sfueq>:                                                               
    22f8:       c8 03 20 28     lf.sfueq.s r3,r4                                
    22fc:       a9 60 00 01     l.ori r11,r0,0x1                                
    2300:       44 00 48 00     l.jr r9                                         
    2304:       e1 6b 00 0e     l.cmov r11,r11,r0   
*/

int main() {
  return sfun (4.0, 4.2) ||
         sfult(4.2, 3.4) ||
         sfule(4.2, 3.4) ||
         sfueq(3.2, 3.2);
}
