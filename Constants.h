#ifndef __CONSTANTS__H_
#define __CONSTANTS__H_

//constant for switching between linux and windows
#define LINUX

#define WALL 'W'
#define SPACE ' '
#define DOCK 'D'
#define UNDISCOVERD_UNREACHABLE '?'
#define UNDISCOVERD_REACHABLE 'U'
#define BLOCKED '-'
#define TARGET 'T'
#define ROBOT 'R'

#define IMG_ROBOT "R"
#define IMG_SPACE "0"

#define IMG_ROBOT_DOCKING "D_R"
#define IMG_ROBOT_CLEANING_ODD "R_1"
#define IMG_ROBOT_CLEANING_EVEN "R_2"
#define IMG_ROBOT_NORTH "R_n"
#define IMG_ROBOT_SOUTH "R_s"
#define IMG_ROBOT_EAST "R_e"
#define IMG_ROBOT_WEST "R_w"

#define DISTANCE_FACTOR	20

#define DEFAULT_NUM_THREADS 1

#define SPACER "          "
#define VALID "valid house"
#define MISSING_DOCK "missing docking station (no D in house)"
#define TOO_MANY_DOCKS "too many docking stations (more than one D in house)"
#define BAD_FILE "cannot open file"

#define INVALID_SO "file cannot be loaded or is not a valid .so"
#define NO_REG_SO "valid .so but no algorithm was registered after loading it"
#define INVALID_SCORE_FORMULA "score_formula.so is a valid .so but it does not have a valid score formula"
#define BAD_SCORE "Score formula could not calculate some scores, see -1 in the results table"

#define USAGE_MSG "Usage: simulator [-config <config path>] [-house_path <house path>] [-algorithm_path <algorithm path>] [-score_formula <score .so path>] [-threads <num threads>]"

#endif //__CONSTANTS__H_