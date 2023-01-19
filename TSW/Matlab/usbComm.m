%% USB Serial Communication Test %%
% 1) Communicates with Teensy 
%       Sends start command and begins to recevie data
%       After data collection point, we "propagate"
%       Propagation determines if we are still in collection mode
%       Or if we pass maxTime configured on Teensy/Matlab 
%       Send stop command
% 2) Processes data received and plots time of each collection point
%       Configure time to start at 0.
%       Graph time vs collection point (second*50Hz)
clear all; clc;
maxTime = 30; % seconds
%% Data Collection %%
device = serialport("COM6",9600); % setup USB connection
% Start of Collection %
command = "@M1";
write(device,command,"string");
[timeData, photodiodeData] = startData(maxTime,device);
% End of Collection %
command = "@S1";
write(device,command,"string");
%% Data Processing
time = timeData - timeData(1);
time = time(1:end-1,:);
[r,c] = size(time);
index = 1;
for i = 1:r
    for j = 1:c
        y(index) = time(i,j);
        x(index) = index;
        index = index +1;
    end
end
index = 1;
n = r*c;
figure(1)
plot(x(1:n),y(1:n))
xlabel("Sample Number")
xticks(1:100:n)
ylabel("Time Recorded by Teensy (milliseconds)")
%% Functions
function [timeData, photodiodeData] = startData(maxTime,device)
    sec = 1; collect = 1; p = 1;
    zeroArray = zeros(maxTime,50);
    startBit = zeroArray; stopBit = zeroArray;
    timeData = zeroArray; photodiodeData = zeroArray;
    thresholdFlag = zeroArray; sunFlag = zeroArray;
    temp1 = zeroArray; temp2 = zeroArray; temp3 = zeroArray;
    current1 = zeroArray; current2 = zeroArray;
    while(collect && (sec < maxTime))
        count = 1;
        while(collect && (count <= 50))
            startBit(sec,count) = read(device,1,"char");
            timeData(sec,count) = read(device,1,"uint32");
            photodiodeData(sec,count) = read(device,1,"uint16");
            thresholdFlag(sec,count) = read(device,1,"char");
            sunFlag(sec,count) = read(device,1,"char");
            temp1(sec,count) = read(device,1,"uint16");
            temp2(sec,count) = read(device,1,"uint16");
            temp3(sec,count) = read(device,1,"uint16");
            current1(sec,count) = read(device,1,"uint16");
            current2(sec,count) = read(device,1,"uint16");
            stopBit(sec,count) = read(device,1,"char");
            count = count + 1;
            [collect,p] = checkPropagate(p);
        end
        sec = sec + 1;
    end
end
function [collect,p_out] = checkPropagate(p_in)
    % p_in represents position input
    if(p_in>=100*50)
        fprintf("Data Collection Stopped by Orbit");
        collect = 0;
        p_out = p_in+1;
        return;
    end
    collect = 1;
    % p_out represents new orbital position
    p_out = p_in+1;
end
