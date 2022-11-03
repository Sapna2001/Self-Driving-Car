clc;
close all;
clear all;

img = imread('lane.jpg');
I = rgb2gray(img);
bw = ~imbinarize(I,'adaptive','ForegroundPolarity','dark','Sensitivity',0.35);
[L,~] = bwlabel(bw);
figure; imshow(img, []); hold on; 
h = imshow(label2rgb(L));
set(h, 'AlphaData', 0.5);
bw2 = imclose(bw, strel('disk', 5));
bw2 = bwareaopen(bw2, 20);
[L2,~] = bwlabel(bw2);
stats = regionprops(L2);
hold on;

for i = 1 : length(stats)
    hold on; rectangle('position', stats(i).BoundingBox, 'EdgeColor', 'g', 'LineWidth', 2)
end
