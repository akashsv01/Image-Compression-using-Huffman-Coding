//This is a project for representing Image compression using Huffman Coding
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Function to concatenate two words based on condition like adding '0' or '1' to the encoded text
void stringconcat(char* str, char* parentcode, char add);
// This Function would be useful for calculating the maximal codeword length in Huffmann Codes
int fibo(int n);

void delay(float number_of_seconds)
{
    int ms = 1000 * number_of_seconds;
    clock_t start_time = clock();
    while (clock() < start_time + ms);
}

int main()
{
    int menu=0;
    int counter=0;
    int avg_perc=0;
    while(menu==0)
    {
        printf("\n**********************************************\n");
        printf("\tWELCOME TO THE IMAGE COMPRESSOR\n");
        printf("**********************************************\n");
        printf("THIS TOOL USES HUFFMAN CODING TO COMPRESS THE IMAGE GIVEN IN .BMP FORMAT.\n\n");
        int i, j;
        char filename[50]; 
        printf("Please enter the image you want to compress : ");
        scanf("%s",filename);
        int data = 0, offset, bpp = 0, width, height;
        long bmpsize = 0, bmpdataoff = 0;
        int** image;
        int temp = 0;
        clock_t  start,end;
        double time_taken;

        //Reading the BMP File
        FILE* image_file;
        image_file = fopen(filename, "rb");
        if (image_file == NULL) 
        {
            printf("Error Opening File!!");
            exit(1);
        }
        else
        {
            start = clock();
            // Set file position of the stream to the beginning which contains file signature "BM" for bitmap images
            printf("Processing BMP Header");
            for(int j=0;j<6;j++)
            {
                printf(".");
                delay(1);
            }
            printf("\n");

            //Set file position to the beginning which contains the ID  of the image "BM"
            offset = 0; 
            fseek(image_file, offset, SEEK_SET);
            //Printing the ID of the file "BM" (if it is a .bmp file)
            for(i = 0 ; i < 2 ; i++)
            {
                fread(&data, 1, 1, image_file);
                printf("%c",data);
            }
            printf("\n");

            // Set file position/offset to 2, which contains size of BMP File
            offset = 2; 
            fseek(image_file, offset, SEEK_SET);
            // Getting size of BMP File
            fread(&bmpsize, 4, 1, image_file);
            
            // Getting offset where the pixel array starts, since the information is at offset 10 from the start, as given in BMP Header
            offset = 10; 
            fseek(image_file, offset, SEEK_SET);
            // Bitmap data offset
            fread(&bmpdataoff, 4, 1, image_file);
            
            // Getting height and width of the image
            // Width is stored at offset 18 and 
            // Height at offset 22, each of 4 bytes
            fseek(image_file, 18, SEEK_SET);
            fread(&width, 4, 1, image_file);
            fread(&height, 4, 1, image_file);
            
            // Number of bits per pixel
            fseek(image_file, 2, SEEK_CUR);
            fread(&bpp, 2, 1, image_file); 

            long long int no_of_bits = width*height*bpp;
            printf("Number of bits in the original BMP image is %d bits.\n",no_of_bits);
            printf("Number of bits per pixel is : %d bits.\n",bpp);
            
            // Setting offset to the start of pixel data
            fseek(image_file, bmpdataoff, SEEK_SET);
            
            // Creating Image array
            image = (int**)malloc(height * sizeof(int*));
            for (i = 0; i < height; i++)
            {
                image[i] = (int*)malloc(width * sizeof(int));
            }
            
            //Number of bytes in the image pixel array
            int numbytes = (bmpsize - bmpdataoff) / 3; 

            // Reading the BMP File into image[] array
            for (i = 0; i < height; i++) 
            {
                for (j = 0; j < width; j++) 
                {
                    fread(&temp, 3, 1, image_file); 
                    // the Image is a 32-bit BMP Image
                    // 24 bits per pixel - Color
                    // 8 bits - Transparency
                    // 0x0000FF - 255 , since there are 256 pixel intensities - Thus, each pixel intensity value is between 0 and 256
                    temp = temp & 0x0000FF; 
                    image[i][j] = temp;
                }
            }
            end = clock();
        }
        time_taken = ((double)(end-start))/CLOCKS_PER_SEC;
        printf("Total time taken for processing BMP Image : %f seconds\n",time_taken-6); //Subtracted 6 due to the delay(1) given above.
        
        // Finding the probability of occurrence
        int hist[256];
        for (i = 0; i < 256; i++)
        {
            hist[i] = 0;
        }
            
        for (i = 0; i < height; i++)
        {
            for (j = 0; j < width; j++)
            {
                hist[image[i][j]] += 1;   
            }
        }
        
        //Finding number of non-zero occurrences, since all 256 intensities might not be present in the BMP image
        int nodes = 0;
        for (i = 0; i < 256; i++)
        {
            if (hist[i] != 0)
                nodes++;
        }
        
                
        // Calculating minimum probability among all probabilities of pixel intensities
        float p = 1.0, ptemp;
        for (i = 0; i < 256; i++) 
        {
            ptemp = (hist[i] / (float)(height * width));
            if (ptemp > 0 && ptemp <= p)
                p = ptemp;
        }
        
        //Calculating max length of Huffmann code word
        i = 0; 
        while ((1 / p) > fibo(i)) 
            i++;
        int maxcodelen = i - 3;
        
        //Declaring these 2 structs so that it contains the information of all leaf nodes in the Huffman Tree
        struct pixfreq
        {
            int pix;    
            float freq;
            struct pixfreq *left, *right;
            char code[maxcodelen];
        };
        
        struct sorted_pixfreq 
        {
            int pix, arrloc;
            float freq;
        };
        
        struct pixfreq* pix_freq;
        struct sorted_pixfreq* huffcodes;
        //If there are n leaf nodes, there are 2*n-1 nodes in the Huffman Tree
        int totalnodes = 2 * nodes - 1;
        pix_freq = (struct pixfreq*)malloc(sizeof(struct pixfreq) * totalnodes);
        huffcodes = (struct sorted_pixfreq*)malloc(sizeof(struct sorted_pixfreq) * nodes);
        
        j = 0;
        int totpix = height * width; //Total number of pixels
        float probability;
        for (i = 0; i < 256; i++)
        {
            if (hist[i] != 0) 
            {
                
                //Pixel intensity value
                huffcodes[j].pix = i; 
                pix_freq[j].pix = i;
                
                //Location of the node in the pix_freq array
                huffcodes[j].arrloc = j;
                
                //Probability of occurrence of each pixel intensity value
                probability = (float)hist[i] / (float)totpix; 
                pix_freq[j].freq = probability;
                huffcodes[j].freq = probability;
                
                //Declaring the child of leaf node as NULL pointer
                pix_freq[j].left = NULL; 
                pix_freq[j].right = NULL;
                
                pix_freq[j].code[0] = '\0'; 
                j++;
            }
        }
        
        //Sorting the struct sorted_pixfreq using a temporary variable
        struct sorted_pixfreq temphuff;
        
        //Sorting w.r.t probability of occurrence in descending order
        for (i = 0; i < nodes; i++)
        {
            for (j = i + 1; j < nodes; j++)
            {
                if (huffcodes[i].freq < huffcodes[j].freq) 
                {
                    temphuff = huffcodes[i];
                    huffcodes[i] = huffcodes[j];
                    huffcodes[j] = temphuff;
                }
            }
        }
        
        //Building Huffman Tree
        float combined_prob;
        int combined_pix;
        int n = 0, k = 0;
        int nextnode = nodes; //Used for appending values to the struct pixfreq
        
        while (n < nodes - 1) 
        {
            
            //Adding the lowest two probabilities
            combined_prob = huffcodes[nodes - n - 1].freq + huffcodes[nodes - n - 2].freq;
            combined_pix = huffcodes[nodes - n - 1].pix + huffcodes[nodes - n - 2].pix;
            
            //Appending to the pix_freq array
            pix_freq[nextnode].pix = combined_pix;
            pix_freq[nextnode].freq = combined_prob;
            pix_freq[nextnode].left = &pix_freq[huffcodes[nodes - n - 2].arrloc]; //Left child
            pix_freq[nextnode].right = &pix_freq[huffcodes[nodes - n - 1].arrloc]; //Right child
            pix_freq[nextnode].code[0] = '\0';
            i = 0;
        
            while (combined_prob <= huffcodes[i].freq)
                i++;
                
            // Inserting the new node in the huffcodes array   
            for (k = nodes; k >= 0; k--) 
            {
                if (k == i)
                {
                    huffcodes[k].pix = combined_pix;
                    huffcodes[k].freq = combined_prob;
                    huffcodes[k].arrloc = nextnode;            
                }
                //Else shifting the nodes to the right in huffcodes array
                else if (k > i)
                    huffcodes[k] = huffcodes[k - 1];
    
            }
            n += 1;
            nextnode += 1;
        }
        
        //Assigning Huffman Codes through backtracking
        char left = '0';
        char right = '1';
        int index;
        for (i = totalnodes - 1; i >= nodes; i--) 
        {
            if (pix_freq[i].left != NULL)
                stringconcat(pix_freq[i].left->code, pix_freq[i].code, left);
            if (pix_freq[i].right != NULL)
                stringconcat(pix_freq[i].right->code, pix_freq[i].code, right);
        }
        
        // Encode the Image
        int pix_val;
        int l;
        
        // Writing the Huffman encoded image into a text file "encoded_image.txt"
        FILE* imagehuff = fopen("encoded_image.txt", "wb");
        int res_len = 0;
        for (i = 0; i < height; i++)
            for (j = 0; j < width; j++) 
            {
                pix_val = image[i][j];
                for (l = 0; l < nodes; l++)
                    if (pix_val == pix_freq[l].pix)
                        fprintf(imagehuff, "%s", pix_freq[l].code);
                        res_len+= strlen(pix_freq[l].code);
            }
        

        printf("\nHUFFMAN CODES : \n\n");
        printf("PIXEL VALUES ==>   CODE\n\n");
        for (i = 0; i < nodes; i++) 
        {
            if (snprintf(NULL, 0, "%d", pix_freq[i].pix) == 2)
            {
                printf("     %d      ==>  %s\n", pix_freq[i].pix, pix_freq[i].code);
            }      
            else
            {
                printf("    %d      ==>  %s\n", pix_freq[i].pix, pix_freq[i].code);
            }
            delay(0.10);
                
        }
        
        float avgbitnum = 0;
        for (i = 0; i < nodes; i++)
            avgbitnum += pix_freq[i].freq * strlen(pix_freq[i].code);
        printf("Average number of bits per pixel : %f", avgbitnum);
        printf("\nNumber of bits in the encoded text is %d.\n",res_len);
        int ind_perc = ((width*height*32) - res_len)*100/(width*height*32);
        printf("Percentage of data saved : %d \n",((width*height*32) - res_len)*100/(width*height*32));
        avg_perc+=ind_perc;
        //Number of iterations
        counter++;
        printf("ENTER 0 TO RUN THE TOOL AGAIN OR 1 TO EXIT - ");
        scanf("%d",&menu);
        if(menu==1)
        {
            printf("AVERAGE PERCENTAGE SAVED OVER ALL IMAGES IS : %d ",avg_perc/counter);
            printf("\n\n");
            return 0;
        }

    }

}

void stringconcat(char* str, char* parentcode, char add)
{
    int i = 0;
    while (*(parentcode + i) != '\0') 
    {
        *(str + i) = *(parentcode + i);
        i++;
    }
    if (add != '2') 
    {
        str[i] = add;
        str[i + 1] = '\0';
    }
    else
        str[i] = '\0';
}

int fibo(int n)
{
    if (n <= 1)
        return n;
    return fibo(n - 1) + fibo(n - 2);
}

