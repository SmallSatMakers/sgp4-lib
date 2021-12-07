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

#include "Tle.hpp"

#include <stdlib.h>
#include <meb_print.h>
#include <string.h>

#include <locale>

namespace LSGP4
{
    namespace
    {
        static const unsigned int TLE1_COL_NORADNUM = 2;
        static const unsigned int TLE1_LEN_NORADNUM = 5;
        static const unsigned int TLE1_COL_INTLDESC_A = 9;
        static const unsigned int TLE1_LEN_INTLDESC_A = 2;
        //  static const unsigned int TLE1_COL_INTLDESC_B = 11;
        static const unsigned int TLE1_LEN_INTLDESC_B = 3;
        //  static const unsigned int TLE1_COL_INTLDESC_C = 14;
        static const unsigned int TLE1_LEN_INTLDESC_C = 3;
        static const unsigned int TLE1_COL_EPOCH_A = 18;
        static const unsigned int TLE1_LEN_EPOCH_A = 2;
        static const unsigned int TLE1_COL_EPOCH_B = 20;
        static const unsigned int TLE1_LEN_EPOCH_B = 12;
        static const unsigned int TLE1_COL_MEANMOTIONDT2 = 33;
        static const unsigned int TLE1_LEN_MEANMOTIONDT2 = 10;
        static const unsigned int TLE1_COL_MEANMOTIONDDT6 = 44;
        static const unsigned int TLE1_LEN_MEANMOTIONDDT6 = 8;
        static const unsigned int TLE1_COL_BSTAR = 53;
        static const unsigned int TLE1_LEN_BSTAR = 8;
        //  static const unsigned int TLE1_COL_EPHEMTYPE = 62;
        //  static const unsigned int TLE1_LEN_EPHEMTYPE = 1;
        //  static const unsigned int TLE1_COL_ELNUM = 64;
        //  static const unsigned int TLE1_LEN_ELNUM = 4;

        static const unsigned int TLE2_COL_NORADNUM = 2;
        static const unsigned int TLE2_LEN_NORADNUM = 5;
        static const unsigned int TLE2_COL_INCLINATION = 8;
        static const unsigned int TLE2_LEN_INCLINATION = 8;
        static const unsigned int TLE2_COL_RAASCENDNODE = 17;
        static const unsigned int TLE2_LEN_RAASCENDNODE = 8;
        static const unsigned int TLE2_COL_ECCENTRICITY = 26;
        static const unsigned int TLE2_LEN_ECCENTRICITY = 7;
        static const unsigned int TLE2_COL_ARGPERIGEE = 34;
        static const unsigned int TLE2_LEN_ARGPERIGEE = 8;
        static const unsigned int TLE2_COL_MEANANOMALY = 43;
        static const unsigned int TLE2_LEN_MEANANOMALY = 8;
        static const unsigned int TLE2_COL_MEANMOTION = 52;
        static const unsigned int TLE2_LEN_MEANMOTION = 11;
        static const unsigned int TLE2_COL_REVATEPOCH = 63;
        static const unsigned int TLE2_LEN_REVATEPOCH = 5;
    }

