/*
 * (c) Digital Geyser.
 *
 * General purpose utilities.
 */
#include "DGUtil.h"

static int DGUtil::temperatureFtoC(int f) {
  return ((f-32)*5)/9;
}

static int DGUtil::temperatureCtoF(int c) {
  return (c*9)/5 + 32;
}
