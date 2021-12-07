@ECHO OFF

REM Variables relevant to the compilation of the program.

SET CXX=cl

SET CPPSRCS=src\CoordGeodetic.cpp src\CoordTopocentric.cpp src\DateTime.cpp src\DecayedException.cpp src\Eci.cpp src\Globals.cpp src\Observer.cpp src\OrbitalElements.cpp src\SatelliteException.cpp src\SGP4.cpp src\SolarPosition.cpp src\TimeSpan.cpp src\Tle.cpp src\TleException.cpp src\Util.cpp src\Vector.cpp

SET EDCXXFLAGS=-I .\ -I .\include\ -Wall -std=c++11

SET EDLDFLAGS=-lurlmon

 

REM This run a command-prompt command to compile using G++.

ECHO Compiling %CPPSRCS% into %TARGET% using %CXX% with %EDCXXFLAGS% flag(s)...

CMD /c "%CXX% %EDCXXFLAGS% examples\passpredict.cpp %CPPSRCS% -o passpredict.exe %EDLDFLAGS%"
CMD /c "%CXX% %EDCXXFLAGS% examples\runtest.cpp %CPPSRCS% -o runtest.exe %EDLDFLAGS%"
CMD /c "%CXX% %EDCXXFLAGS% examples\sattrack.cpp %CPPSRCS% -o sattrack.exe %EDLDFLAGS%"
CMD /c "%CXX% %EDCXXFLAGS% examples\obtaintle.cpp %CPPSRCS% -o obtaintle.exe %EDLDFLAGS%"