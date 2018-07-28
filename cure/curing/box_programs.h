/**
 * default built-in programs for the curing app.
 */

typedef struct {
  uint8_t startTemperature;
  uint8_t endTemperature;
  uint8_t startHumidity;
  uint8_t endHumidity;
  uint16_t durationInHours;
  uint8_t percentVentilation;
} BoxProgramPhase;

typedef struct {
  const char *programName;
  const char *programDescription;
  int numberOfPhases;
  const BoxProgramPhase *phases;
} BoxProgram;

#define INFINITY_HOURS 0

// For each program, define an array of phases.
// Each phase has 6 numbers:
//    start T in degreesCelsius
//    end T in degreesCelsius
//    start humidity in percentages
//    end humidity in percentages
//    duration of the phase in hours (if infinity, then use INFINITY_HOURS)
//    percentage of ventilation used in percentages
const BoxProgramPhase pancettaPhases[] = {
  { 55, 55, 60, 60, 98 , 50},
};

const BoxProgramPhase coldSmokedSausageCuringPhases[] = {
  { 20, 40, 40, 60, 84, 50 },
};

const BoxProgramPhase PepperoneSheepCasingsCuringPhases[] = {
  { 85, 85, 0xff, 0xff, 12 ,10},
  { 60, 60, 70, 60, 168 , 50},
};

const BoxProgramPhase PepperoneHogCasingsCuringPhases[] = {
  { 85, 85, 0xff, 0xff, 12, 10},
  { 60, 60, 70, 60, 360, 50 },
};

const BoxProgramPhase PepperoneHogMiddlesCuringPhases[] = {
  { 85, 85, 0xff, 0xff, 12, 0 },
  { 60, 60, 70, 60, 672, 50 },
};

const BoxProgramPhase CoppaCuringPhases[] = {
  { 75, 75, 0xff, 0xff, 12, 0 },
  { 60, 60, 70, 60, 504, 50 },
};

const BoxProgramPhase ChorizoCuringPhases[] = {
  { 60, 60, 65, 65, 456, 50 },
};

const BoxProgramPhase LandjagerCuringPhases[] = {
  { 60, 60, 65, 65, 312, 50 },
};

const BoxProgramPhase ProsciuttoCuringPhases[] = {
  { 60, 60, 65, 65, 3600, 100 },
};

const BoxProgramPhase CountryHamCuringPhases[] = {
  { 60, 60, 67, 67, 1176, 50 },
};
const BoxProgramPhase BresaolaCuringPhases[] = {
  { 60, 60, 65, 65, 504, 50 },
};
const BoxProgramPhase DuckProsciuttoCuringPhases[] = {
  { 55, 55, 85, 85, 168, 50 },
};
const BoxProgramPhase BloomyRindCheeseAgingPhases[] = {
  { 48, 48, 96, 96, 0xff, 50 },
};
const BoxProgramPhase BlueCheeseAgingPhases[] = {
  { 52, 52, 90, 90, 1440, 50 },
  { 43, 43, 90, 90, 0xff, 50 },
};
const BoxProgramPhase FetaCheeseAgingPhases[] = {
  { 57, 57,0xff , 0xff, 0xff, 50 },
};
const BoxProgramPhase CaerphillyCheeseAgingPhases[] = {
  { 52, 52,92 , 92, 0xff, 50 },
};
const BoxProgramPhase CheddarCheeseAgingPhases[] = {
  { 52, 52, 0xff, 0xff, 0xff, 50 },
};

