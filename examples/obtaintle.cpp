/**
 * @file obtaintle.cpp
 * @author Sunip K. Mukherjee (sunipkmukherjee@gmail.com)
 * @brief Obtains TLE for ISS from internet and prints it
 * @version 0.1
 * @date 2021-12-07
 * 
 * @copyright Copyright (c) 2021
 * 
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
    const char *url = "http://celestrak.com/NORAD/elements/stations.txt";
    Tle tle = Tle(25544, url);
    std::cout << tle << std::endl;
    return 0;
}
