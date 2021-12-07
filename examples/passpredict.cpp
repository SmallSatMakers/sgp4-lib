/**
 * @file predict.cpp
 * @author Mit Bailey (mitbailey99@gmail.com)
 * @brief Displays visible pass times for a given satellite.
 * @version See Git tags for version information.
 * @date 2021.11.19
 *
 * @copyright Copyright (c) 2021
 *
 */

#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif
#include <errno.h>
#include <string.h>
#include "CoordTopocentric.hpp"
#include "DateTime.hpp"
#include "Observer.hpp"
#include "SGP4.hpp"
#include "meb_print.h"

using namespace LSGP4;

#define SEC *1000000.0           // nanoseconds to seconds
#define DEG *(180.0 / 3.1415926) // radians to degrees
#define GS_LAT 42.655583
#define GS_LON -71.325433
#define GS_ELEV 0.061 // Lowell ASL + Olney Height; Kilometers for some reason.
#define MIN_ELEV 10.0 // degrees
#define TLE_LEN 70

#define WEBSTREAM_URL "http://celestrak.com/NORAD/elements/stations.txt"

#ifdef OS_Windows
#include <windows.h>
#include <urlmon.h>

#define WEBSTREAM_TMP "webstream.tmp"

#endif // OS_Windows

// TODO: Update README.md file.

// TLE Object SX [https://www.n2yo.com/satellite/?s=49278]
// TLE Object SW [https://www.n2yo.com/satellite/?s=49277]

/*
 * Usage:
 * predict.out TLE.txt output.txt
 * predict.out TLE.txt output.txt {Days to Predict}
 * predict.out NORADID output.txt
 * predict.out NORADID output.txt {Days to Predict}
 */
