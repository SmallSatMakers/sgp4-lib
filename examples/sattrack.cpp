/*
 * Copyright 2013 Daniel Warner <contact@danrw.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <CoordTopocentric.hpp>
#include <CoordGeodetic.hpp>
#include <Observer.hpp>
#include <SGP4.hpp>

#include <iostream>

using namespace LSGP4;

int main()
{
    Observer obs(51.507406923983446, -0.12773752212524414, 0.05); // observer location
    Tle tle = Tle("ISS (ZARYA)             ",                     // default TLE
                  "1 25544U 98067A   21337.49738641 -.00000450  00000+0  00000+0 0  9996",
                  "2 25544  51.6389 225.5617 0004535 264.0051 276.0775 15.48792552314838");
    SGP4 sgp4(tle);                                                       // SGP4 model for current TLE
    std::string url = "http://celestrak.com/NORAD/elements/stations.txt"; // Network location for TLE text data
    tle.UpdateFromNetwork(url.c_str());                                   // Update TLE data
    sgp4.SetTle(tle);                                                     // Update SGP4 model for new TLE

    std::cout << tle << std::endl;

    for (int i = 0; i < 10; ++i)
    {
        DateTime dt = tle.Epoch().AddMinutes(i * 10);
        /*
         * calculate satellite position
         */
        Eci eci = sgp4.FindPosition(dt);
        /*
         * get look angle for observer to satellite
         */
        CoordTopocentric topo = obs.GetLookAngle(eci);
        /*
         * convert satellite position to geodetic coordinates
         */
        CoordGeodetic geo = eci.ToGeodetic();

        std::cout << dt << " " << topo << " " << geo << std::endl;
    };

    return 0;
}