    /**
 * Initialise the tle object.
 * @exception TleException
 */
    void Tle::Initialize()
    {
        if (!IsValidLineLength(line_one_))
        {
            throw TleException("Invalid length for line one");
        }

        if (!IsValidLineLength(line_two_))
        {
            throw TleException("Invalid length for line two");
        }

        if (line_one_[0] != '1')
        {
            throw TleException("Invalid line beginning for line one");
        }

        if (line_two_[0] != '2')
        {
            throw TleException("Invalid line beginning for line two");
        }

        unsigned int sat_number_1;
        unsigned int sat_number_2;

        ExtractInteger(line_one_.substr(TLE1_COL_NORADNUM,
                                        TLE1_LEN_NORADNUM),
                       sat_number_1);
        ExtractInteger(line_two_.substr(TLE2_COL_NORADNUM,
                                        TLE2_LEN_NORADNUM),
                       sat_number_2);

        if (sat_number_1 != sat_number_2)
        {
            throw TleException("Satellite numbers do not match");
        }

        norad_number_ = sat_number_1;

        if (name_.empty())
        {
            name_ = line_one_.substr(TLE1_COL_NORADNUM, TLE1_LEN_NORADNUM);
        }

        int_designator_ = line_one_.substr(TLE1_COL_INTLDESC_A,
                                           TLE1_LEN_INTLDESC_A + TLE1_LEN_INTLDESC_B + TLE1_LEN_INTLDESC_C);

        unsigned int year = 0;
        double day = 0.0;

        ExtractInteger(line_one_.substr(TLE1_COL_EPOCH_A,
                                        TLE1_LEN_EPOCH_A),
                       year);
        ExtractDouble(line_one_.substr(TLE1_COL_EPOCH_B,
                                       TLE1_LEN_EPOCH_B),
                      4, day);
        ExtractDouble(line_one_.substr(TLE1_COL_MEANMOTIONDT2,
                                       TLE1_LEN_MEANMOTIONDT2),
                      2, mean_motion_dt2_);
        ExtractExponential(line_one_.substr(TLE1_COL_MEANMOTIONDDT6,
                                            TLE1_LEN_MEANMOTIONDDT6),
                           mean_motion_ddt6_);
        ExtractExponential(line_one_.substr(TLE1_COL_BSTAR,
                                            TLE1_LEN_BSTAR),
                           bstar_);

        /*
     * line 2
     */
        ExtractDouble(line_two_.substr(TLE2_COL_INCLINATION,
                                       TLE2_LEN_INCLINATION),
                      4, inclination_);
        ExtractDouble(line_two_.substr(TLE2_COL_RAASCENDNODE,
                                       TLE2_LEN_RAASCENDNODE),
                      4, right_ascending_node_);
        ExtractDouble(line_two_.substr(TLE2_COL_ECCENTRICITY,
                                       TLE2_LEN_ECCENTRICITY),
                      -1, eccentricity_);
        ExtractDouble(line_two_.substr(TLE2_COL_ARGPERIGEE,
                                       TLE2_LEN_ARGPERIGEE),
                      4, argument_perigee_);
        ExtractDouble(line_two_.substr(TLE2_COL_MEANANOMALY,
                                       TLE2_LEN_MEANANOMALY),
                      4, mean_anomaly_);
        ExtractDouble(line_two_.substr(TLE2_COL_MEANMOTION,
                                       TLE2_LEN_MEANMOTION),
                      3, mean_motion_);
        ExtractInteger(line_two_.substr(TLE2_COL_REVATEPOCH,
                                        TLE2_LEN_REVATEPOCH),
                       orbit_number_);

        if (year < 57)
            year += 2000;
        else
            year += 1900;

        epoch_ = DateTime(year, day);

        initd = true;
    }

    /**
 * Check 
 * @param str The string to check
 * @returns Whether true of the string has a valid length
 */
    bool Tle::IsValidLineLength(const std::string &str)
    {
        return str.length() == LineLength() ? true : false;
    }

    /**
 * Convert a string containing an integer
 * @param[in] str The string to convert
 * @param[out] val The result
 * @exception TleException on conversion error
 */
    void Tle::ExtractInteger(const std::string &str, unsigned int &val)
    {
        bool found_digit = false;
        unsigned int temp = 0;

        for (std::string::const_iterator i = str.begin(); i != str.end(); ++i)
        {
            if (isdigit(*i))
            {
                found_digit = true;
                temp = (temp * 10) + static_cast<unsigned int>(*i - '0');
            }
            else if (found_digit)
            {
                throw TleException("Unexpected non digit");
            }
            else if (*i != ' ')
            {
                throw TleException("Invalid character");
            }
        }

        if (!found_digit)
        {
            val = 0;
        }
        else
        {
            val = temp;
        }
    }

