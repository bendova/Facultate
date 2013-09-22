function tema1
f(1) = dir('documentatie_UMAPID.doc');
f(2) = dir('documentatie_UMAPID.pdf');
f(3) = dir('Prefata_Undine.txt');
f(4) = dir('show_audio.m');
f(5) = dir('Y04.M');
f(6) = dir('instr_01.wav');
f(7) = dir('speech_01.wav');
f(8) = dir('sound_01.wav');
f(9) = dir('KARMA_DATA482#1_5_V7.mat');
f(10) = dir('quartz.dll');
f(11) = dir('WinRar.exe');
f(12) = dir('WINZIP32.EXE');

for i = 1:12
    disp(strcat('file: ', f(i).name));
    disp(strcat('size[bytes]: ', int2str(f(i).bytes)));
end
