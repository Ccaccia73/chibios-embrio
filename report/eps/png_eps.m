img = imresize(imread('DSCF2211.JPG'),0.5);         %# A sample image
imshow(img,'Border','tight',...      %# Display in a figure window without
       'InitialMagnification',100);  %#    a border at full magnification
print('DSCF2211.eps','-depsc2','-r300');