int main(int argc, char *argv[])
{
    char TLE[2][TLE_LEN] = {0};
    FILE *fp_out = NULL;

    char identifier[256] = {0}; // NORAD ID or TLE filename.
    char fname_out[256] = {0};
    int days_to_predict = 2;   // Predict two days (48 hours) by default.
    bool use_norad_id = true;  // Assume the use of a NORAD ID by default.
    bool use_out_file = false; // No out-file printing by default.

    if (argc == 1)
    {
        // ./predict.out
        // Ask the user for input.

        // TODO: Make last two optional

        bprintf("NORAD ID or TLE input filename: ");
        if (scanf("%s", identifier)) {}

        bprintf("Output filename: ");
        if (scanf("%s", fname_out)) {}
        use_out_file = true;

        bprintf("Days to predict: ");
        if (scanf("%d", &days_to_predict)) {}
    }
    else if (argc == 2)
    {
        // ./predict.out {Input File}
        // ./predict.out {NORAD ID}

        // TODO: Add usages printout and exit() if user uses -help.

        strcpy(identifier, argv[1]);
    }
    else if (argc == 3)
    {
        // ./predict.out {Input File} {Output File}
        // ./predict.out {NORAD ID} {Output File}
        // Default prediction time.

        strcpy(identifier, argv[1]);
        strcpy(fname_out, argv[2]);
        use_out_file = true;
    }
    else if (argc == 4)
    {
        // ./predict.out {Input File} {Output File} {Days to Predict}
        // ./predict.out {NORAD ID} {Output File} {Days to Predict}
        // Given prediction time.

        strcpy(identifier, argv[1]);
        strcpy(fname_out, argv[2]);
        use_out_file = true;
        days_to_predict = atoi(argv[3]);
    }
    else
    {
        dbprintlf(FATAL "Invalid usage.");
        dbprintlf("Usages (" RED_FG "required" TERMINATOR ", " BLUE_FG "optional" TERMINATOR "):");
        dbprintlf(RED_FG "./predict.out");
        dbprintlf(RED_FG "./predict.out <Five-Digit NORAD ID or Input File> " TERMINATOR BLUE_FG "[Output File] [Days to Predict]");

        return -1;
    }

    for (size_t i = 0; i < strlen(identifier); i++)
    {
        if (!isdigit(identifier[i]))
        {
            // Non-digit found, cannot be NORAD ID.
            use_norad_id = false;
            break;
        }
    }
    bprintlf();

    if (use_norad_id)
    {
        dbprintlf("NORAD ID detected, using %s.", identifier);
    }
    else
    {
        dbprintlf("Filename detected, using %s.", identifier);
    }

    Tle tle;

    // If filename is given, continue on as before.
    // If NORAD ID is given, fetch the TLE from the internet.
    if (use_norad_id)
    {
        tle = Tle((unsigned int) atoi(identifier), WEBSTREAM_URL);
    }
    else
    {
        // Read from file.
        auto tles = ReadTleFromFile(identifier);
        if (tles.size() == 0)
            throw std::invalid_argument("Could not read valid TLEs");
        tle = tles[0];
    }

    // Open the output file.
    if (use_out_file)
    {
        fp_out = fopen(fname_out, "w");
        if (fp_out == NULL)
        {
            dbprintlf(FATAL "Failed to open %s for write.", fname_out);
            return -5;
        }
    }

    bprintlf();
    bprintlf("PASS PREDICTIONS FOR:");
    bprintlf("%s", TLE[0]);
    bprintlf("%s", TLE[1]);

    SGP4 *satellite = new SGP4(tle);
    Observer *dish = new Observer(GS_LAT, GS_LON, GS_ELEV);
    DateTime tnow = DateTime::Now(true);

    Eci pos_now = satellite->FindPosition(tnow);
    CoordTopocentric current_pos = dish->GetLookAngle(pos_now);
    CoordGeodetic current_lla = pos_now.ToGeodetic();

    bprintlf(YELLOW_FG "DATA FOR %d DAYS", days_to_predict);

    bprintlf("Current Position: %.2f AZ, %.2f EL | %.2f LA, %.2f LN", current_pos.azimuth DEG, current_pos.elevation DEG, current_lla.latitude DEG, current_lla.longitude DEG);
    DateTime tnext = tnow;
    bool in_pass = false;
    double max_el = 0.0;
    double max_el_az = 0.0;
    DateTime max_el_time;
    int begin_et = 0;

    TimeSpan FiveHours(5, 0, 0);
    DateTime tEST = tnow - FiveHours;

    if (use_out_file)
    {
        fprintf(fp_out, "Report Generated %04d.%02d.%02d %02d:%02d:%02d EST\nData for %d days.\n\n", tEST.Year(), tEST.Month(), tEST.Day(), tEST.Hour(), tEST.Minute(), tEST.Second(), days_to_predict);

        fprintf(fp_out, "%s\n%s\n", tle.Line1().c_str(), tle.Line2().c_str());
        fprintf(fp_out, "======================================================================\n\n");
    }

    bprintlf("It is currently %04d.%02d.%02d %02d:%02d:%02d UTC\n", tnow.Year(), tnow.Month(), tnow.Day(), tnow.Hour(), tnow.Minute(), tnow.Second());

    bprintlf("It is currently %04d.%02d.%02d %02d:%02d:%02d EST\n", tEST.Year(), tEST.Month(), tEST.Day(), tEST.Hour(), tEST.Minute(), tEST.Second());

    for (int i = 0; i < 86400 * days_to_predict; i++)
    {
        tnext = tnext.AddSeconds(1);
        Eci eci_ahd = satellite->FindPosition(tnext);
        CoordTopocentric pos_ahd = dish->GetLookAngle(eci_ahd);
        double ahd_el = pos_ahd.elevation DEG;
        if (ahd_el > 10.0)
        {
            if (!in_pass)
            {
                bprintlf("== SATELLITE PASS (Now + %d minutes) ==", i / 60);
                bprintlf("Time (EST)             Az (deg)   El (deg)");
                bprintlf("------------------------------------------");
                if (use_out_file)
                {
                    fprintf(fp_out, "== SATELLITE PASS (Now + %d minutes) ==\n", i / 60);
                    fprintf(fp_out, "Time (EST)             Az (deg)   El (deg)\n");
                    fprintf(fp_out, "------------------------------------------\n");
                }
                in_pass = true;
                begin_et = i;
            }

            if (ahd_el > max_el)
            {
                max_el = ahd_el;
                max_el_az = pos_ahd.azimuth DEG;
                max_el_time = tnext;
            }

            // Print data for every 60 seconds after the pass begins, and once immediately when the pass begins..
            if ((i - begin_et) % 60 == 0)
            {

                tEST = tnext - FiveHours;
                bprintlf("%04d.%02d.%02d %02d:%02d:%02d    %6.02lf     %6.02lf", tEST.Year(), tEST.Month(), tEST.Day(), tEST.Hour(), tEST.Minute(), tEST.Second(), pos_ahd.azimuth DEG, ahd_el);

                if (use_out_file)
                {
                    fprintf(fp_out, "%04d.%02d.%02d %02d:%02d:%02d    %6.02lf     %6.02lf\n", tEST.Year(), tEST.Month(), tEST.Day(), tEST.Hour(), tEST.Minute(), tEST.Second(), pos_ahd.azimuth DEG, ahd_el);
                }
            }
        }
        else if (in_pass)
        {
            // Make sure to print final state when pass ends.
            tEST = tnext - FiveHours;
            bprintlf("%04d.%02d.%02d %02d:%02d:%02d    %6.02lf     %6.02lf\n", tEST.Year(), tEST.Month(), tEST.Day(), tEST.Hour(), tEST.Minute(), tEST.Second(), pos_ahd.azimuth DEG, ahd_el);

            if (use_out_file)
            {
                fprintf(fp_out, "%04d.%02d.%02d %02d:%02d:%02d    %6.02lf     %6.02lf\n", tEST.Year(), tEST.Month(), tEST.Day(), tEST.Hour(), tEST.Minute(), tEST.Second(), pos_ahd.azimuth DEG, ahd_el);
            }

            bprintlf("Pass Statistics");
            bprintlf("    Maximum Elevation:");
            tEST = max_el_time - FiveHours;
            bprintlf("%04d.%02d.%02d %02d:%02d:%02d    %6.02lf     %6.02lf", tEST.Year(), tEST.Month(), tEST.Day(), tEST.Hour(), tEST.Minute(), tEST.Second(), max_el_az, max_el);
            printf("\n\n");
            if (use_out_file)
            {
                fprintf(fp_out, "\nPass Statistics\n    Maximum Elevation:\n%04d.%02d.%02d %02d:%02d:%02d    %6.02lf     %6.02lf\n\n\n", tEST.Year(), tEST.Month(), tEST.Day(), tEST.Hour(), tEST.Minute(), tEST.Second(), max_el_az, max_el);
            }

            in_pass = false;
            max_el = 0;
        }
    }

    if (use_out_file)
    {
        fclose(fp_out);
    }

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    remove(WEBSTREAM_TMP);
#endif // defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

    return 0;
}