    /**
 * Convert a string containing an double
 * @param[in] str The string to convert
 * @param[in] point_pos The position of the decimal point. (-1 if none)
 * @param[out] val The result
 * @exception TleException on conversion error
 */
    void Tle::ExtractDouble(const std::string &str, int point_pos, double &val)
    {
        std::string temp;
        bool found_digit = false;

        for (std::string::const_iterator i = str.begin(); i != str.end(); ++i)
        {
            /*
         * integer part
         */
            if (point_pos >= 0 && i < str.begin() + point_pos - 1)
            {
                bool done = false;

                if (i == str.begin())
                {
                    if (*i == '-' || *i == '+')
                    {
                        /*
                     * first character could be signed
                     */
                        temp += *i;
                        done = true;
                    }
                }

                if (!done)
                {
                    if (isdigit(*i))
                    {
                        found_digit = true;
                        temp += *i;
                    }
                    else if (found_digit)
                    {
                        throw TleException("Unexpected non digit");
                    }
                    else if (*i != ' ')
                    {
                        throw TleException("Invalid character");
                    }
                }
            }
            /*
         * decimal point
         */
            else if (point_pos >= 0 && i == str.begin() + point_pos - 1)
            {
                if (temp.length() == 0)
                {
                    /*
                 * integer part is blank, so add a '0'
                 */
                    temp += '0';
                }

                if (*i == '.')
                {
                    /*
                 * decimal point found
                 */
                    temp += *i;
                }
                else
                {
                    throw TleException("Failed to find decimal point");
                }
            }
            /*
         * fraction part
         */
            else
            {
                if (i == str.begin() && point_pos == -1)
                {
                    /*
                 * no decimal point expected, add 0. beginning
                 */
                    temp += '0';
                    temp += '.';
                }

                /*
             * should be a digit
             */
                if (isdigit(*i))
                {
                    temp += *i;
                }
                else
                {
                    throw TleException("Invalid digit");
                }
            }
        }

        if (!Util::FromString<double>(temp, val))
        {
            throw TleException("Failed to convert value to double");
        }
    }

    /**
 * Convert a string containing an exponential
 * @param[in] str The string to convert
 * @param[out] val The result
 * @exception TleException on conversion error
 */
    void Tle::ExtractExponential(const std::string &str, double &val)
    {
        std::string temp;

        for (std::string::const_iterator i = str.begin(); i != str.end(); ++i)
        {
            if (i == str.begin())
            {
                if (*i == '-' || *i == '+' || *i == ' ')
                {
                    if (*i == '-')
                    {
                        temp += *i;
                    }
                    temp += '0';
                    temp += '.';
                }
                else
                {
                    throw TleException("Invalid sign");
                }
            }
            else if (i == str.end() - 2)
            {
                if (*i == '-' || *i == '+')
                {
                    temp += 'e';
                    temp += *i;
                }
                else
                {
                    throw TleException("Invalid exponential sign");
                }
            }
            else
            {
                if (isdigit(*i))
                {
                    temp += *i;
                }
                else
                {
                    throw TleException("Invalid digit");
                }
            }
        }

        if (!Util::FromString<double>(temp, val))
        {
            throw TleException("Failed to convert value to double");
        }
    }

#ifdef OS_Windows
#include <windows.h>
#include <tchar.h>
#include <urlmon.h>

#pragma comment(lib, "urlmon.lib")

#endif

        void
        Tle::UpdateFromNetwork(const char *url)
    {
        if (!initd)
        {
            dbprintlf(FATAL "Object not initialized");
            return;
        }
        FILE *pp = NULL;
#ifndef OS_Windows
        char cmd[512];
        snprintf(cmd, sizeof(cmd), "wget -q -O- %s", url);
        pp = popen(cmd,
                   "r");
#else // Windows detected, here we get file from internet, save it, and read it back into pp
        char *tempfile = "webstream.tmp";

        if (S_OK != URLDownloadToFile(NULL, url, tempfile, 0, NULL))
        {
            dbprintlf("Could not download TLE data");
            return;
        }

        pp = fopen(tempfile, "r");
#endif
        if (pp == NULL)
        {
            dbprintlf(RED_FG "Could not open pipe to obtain TLE data");
            return;
        }
        char search_str1[128];
        snprintf(search_str1, sizeof(search_str1), "1 %u", NoradNumber());
        char search_str2[128];
        snprintf(search_str2, sizeof(search_str2), "2 %u", NoradNumber());
        char l1[128];
        char l2[128];
        char buf[128];
        memset(l1, 0x0, sizeof(l1));
        memset(l2, 0x0, sizeof(l2));
        memset(buf, 0x0, sizeof(buf));
        bool l1_updated = false;
        bool l2_updated = false;
        while (fgets(buf, sizeof(buf), pp))
        {
            if (strstr(buf, search_str1) != NULL)
            {
                strcpy(l1, buf);
                l1_updated = true;
            }
            if (strstr(buf, search_str2) != NULL)
            {
                strcpy(l2, buf);
                l2_updated = true;
            }
        }
        pclose(pp);
        if (l1_updated && l2_updated)
        {
            tprintlf("Update: Obtained updated TLE for %u", NoradNumber());
            l1[69] = '\0';
            l2[69] = '\0';
            Update(l1, l2);
        }
        else
        {
            tprintlf("Update: Object %u not found", NoradNumber());
        }
        fclose(pp);
#ifdef OS_Windows
        // Clean up temp files etc
        _unlink(tempfile);
#endif // OS_Windows
        return;
    }