// Once you have all the program phases above,
// create the "BoxProgram" block that contains
// name, the number of phases (use sizeof()) and the actual phases.
const BoxProgram defaultPrograms[] = {
  {
    "Pancetta",
    "This program is used to cure pancetta.",
    sizeof(pancettaPhases)/sizeof(BoxProgramPhase),
    pancettaPhases
  },
  {
    "Cold Smoked Sausage Curing",
    "This program is used to cure cold smoked sausages like Hungarian paprika sausage, Andouille and Chorizo sausage.",
    sizeof(coldSmokedSausageCuringPhases)/sizeof(BoxProgramPhase),
    coldSmokedSausageCuringPhases
  },
  {
    "Pepperone and Tuscan Salami with Sheep Casings Curing",
    "This program is used to cure Pepperone and Tuscan salami made with sheep casings.",
    sizeof(PepperoneSheepCasingsCuringPhases)/sizeof(BoxProgramPhase),
    PepperoneSheepCasingsCuringPhases
  },
  {
    "Pepperone and Tuscan Salami with Hog Casings Curing",
    "This program is used to cure Pepperone and Tuscan salami made with hog casings and Soppressata.",
    sizeof(PepperoneHogCasingsCuringPhases)/sizeof(BoxProgramPhase),
    PepperoneHogCasingsCuringPhases
  },
  {
    "Pepperone and Tuscan Salami with Hog Middles Curing",
    "This program is used to cure Pepperone and Tuscan salami made with hog middles.",
    sizeof(PepperoneHogMiddlesCuringPhases)/sizeof(BoxProgramPhase),
    PepperoneHogMiddlesCuringPhases
  },
  {
    "Coppa Curing",
    "This program is used to cure Coppa.",
    sizeof(CoppaCuringPhases)/sizeof(BoxProgramPhase),
    CoppaCuringPhases
  },
  {
    "Chorizo Curing",
    "This program is used to cure Chorizo, Saucisson Sec and Hungarian salami.",
    sizeof(ChorizoCuringPhases)/sizeof(BoxProgramPhase),
    ChorizoCuringPhases
  },
  {
    "Landjager Curing",
    "This program is used to cure Landjager.",
    sizeof(LandjagerCuringPhases)/sizeof(BoxProgramPhase),
    LandjagerCuringPhases
  },
  {
    "Dry-cured Ham Curing",
    "This program is used to cure dry-cured ham, such as prosciutto.",
    sizeof(ProsciuttoCuringPhases)/sizeof(BoxProgramPhase),
    ProsciuttoCuringPhases
  },
  {
    "Country Ham Curing",
    "This program is used to cure country ham.",
    sizeof(CountryHamCuringPhases)/sizeof(BoxProgramPhase),
    CountryHamCuringPhases
  },
  {
    "Bresaola Curing",
    "This program is used to cure bresaola, lardo and cured pork belly.",
    sizeof(BresaolaCuringPhases)/sizeof(BoxProgramPhase),
    BresaolaCuringPhases
  },
  {
    "Duck Prosciutto Curing",
    "This program is used to cure duck prosciutto.",
    sizeof(DuckProsciuttoCuringPhases)/sizeof(BoxProgramPhase),
    DuckProsciuttoCuringPhases
  },
  {
    "Bloomy Rind Cheese Aging",
    "This program is used to age bloomy rind cheeses, such as Camembert, Truffle, Brie and Valencay.",
    sizeof(BloomyRindCheeseAgingPhases)/sizeof(BoxProgramPhase),
    BloomyRindCheeseAgingPhases
  },
  {
    "Blue Cheese Aging",
    "This program is used to age blue cheeses, such as Bisbee, Stilton and Blue Capricorn.",
    sizeof(BlueCheeseAgingPhases)/sizeof(BoxProgramPhase),
    BlueCheeseAgingPhases
  },
  {
    "Feta Cheese Aging",
    "This program is used to age Feta cheese.",
    sizeof(FetaCheeseAgingPhases)/sizeof(BoxProgramPhase),
    FetaCheeseAgingPhases
  },
  {
    "Caerphilly Cheese Aging",
    "This program is used to age Caerphilly and bandaged Cheddar cheese.",
    sizeof(CaerphillyCheeseAgingPhases)/sizeof(BoxProgramPhase),
    CaerphillyCheeseAgingPhases
  },
  {
    "Cheddar Cheese Aging",
    "This program is used to age Cheddar cheese (stirred curd and vacuum sealed).",
    sizeof(CheddarCheeseAgingPhases)/sizeof(BoxProgramPhase),
    CheddarCheeseAgingPhases
  },
};
