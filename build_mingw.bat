@ECHO OFF

REM Variables relevant to the compilation of the program.

SET CXX=g++

SET CPPSRCS=src/CoordGeodetic.cpp src/CoordTopocentric.cpp src/DateTime.cpp src/DecayedException.cpp src/Eci.cpp src/Globals.cpp src/Observer.cpp src/OrbitalElements.cpp src/SatelliteException.cpp src/SGP4.cpp src/SolarPosition.cpp src/TimeSpan.cpp src/Tle.cpp src/TleException.cpp src/Util.cpp src/Vector.cpp

SET EDCXXFLAGS=-I ./ -I ./include/ -Wall

SET EDLDFLAGS=-lurlmon

 

REM This run a command-prompt command to compile using G++.

ECHO Compiling %CPPOBJS% into %TARGET% using %CXX% with %EDCXXFLAGS% flag(s)...

CMD /c "%CXX% %EDCXXFLAGS% example/passpredict.cpp %CPPSRCS% -o passpredict.exe %EDLDFLAGS%"
CMD /c "%CXX% %EDCXXFLAGS% example/runtest.cpp %CPPSRCS% -o runtest.exe %EDLDFLAGS%"
CMD /c "%CXX% %EDCXXFLAGS% example/sattrack.cpp %CPPSRCS% -o sattrack.exe %EDLDFLAGS%"
CMD /c "%CXX% %EDCXXFLAGS% example/obtaintle.cpp %CPPSRCS% -o obtaintle.exe %EDLDFLAGS%"