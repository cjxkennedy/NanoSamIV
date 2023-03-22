%% ASEN Senior Projects Orbit Model Code (M1)
% Jade Babcock-Chi
% Last Revision: 3/22/23

clear; close all; clc;
serialportlist;
% open serial port 
% check Teensy port # in device manager
% for example, COM7
teensy = serialport("/dev/tty.usbmodem80946601",9600) %teensy will ignore rate
% preallocate data
dataOutput = zeros(50000,20,"uint8"); % max 50000 samples
filename = "test_M1.mat";

%% Initialize Variables

mu = 3.986*10^5;    %[km^3/s^2] earth gravitation parameter
Re = 6371;          %[km]  earth radius
a = Re + 417.5;     %[km]  semi-major axis
e = 0;              %      eccentricity (circular)
i = 51.6;           %[deg] inclination
B = 0;             %[deg] beta angle
a1 = 150;           %[km]  tangential
a2 = 0;             %[km]  tangential
P = 2*pi*sqrt(a^3/mu);
tstep = 0.02;       %[s]   time step(20 ms)
tStart = 0;
tStop = P;
tvec = tStart:tstep:tStop; %0:tstep:P


%% INPUTS TO MATLAB APP

% Beta Angle (text input)
% Slider with time to start for testing purposes and stop for testing
% purposes

%% Call Functions + Plot Results

[vec, P, zos, flag, tangAlts, dataOutput] = orbitSim(a, B, tstep, a1, a2, Re, teensy, dataOutput, tvec);
save(filename,"dataOutput");
tangAltsShifted = [NaN(1,1); tangAlts(1:end-1, 1)];
tangAltsDiff = (tangAltsShifted - tangAlts)/0.02; %divide by time step (20 ms) to get to km/s

%% Functions

function [tvec, P, zos, flag, tangAlts, dataOutput] = orbitSim(a,B, tstep, a1, a2, Re, teensy, dataOutput, tvec)
   
    tangAlts = zeros(length(tvec),1);
    flag = zeros(length(tvec),1);
    POS = (pi/180)*(90 - B); % [rad]
    Rtang1 = Re + a1;
    Rtang2 = Re + a2;
    Rsat = a;
    theta1 = pi/2 - acos(Rtang1/Rsat);
    theta2 = pi/2 - acos(Rtang2/Rsat);
    idx = 0;
    count = 1; % unused variable, now used ?
    
    for t = tvec
        tic
        idx = idx + 1;
        ZPS = t*2*pi/P; %[rad] fraction of the orbit we are in
        zps(idx,1) = ZPS;
        ZOS = acos(sin(POS)*cos(ZPS));
        zos(idx,1) = ZOS;
        tangAlts(idx,1) = convert2Tang(ZOS,Rsat,Re);
        if ZOS <= (pi - theta1 + (tstep/P)*2*pi) && ZOS >= (pi - theta1 - (tstep/P)*2*pi)%add bounds 0 +/- something
            %send command (write to port)
            if t < P/2
                write(teensy,"@M1","string");
            elseif t > P/2
                write(teensy,"@SS","string");
            end
        elseif ZOS <= (pi - theta2 + (tstep/P)*2*pi) && ZOS >= (pi - theta2 - (tstep/P)*2*pi) %add bounds 150 +/- something
            %send command (write to port)
            if t < P/2
                write(teensy,"@SS","string");
            elseif t > P/2
                write(teensy,"@M1","string");
            end
        elseif ZOS < (pi - theta2) && ZOS > (pi - theta1)
            %log data (read from port)
            dataOutput(count,1:20) = read(teensy,20,"uint8");
            count = count + 1;
            flag(idx,1) = 1;
        else
            %do nothing
            flag(idx,1) = 0;
        end
        while toc < 0.0195 % 20ms
            pause(0.001)
        end
        %toc
        
    end
end

function tangAlt = convert2Tang(ZOS, Rsat, Re)
    theta = pi - ZOS;
    tangAlt = Rsat*sin(theta) - Re;
end

%         pause(0.0195 - toc)
%         toc
        % option for get 
