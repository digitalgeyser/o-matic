/**
 * default built-in programs for the curing app.
 */

typedef struct {
  uint8_t startTemperature;
  uint8_t endTemperature;
  uint8_t startHumidity;
  uint8_t endHumidity;
  uint16_t durationInMinutes;
} BoxProgramPhase;

typedef struct {
  const char *programName;
  int numberOfPhases;
  const BoxProgramPhase *phases;
} BoxProgram;

#define INFINITY_MINUTES 0

// For each program, define an array of phases.
// Each phase has 5 numbers:
//    start T in degreesCelsius
//    end T in degreesCelsius
//    start humidity in percentages
//    end humidity in percentages
//    duration of the phase in minutes (if infinity, then use INFINITY_MINUTES)
const BoxProgramPhase program1Phases[] = {
  { 20, 40, 40, 60, 100 },
  { 20, 40, 40, 60, 100 }
};

const BoxProgramPhase program2Phases[] = {
  { 20, 40, 40, 60, 100 },
};

const BoxProgram defaultPrograms[] = {
  {
    "Program1",
    2,
    program1Phases
  },
  {
    "Program2",
    1,
    program2Phases
  }
};