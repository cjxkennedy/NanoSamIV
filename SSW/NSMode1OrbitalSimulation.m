%% ASEN Senior Projects Orbit Model Code
% Jade Babcock-Chi
% 2/14/23

%% Initialize Variables

clear; close all; clc;
Re = 6371; %[km]
a = Re + 417.5; %[km]
e = 0; 
i = 51.6; %[deg]
B = 20; %[deg]
mu = 3.986*10^5; %[km^3/s^2]
tstep = 0.02; %[s] (20 ms)
a1 = 150; %[km] tangential
a2 = 0; %[km] tangential

%% Call Functions + Plot Results

[vec, P, zos, flag, tangAlts] = orbitSim(a, e, i, B, mu, tstep, a1, a2, Re);

tangAltsShifted = [NaN(1,1); tangAlts(1:end-1, 1)];
tangAltsDiff = (tangAltsShifted - tangAlts)/0.02; %divide by time step (20 ms) to get to km/s

figure()
plot(vec',zos) %should never get to zero or pi, somewhere between
hold on
plot(vec', flag*3) %plot a 'high' where start or stop data collect 
title("ZOS for 1 Orbit Period");
ylabel("ZOS Angle [rad]");
xlabel("Time in Oribit [s]");
hold off

figure()
plot(vec', tangAlts) % lots of NaNs since we cant define a tangent altitude in most cases, largest is 417.5 km
hold on
plot(vec', flag*1000) %plot a 'high' where start or stop data collect 
title("Tangent Altitude for 1 Orbit Period");
ylabel("Tangent Altitude [km]");
xlabel("Time in Oribit [s]");
hold off

figure()
plot(vec', tangAltsDiff); %rate of change of tangent altitude vs. time
hold on
title("Tangent Altitude for 1 Orbit Period");
ylabel("Rate of Change of Tangent Altitude [km/s] ");
xlabel("Time in Oribit [s]");
hold off

%% Functions

function [tvec, P, zos, flag, tangAlts] = orbitSim(a, e, i, B, mu, tstep, a1, a2, Re)
    P = 2*pi*sqrt(a^3/mu);
    tvec = 0:tstep:P;
    tangAlts = zeros(length(tvec),1);
    flag = zeros(length(tvec),1);
    POS = (pi/180)*(90 - B); % [rad]
    Rtang1 = Re + a1;
    Rtang2 = Re + a2;
    Rsat = a;
    theta1 = pi/2 - acos(Rtang1/Rsat);
    theta2 = pi/2 - acos(Rtang2/Rsat);
    idx = 0;
    count = 1;
    for t = tvec
        idx = idx + 1;
        ZPS = t*2*pi/P; %[rad] fraction of the orbit we are in
        zps(idx,1) = ZPS;
        ZOS = acos(sin(POS)*cos(ZPS));
        zos(idx,1) = ZOS;
        if ZOS <= (pi - theta1 + (tstep/P)*2*pi) && ZOS >= (pi - theta1 - (tstep/P)*2*pi) %add bounds 0 +/- something
            %send command (write to port)
            tangAlts(idx,1) = convert2Tang(ZOS,Rsat,Re);
            flag(idx,1) = 1;
        elseif ZOS <= (pi - theta2 + (tstep/P)*2*pi) && ZOS >= (pi - theta2 - (tstep/P)*2*pi) %add bounds 150 +/- something
            %send command (write to port)
            tangAlts(idx,1) = convert2Tang(ZOS,Rsat,Re);
            flag(idx,1) = 1;
        elseif ZOS < (pi + theta2) && ZOS > (pi + theta1)
            %log data (read from port)
            tangAlts(idx,1) = convert2Tang(ZOS,Rsat,Re);
            flag(idx,1) = 1;
        else
            %do nothing
            tangAlts(idx,1) = convert2Tang(ZOS,Rsat,Re);
            flag(idx,1) = 0;
        end
        
        if flag(idx,1) == 1 %find only the 4 times that we issue commands
            if count == 1
                timeOfCmnd(count) = t;
                count = count + 1;
            elseif t > timeOfCmnd(count-1) + 1 %based on our time step flag = 1 greater than 4 times
                timeOfCmnd(count) = t;
                count = count + 1;
            end
        end
    end
    fprintf("Duration of Occultation Event: %.2f s\n", timeOfCmnd(2) - timeOfCmnd(1)); 
end

function tangAlt = convert2Tang(ZOS, Rsat, Re)
    theta = pi - ZOS;
    tangAlt = Rsat*sin(theta) - Re;
    if tangAlt < 0 % cannot have a negative tangent altitude
        tangAlt = NaN;
    end
end
