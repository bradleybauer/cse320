
/******************************************************************************
  Project #4 skeleton
******************************************************************************/

#include <Sim.h>

void circuits( SD, Signal, Signal, Signal, Signal, Signal, Signal );


/******************************************************************************
  Function "simnet"   -- test bed for student circuits
******************************************************************************/

void simnet()
{
  Signal Init, Clock, w, x, y, z;

  Pulser ((SD("1a"), "i - Init"),  Init, 'i', 1000);
  Pulser ((SD("2a"), "c - Clock"), Clock, 'c', 1000);

  circuits( SD("1b-4b"), Init, Clock, w, x, y, z );

  Probe((SD("1c"), "W"), w);
  Probe((SD("2c"), "X"), x);
  Probe((SD("3c"), "Y"), y);
  Probe((SD("4c"), "Z"), z);
}


/******************************************************************************
  Function "circuits" -- implementation of student circuits
******************************************************************************/

void circuits( SD sd, Signal Init, Signal Clock,
  Signal w, Signal x, Signal y, Signal z )
{
  Module((sd, "circuits"), (Init, Clock), (w, x, y, z));

  // d flip flop inputs
  Signal W, X, Y, Z;

  // logic working vars
  // negations
  Signal wp, yp, zp;
  // conjunctions
  Signal wand1, wand2, yand1, yand2, yand3;

  Dff((SD(sd, "1a"), "Dff - w"), (Init, W, Clock, Zero), w);
  Dff((SD(sd, "2a"), "Dff - x"), (Zero, X, Clock, Init), x);
  Dff((SD(sd, "3a"), "Dff - y"), (Init, Y, Clock, Zero), y);
  Dff((SD(sd, "4a"), "Dff - z"), (Zero, Z, Clock, Init), z);

  // Take current state, (w,x,y,z), and compute next state (W,X,Y,Z)

  Not(SD(sd, "1b"), w, wp);
  Not(SD(sd, "3b"), y, yp);
  Not(SD(sd, "4b"), z, zp);

  // W()
  And(SD(sd, "1d"), (w, yp), wand1);
  And(SD(sd, "2d"), (y, z), wand2);
  Or(SD(sd, "1e"), (wand1, wand2), W);

  // X()
  //And(SD(sd, "3c"), (y, zp), xand1);
  //And(SD(sd, "4c"), (wp, yp), xand2);
  //Or(SD(sd, "3d"), (xand1, xand2), X);
  Not(SD(sd, "3e"), W, X);

  // Y()
  And(SD(sd, "5d"), (yp, zp), yand1);
  And(SD(sd, "4d"), (wp, yp), yand2); // same as xand2
  And(SD(sd, "6d"), (wp, zp), yand3);
  Or(SD(sd, "5e"), (yand1, yand2, yand3), Y);

  // Z()
  Or(SD(sd, "6a"), (y, Zero), Z);
}

