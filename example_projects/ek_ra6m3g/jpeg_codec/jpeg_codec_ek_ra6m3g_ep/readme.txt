/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This Example Project demonstrates the basic usage of JPEG Codec driver. 
	The project converts raw image of RGB565 format to YCbCr format and encodes it.
	The encoded image is then decoded into RGB565 format.
        Encoded and decoded images can be viewed using Image viewer option in e2studio.
	
NOTE:
1.Add breakpoints after the completion of encoding and decoding process.
2.Add image viewer for the buffers in e2studio and view the encoded and decoded images.
       For buffer with encoded data:
           Rendering - Image
       For buffer with decoded data:
           Rendering - Raw Image
           width - 240
           height - 320
           Encoding format - RGB565
Image viewer is added by:
  i.  Add the buffer to the "Memory" monitor feature in Debug window
  ii. Choose "New Rendering" option in Memory monitor and add rendering as mentioned above for
      the respective buffers.