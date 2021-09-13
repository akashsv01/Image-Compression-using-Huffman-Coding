# Image-Compression-using-Huffman-Coding

➼ **OBJECTIVE**

To compress multiple bitmap (.bmp) images and calculate the average percentage of data saved.

➼ **ABSTRACT**

Images play an indispensable role in representing vital information. Thus, it needs to be saved for further use or must be transmitted over a medium. In order to have efficient utilization of disk space and transmission rate, images need to be compressed. Image compression is the technique of reducing the file size of a image without compromising with the image quality at acceptable level and is one of the most important steps in image transmission and storage.

Huffman coding is regarded as one of the most successful lossless compression techniques. It is based on the frequency of occurrence of a data item (pixel in images) and provides the least amount of information bits per source symbol. In this project, an image (in .bmp format) is taken as an input and using Huffman Coding, each pixel (in binary format) is compressed and then the percentage of compression is calculated. In addition to this, the encoded text is written in a text file.

➼ **PROCEDURE**

• The first step of Huffman coding technique is to read the Bitmap image (which is in .bmp format) into a 2D array.

• The second step is to reduce the input image to a ordered histogram, where the probability of occurrence of pixel intensity values present in the image are stored.

• Find the number of pixel intensity values having non-zero probability of occurrence and calculate the maximum length of Huffman code words.

• Build the Huffman Tree.

• Backtrack from the root to the leaf nodes to assign code words.

• Encode the image and write the Huffman encoded image to a text file. 

• Print the Huffman codes.

• Calculate the percentage of data saved for each image and display the average percentage of the data saved.


