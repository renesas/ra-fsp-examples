%% Verify Filter Outputs
%  This script imports the binary file containing the IIRFA output as an 
%  array to visually verify the filter operation in the time and frequency 
%  domain.  

%% Filter noisy signal with MATLAB filter

% Load the single-period noisy signal and extend the signal to 16 periods 
noise = csvread('noise.txt');
noise2 = cat(2, noise, noise);
noise4 = cat(2, noise2, noise2);
noise8 = cat(2, noise4, noise4);
noise16 = cat(2, noise8, noise8);

% Load the MATLAB filter object to the workspace
load('Hbp.mat');

% Filter the noisy signal 
output_matlab = filter(Hbp, noise16);


%% Import binary file containing IIRFA output array
fileID = fopen('output_iirfa.bin');
output = fread(fileID, 'float32');
output_iirfa = output.';


%% Plot and compare results in the time domain
n = length(output_iirfa);
i = 0:(10/256):160-(10/256); %this is milliseconds

figure(1);
subplot(2,1,1);
plot(i, noise16, 'k');
title('Noisy Input in Time Domain');
xlabel('time (ms)');


subplot(2,1,2);
hold on;
plot(i, output_iirfa, 'b');
plot(i, output_matlab, 'r--');
xlabel('time (ms)');
legend('IIRFA' , 'MATLAB', 'Location', 'northwest');
title('IIRFA vs MATLAB Output in Time Domain')
hold off;

%% Plot the frequency spectrum of input and output signals of IIRFA

% Computing the single-sided amplitude spectrum of noisy input
Fs = 25600;
L = 16*256;

Y_n = fft(noise16);
P2_n = abs(Y_n/L);
P1_n = P2_n(1:L/2+1);
P1_n(2:end-1) = 2*P1_n(2:end-1);
f_n = Fs*(0:(L/2))/L;

% Computing the single-sided amplitude spectrum of IIRFA output
Y_o = fft(output_iirfa);
P2_o = abs(Y_o/L);
P1_o = P2_o(1:L/2+1);
P1_o(2:end-1) = 2*P1_o(2:end-1);
f_o = Fs*(0:(L/2))/L;

% Frequency plots
figure(2);
subplot(2,1,1);
plot(f_n,P1_n/2048);
title('Single-Sided Frequency Spectrum of Noisy Input')
xlabel('f (Hz)')
ylabel('|P1(f)|')

subplot(2,1,2);
plot(f_o,P1_o/2048);
title('Single-Sided Frequency Spectrum of IIRFA Output')
xlabel('frequency (Hz)')
ylabel('|P1(f)|')
