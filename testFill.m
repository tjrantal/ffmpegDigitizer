fclose all;
try
	close all;
catch
	%Do nothing...
end_try_catch;
clear all;


%Region grow function
function areaCoordinates = growRegion(I, x,  y)
	markerColor = double(squeeze(I(int32(y),int32(x),:)));
	imageWidth =size(I,2); 
	imageHeight =size(I,1);
	neighbourhoodSize = 4;
	visited = zeros(imageWidth,imageHeight,'int32');	
	pixelQueue = struct();
	pixelQueue(1).coordinates = [int32(x),int32(y)];
	areaCoordinates = [];
	disp([num2str(markerColor)]);
	while !isempty(fieldnames(pixelQueue))  %//Go through all cells in queue
		nextPixel = pixelQueue(1).coordinates;       % /*Get the pixel with the lowest cost and remove it from the queue*/
		if length(pixelQueue) ==1
			pixelQueue =  struct();
		else
			pixelQueue = pixelQueue(2:length(pixelQueue));
		end
		%keyboard;
		
		%/*In case the pixel has been visited subsequent to having been added*/
		while (visited(nextPixel(1),nextPixel(2)) == 1 && !isempty(fieldnames(pixelQueue)))
			nextPixel = pixelQueue(1).coordinates;        %/*Get the pixel with the lowest cost and remove it from the queue*/
			if length(pixelQueue) ==1
				pixelQueue =  struct();
			else
				pixelQueue = pixelQueue(2:length(pixelQueue));
			end
			%disp(['pQSize ' num2str(length(pixelQueue)) 'removing visited']);
		end
		disp([num2str(nextPixel(1)) ' ' num2str(nextPixel(2)) ]);
		if isempty(fieldnames(pixelQueue)) && visited(nextPixel(1),nextPixel(2)) == 1
			break;
		end
		visited(nextPixel(1),nextPixel(2)) = int32(1);
		%keyboard;
		areaCoordinates = [areaCoordinates; nextPixel];
		%/*        Add 4-connected neighbourhood to the queue, unless the
		%neighbourhood pixels have already been visited or are part of the
		%mask already*/
		neighbourhood = zeros(4,2);
		neighbourhood(1,1) = nextPixel(1)-1;        %/*Left one*/
		neighbourhood(2,1) = nextPixel(1)+1;        %/*Right one*/
		neighbourhood(3,1) = nextPixel(1);
		neighbourhood(4,1) = nextPixel(1);
		
		neighbourhood(1,2) = nextPixel(2);
		neighbourhood(2,2) = nextPixel(2);
		neighbourhood(3,2) = nextPixel(2)-1;        %/*Up one*/
		neighbourhood(4,2) = nextPixel(2)+1;        %/*Down one*/

		%//checkNeighbours and add to queue;
        for r = 1:neighbourhoodSize
			coordinates = neighbourhood(r,:);
			%//Check that the neighbour is within the image.
            if coordinates(1) > 0 && coordinates(1) <= imageWidth && coordinates(2) >0 && coordinates(2) <= imageHeight
				%//Add to queue if the neighbour has not been visited, and is of marker color
				pixelColor = double(squeeze(I(coordinates(2),coordinates(1),:)));
				if (visited(coordinates(1),(2)) == 0 && ...
					abs(pixelColor(1)-markerColor(1)) < 17 && ...
					abs(pixelColor(2)-markerColor(2)) < 17 && ...
					abs(pixelColor(3)-markerColor(3)) < 17
					)
					if isempty(fieldnames(pixelQueue))
						pixelQueue(1).coordinates = coordinates;
					else
						pixelQueue(length(pixelQueue)+1).coordinates = coordinates;
					end
				end
            end
        end
		%disp(['pQSize ' num2str(length(pixelQueue))]);
		%disp(num2str(length(pixelQueue(1).coordinates)));
	end
endfunction;


clc;

ginputGraphTKit = 1;

I = imread("firstFrame.jpg");
if ginputGraphTKit
graphics_toolkit fltk;
figure("__graphics_toolkit__","fltk");
else
	graphics_toolkit gnuplot;
	figure("__graphics_toolkit__","gnuplot");
end

imH = imshow(I);
drawnow();
if ginputGraphTKit
	[x y] = ginput(1);
else
	x = 205;
	y = 97;
end
%region grow
grownAreaCoordinates = growRegion(I, x,  y);
disp(['Area grown ' num2str(size(grownAreaCoordinates))]);
%Visualize fill
for i = 1:size(grownAreaCoordinates,1)
	I(grownAreaCoordinates(i,2),grownAreaCoordinates(i,1),:) = uint8([255 0 0]);
end
if 0
graphics_toolkit fltk;
figure("__graphics_toolkit__","fltk");
else
	graphics_toolkit gnuplot;
	figure("__graphics_toolkit__","gnuplot");
end

imH = imshow(I);
drawnow();