    Tle::Tle(const unsigned int norad_id, const char *url)
    : initd(false)
    {
        if (url == NULL || url == nullptr)
        {
            throw std::invalid_argument("URL is NULL");
        }
        if (norad_id == 0)
        {
            throw std::invalid_argument("Invalid NORAD ID");
        }
        bool object_found = false;
        FILE *pp = NULL;
#ifndef OS_Windows
        char cmd[512];
        snprintf(cmd, sizeof(cmd), "wget -q -O- %s", url);
        pp = popen(cmd,
                   "r");
#else // Windows detected, here we get file from internet, save it, and read it back into pp
        if (S_OK != URLDownloadToFile(NULL, url, tempfile, 0, NULL))
        {
            dbprintlf("Could not download TLE data");
            return;
        }

        pp = fopen(tempfile, "r");
#endif
        if (pp == NULL)
        {
            dbprintlf(RED_FG "Could not open pipe to obtain TLE data");
            return;
        }
        char search_str1[128];
        snprintf(search_str1, sizeof(search_str1), "1 %u", norad_id);
        char search_str2[128];
        snprintf(search_str2, sizeof(search_str2), "2 %u", norad_id);
        char l1[128];
        char l2[128];
        char buf[128];
        memset(l1, 0x0, sizeof(l1));
        memset(l2, 0x0, sizeof(l2));
        memset(buf, 0x0, sizeof(buf));
        bool l1_updated = false;
        bool l2_updated = false;
        while (fgets(buf, sizeof(buf), pp))
        {
            if (strstr(buf, search_str1) != NULL)
            {
                strcpy(l1, buf);
                l1_updated = true;
            }
            if (strstr(buf, search_str2) != NULL)
            {
                strcpy(l2, buf);
                l2_updated = true;
            }
        }
        if (l1_updated && l2_updated)
        {
            tprintlf("Update: Obtained updated TLE for %u", norad_id);
            l1[69] = '\0';
            l2[69] = '\0';
            object_found = true;
        }
        else
        {
            tprintlf("Update: Object %u not found", norad_id);
        }
#ifdef OS_Windows
        // Clean up temp files etc
        fclose(pp);
        _unlink(tempfile);
#else
        pclose(pp);
#endif // OS_Windows
        if (object_found)
        {
            line_one_ = l1;
            line_two_ = l2;
            Initialize();
        }
    }
};

std::vector<LSGP4::Tle> ReadTleFromFile(const char *fname)
{
    int m_obj = 1;
    int n_obj = 0;

    FILE *fp = fopen(fname, "r");
    if (fp == NULL)
    {
        printf("Error opening TLE file %s, exiting\n", fname);
        throw std::invalid_argument("Could not access file");
    }

    std::vector<LSGP4::Tle> objs(m_obj);
    // start reading TLE file
    char *res;
    char l1[128];
    char l2[128];
    memset(l1, 0x0, sizeof(l1));
    memset(l2, 0x0, sizeof(l2));
    do
    {
        char buf[128];
        memset(buf, 0x0, sizeof(buf));
        // 1. Read one line
        res = fgets(buf, sizeof(buf), fp);
        buf[strcspn(buf, "\n")] = '\0';
        if (strlen(buf) != 69) // L1?
            continue;
        if (buf[0] != '1') // not L1
        {
            dbprintlf(FATAL "Expecting Line 1, %c, read %s", buf[0], buf);
            continue;
        }
        if (m_obj <= n_obj)
        {
            m_obj++;
            objs.resize(m_obj);
        }
        strncpy(l1, buf, 69); // L1 is found, read it in
        l1[69] = '\0';
        // 2. Read line 2
        res = fgets(buf, sizeof(buf), fp);
        buf[strcspn(buf, "\n")] = '\0';
        if (strlen(buf) != 69) // L2?
            continue;
        if (buf[0] != '2') // not L2
        {
            dbprintlf(FATAL "Expecting Line 2, read %s", buf);
            continue;
        }
        strncpy(l2, buf, 69); // L2 is found, read it in
        l2[69] = '\0';
        objs.at(n_obj).Update(l1, l2);
        n_obj++;
    } while (res != NULL);
    fclose(fp);
    return objs;
}