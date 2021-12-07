@ECHO OFF

REM Variables relevant to the compilation of the program.

SET CXX=cl

SET CPPSRCS=src\CoordGeodetic.cpp src\CoordTopocentric.cpp src\DateTime.cpp src\DecayedException.cpp src\Eci.cpp src\Globals.cpp src\Observer.cpp src\OrbitalElements.cpp src\SatelliteException.cpp src\SGP4.cpp src\SolarPosition.cpp src\TimeSpan.cpp src\Tle.cpp src\TleException.cpp src\Util.cpp src\Vector.cpp

SET EDCXXFLAGS= /I .\ /I .\include\ /W0 /EHsc

SET EDLDFLAGS=

 

REM This run a command-prompt command to compile using G++.

ECHO Compiling %CPPSRCS% into %TARGET% using %CXX% with %EDCXXFLAGS% flag(s)...

CMD /c "%CXX% %EDCXXFLAGS% examples\passpredict.cpp %CPPSRCS% /Fe: passpredict.exe %EDLDFLAGS%"
CMD /c "%CXX% %EDCXXFLAGS% examples\runtest.cpp %CPPSRCS% /Fe: runtest.exe %EDLDFLAGS%"
CMD /c "%CXX% %EDCXXFLAGS% examples\sattrack.cpp %CPPSRCS% /Fe: sattrack.exe %EDLDFLAGS%"
CMD /c "%CXX% %EDCXXFLAGS% examples\obtaintle.cpp %CPPSRCS% /Fe: obtaintle.exe %EDLDFLAGS%"