/******************************************************************************
  Bradley Bauer
  Project #3
******************************************************************************/

#include <Sim.h>

void circuits( SD, Signal, Signal, Signal, Signal,
  Signal, Signal, Signal, Signal, Signal, Signal, Signal, Signal );

/******************************************************************************
  Function "simnet"   -- test bed for student circuits
******************************************************************************/
void simnet()
{
  Signal strobe; // counter strobe input
  Signal reset; // counter reset signal (before complemented)
  Signal reset_compl;
  Signal output (4); // 4bit counter output

  Pulser ((SD("2a"), "r - counter reset"),  reset, 'r', 10000);
  Pulser ((SD("4a"), "s - counter strobe"), strobe, 's', 10000);

  // Complement the reset signal (counter reset is active low)
  Not(SD("3b"), reset, reset_compl);
  Counter((SD("3c-4c"), "4-bit counter"), (reset_compl, strobe), output);

  Signal Present, a, b, c, d, e, f, g;

  circuits(SD("2d-5d"), output[3], output[2], output[1], output[0], Present,
          a, b, c, d, e, f, g);

  Probe((SD("1g"), "Present"), Present);
  Probe((SD("1f-5f"), "f"), f);
  Probe((SD("3f-7f"), "e"), e);
  Probe((SD("2e-2i"), "a"), a);
  Probe((SD("4e-4i"), "g"), g);
  Probe((SD("6e-6i"), "d"), d);
  Probe((SD("1h-5h"), "b"), b);
  Probe((SD("3h-7h"), "c"), c);
}


/******************************************************************************
  Function "circuits" -- implementation of student circuits

  Do not alter the interface -- function "circuits" must receive four
  signals as inputs and return eight signals as outputs.
******************************************************************************/
void circuits( SD sd, Signal w, Signal x, Signal y, Signal z,
  Signal Present, Signal a, Signal b, Signal c, Signal d, Signal e,
  Signal f, Signal g )
{
  Module( (sd, "circuits"), (w, x, y, z), (a, Present, f, b, g, c, e, d) );

  // Replace with declarations for any auxiliary Signals
  Signal wp, xp, yp, zp; // complements
  Signal pand1, pand2, pand3, pand4; // and gates in present signal
  Signal band, dand, gand; // and gates for b(), d(), and g()

  // Compute complements
  Not(SD(sd, "1a"), w, wp);
  Not(SD(sd, "2a"), x, xp);
  Not(SD(sd, "3a"), y, yp);
  Not(SD(sd, "4a"), z, zp);

  // P(), present signal
  And(SD(sd, "1b"), (w,xp,zp), pand1);
  And(SD(sd, "2b"), (w,xp,yp), pand2);
  And(SD(sd, "3b"), (wp,x,y), pand3);
  And(SD(sd, "4b"), (wp,x,z), pand4);
  Or(SD(sd, "1g"), (pand1, pand2, pand3, pand4), Present);

  // a(), c() are constants
  Or(SD(sd, "5b"), (x,xp), a);
  Or(SD(sd, "6b"), (x,xp), c);

  // b()
  And(SD(sd, "7b"), (y,z), band);
  Or(SD(sd, "7c"), (band, w), b);

  // d()
  And(SD(sd, "8b"), (wp,zp), dand);
  Or(SD(sd, "8c"), (yp, dand), d);

  // e()
  Not(SD(sd, "9c"), z, e);

  // f()
  Or(SD(sd, "10c"), (zp, yp), f);

  // g()
  Or(SD(sd, "11c"), (yp, zp), g);
}

