img = imread('architecture.png');         %# A sample image
imshow(img,'Border','tight',...      %# Display in a figure window without
       'InitialMagnification',100);  %#    a border at full magnification
print('architecture.eps','-depsc2','-r300');
