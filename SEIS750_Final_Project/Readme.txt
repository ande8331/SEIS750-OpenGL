All executables are located in teh Debug folder.

*Encoder - Program to encode text into an image.  Best run from the command line.
Usage:
encoder.exe <input_image file path> <input text file path> <output image file path> <number of red bits to use> <number of green bits to use> <number of blue bits to use>

Example: 
C:\Users\Ross\Dropbox\SEIS750_Final_Project\Debug>Encoder.exe helo.png mlbrules.txt helo_mlb.png 4 2 2


*Decoder - Program to decode text from an image.  Best run from the command line.
Usage:
decoder.exe <input image file path> <output text file path> <number of red bits used> <number of green bits used> <number of blue bits used>

Example: 
C:\Users\Ross\Dropbox\SEIS750_Final_Project\Debug>Decoder.exe helo_mlb.png output.txt 4 2 2 

*Filter - Program to view each bit plane of an image.  Best run from the command line.
Usage:
filters.exe <input image file path>

Example:
C:\Users\Ross\Dropbox\SEIS750_Final_Project\Debug>Filters.exe helo.png

Keys: <r/b/g> Clear the respective bit mask
      <R/B/G> Shift the bitmask left one position

*Presentation - Contains Powerpoint presentation and images embedded in presentation.

*Writeup - Contains Word doc description of project and findings.
