clc; clear; close all;

% Lucca Trapani

% 2/19/23

% This code parses the data received over Serial.  The data received is in
% the form of a .mat file with each column containing ASCII characters, and
% each row is at a timestep.

% Data naming convention:
% 'NS4Data_[Day]_[Month]_[Year]_[TestDescription].mat
fileOut = 'NS4Data_02_19_23_ParseFirstTest.mat';

% Load data:
load('data.mat');
rawData = dataUseful;
timeSteps = height(rawData);
startCol = 1;
stopCol = 20;
startOut = zeros(timeSteps, 1);
stopOut = zeros(timeSteps, 1);
timeCol = 2:5;
timeOut = zeros(timeSteps, 1);
pdCol = 6:7;
pdOut = timeOut;
thresholdCol = 8;
thresholdOut = timeOut;
sunCol = 9;
sunOut = timeOut;
pdThermCol = 10:11;
pdThermOut = timeOut;
analogThermCol = 12:13;
analogThermOut = timeOut;
digitalThermCol = 14:15;
digitalThermOut = timeOut;
analogCurrentCol = 16:17;
analogCurrentOut = timeOut;
digitalCurrentCol = 18:19;
digitalCurrentOut = timeOut;
% Parse!

for i = 1:timeSteps
    startOut(i) = NSparse(rawData(i, :), startCol);
    stopOut(i) = NSparse(rawData(i, :), stopCol);
    timeOut(i) =  NSparse(rawData(i, :), flip(timeCol));
    pdOut(i) =  NSparse(rawData(i, :), flip(pdCol));
    thresholdOut(i) =  NSparse(rawData(i, :), thresholdCol);
    sunOut(i) =  NSparse(rawData(i, :), sunCol);
    pdThermOut(i) =  NSparse(rawData(i, :), pdThermCol);
    analogThermOut(i) =  NSparse(rawData(i, :), flip(analogThermCol));
    digitalThermOut(i) =  NSparse(rawData(i, :), flip(digitalThermCol));
    analogCurrentOut(i) =  NSparse(rawData(i, :), flip(analogCurrentCol));
    digitalCurrentOut(i) =  NSparse(rawData(i, :), flip(digitalCurrentCol));
end
timeOut = timeOut - timeOut(1);

% Save data
save(fileOut, 'timeOut', 'pdOut', 'thresholdOut', 'sunOut', 'pdThermOut', 'analogThermOut', ...
    'digitalThermOut', 'analogCurrentOut', 'digitalCurrentOut');

%% Function
function [parsedDataOut] = NSparse(data, columns)
    % ASCII col vector
    binVal = '';
    
    for ii = 1:length(columns)
        binVal = [binVal, dec2bin(data(columns(ii)), 8)];
    end
    parsedDataOut = bin2dec(binVal);
end
